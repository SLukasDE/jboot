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

#include <memory>

namespace jboot {
namespace processing {
namespace task {

Thread::Thread(TaskFactory& aTaskFactory)
: taskFactory(aTaskFactory),
  thread(&Thread::run, this)
{
	threadMutex.lock();
	thread.detach();
	threadMutex.lock();
}

Thread::~Thread() {
	threadMutex.lock();
}

void Thread::run() {
	/* resume Thread::Thread */
	threadMutex.unlock();

	std::unique_lock<std::mutex> lockThreadsMutex(taskFactory.threadsMutex);
	while(true) {
		taskFactory.threadsCV.wait(lockThreadsMutex);
		Binding* bindingPtr;
		std::shared_ptr<esl::processing::task::Task::Binding> binding;

		{
			std::lock_guard<std::mutex> lockQueueMutex(taskFactory.queueMutex);
			if(taskFactory.queue.empty()) {
				break;
			}
			binding = taskFactory.queue.front().first;
			bindingPtr = taskFactory.queue.front().second;
			taskFactory.queue.pop_front();
		}

		try {
			bindingPtr->run();
		}
		catch(...) {

		}
//		lockThreadsMutex.unlock();
	}

	/* resume Thread::~Thread */
	threadMutex.unlock();
}

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */
