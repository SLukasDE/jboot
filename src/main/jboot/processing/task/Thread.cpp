/*
 * This file is part of JBoot framework.
 * Copyright (C) 2022 Sven Lukas
 *
 * JBoot is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * JBoot is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with JBoot.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <jboot/processing/task/Binding.h>
#include <jboot/processing/task/Thread.h>
#include <jboot/processing/task/TaskFactory.h>

#include <chrono>
#include <memory>

namespace jboot {
namespace processing {
namespace task {

void Thread::create(TaskFactory& taskFactory) {
	++taskFactory.threadsAvailable;
	//std::thread thread(&Thread::run, taskFactory);
	std::thread thread([&taskFactory]() {
		Thread t(taskFactory);
	});
	thread.detach();
}

Thread::Thread(TaskFactory& aTaskFactory)
: taskFactory(aTaskFactory)
{
	while(taskFactory.threadsMax.load() != 0) {
		while([this]() {
			if(taskFactory.threadsMax.load() == 0) {
				return false;
			}

			std::lock_guard<std::mutex> lockQueueMutex(taskFactory.queueMutex);
			return !taskFactory.queue.empty();
		}()) {
			Binding* bindingPtr;
			std::shared_ptr<esl::processing::task::Task::Binding> binding;

			{
				std::lock_guard<std::mutex> lockQueueMutex(taskFactory.queueMutex);
				if(taskFactory.queue.empty()) {
					break;
				}
				bindingPtr = taskFactory.queue.front().first;
				binding = taskFactory.queue.front().second;
				taskFactory.queue.pop_front();
			}

			/* Insert binding to threadsProcessing */
			{
				std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
				taskFactory.threadsProcessing.insert(std::make_pair(bindingPtr, std::move(binding)));
			}

			/* Run procedure by calling "run"-wrapper, to manage status, exceptions, ... */
			bindingPtr->run();

			/* Remove binding from threadsProcessing */
			{
				std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
				taskFactory.threadsProcessing.erase(bindingPtr);
			}
		}

		std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
		if(taskFactory.threadsCV.wait_for(lockThreadsMutex, taskFactory.threadTimeout, [this]() {
			return taskFactory.threadsMax.load() == 0 ||  !taskFactory.queue.empty();
		}) == false) {
			break;
		}
	}
}

Thread::~Thread() {
	{
		std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
		--taskFactory.threadsAvailable;
	}

	taskFactory.threadsFinishedCV.notify_one();

}

void Thread::run(TaskFactory& taskFactory) {
	Thread thread(taskFactory);
}

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */
