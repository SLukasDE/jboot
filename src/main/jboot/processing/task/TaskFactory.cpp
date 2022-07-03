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

#include <jboot/processing/task/TaskFactory.h>

namespace jboot {
namespace processing {
namespace task {

std::unique_ptr<esl::processing::TaskFactory> TaskFactory::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::processing::TaskFactory>(new TaskFactory(settings));
}

TaskFactory::TaskFactory(const std::vector<std::pair<std::string, std::string>>& settings) {
    for(const auto& setting : settings) {
		if(setting.first == "max-threads") {
			if(threadsMax.load() > 0) {
		        throw std::runtime_error("multiple definition of attribute 'max-threads'.");
			}

			int tmpMaxThreads;
			try {
				tmpMaxThreads = std::stol(setting.second);
			}
			catch(...) {
	            throw std::runtime_error("jboot: Invalid value \"" + setting.second + "\" for attribute 'max-threads'.");
			}

			if(tmpMaxThreads <= 0 || tmpMaxThreads > 1000) {
	            throw std::runtime_error("jboot: Invalid value \"" + std::to_string(tmpMaxThreads) + "\" for attribute 'max-threads'. Value has to be between 1 and 1000.");
			}
			threadsMax.store(static_cast<unsigned int>(tmpMaxThreads));
		}
		else if(setting.first == "thread-timeout-ms") {
			if(hasThreadTimeout) {
		        throw std::runtime_error("multiple definition of attribute 'thread-timeout-ms'.");
			}
			hasThreadTimeout = true;
			//long threadTimeoutMs = static_cast<int>(std::stol(setting.second));
			long threadTimeoutMs = std::stol(setting.second);
			if(threadTimeoutMs < 1) {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for key 'thread-timeout-ms'. Value must be > 0");
			}
			threadTimeout = std::chrono::milliseconds(threadTimeoutMs);
		}
		else {
            throw std::runtime_error("unknown attribute '\"" + setting.first + "\"'.");
		}
    }

	if(threadsMax.load() == 0) {
        throw std::runtime_error("Definition of 'max-threads' is missing.");
	}

}

TaskFactory::~TaskFactory() {
	{
		std::lock_guard<std::mutex> lockQueueMutex(queueMutex);

		threadsMax.store(0);
		for(auto& entry : queue) {
			entry.first->setStatus(esl::processing::Status::canceled);
		}
		queue.clear();
	}

	threadsCV.notify_all();

	std::unique_lock<std::mutex> lockThreadsMutex(threadsMutex);
	threadsFinishedCV.wait(lockThreadsMutex, [&]() {
		return threadsAvailable == 0;
	});
}

esl::processing::Task TaskFactory::createTask(esl::processing::TaskDescriptor descriptor) {
	std::shared_ptr<esl::processing::Task::Binding> binding;

	{
		std::lock_guard<std::mutex> lockQueueMutex(queueMutex);

		std::unique_ptr<Binding> bindingTmp(new Binding(*this, std::move(descriptor)));
		Binding* bindingPtr = bindingTmp.get();
		binding = std::shared_ptr<esl::processing::Task::Binding>(bindingTmp.release());
		queue.push_back(std::make_pair(bindingPtr, binding));
	}


	{
		std::lock_guard<std::mutex> lockThreadMutex(threadsMutex);
		if(threadsProcessing.size() < threadsMax) {
			Thread::create(*this);
		}
	}

	threadsCV.notify_one();

	return esl::processing::Task(binding);
}

std::vector<esl::processing::Task> TaskFactory::getTasks() const {
	std::vector<esl::processing::Task> tasks;

	std::lock_guard<std::mutex> lockQueueMutex(queueMutex);
	for(auto& entry : queue) {
		tasks.push_back(esl::processing::Task(entry.second));
	}

	std::lock_guard<std::mutex> lockThreadMutex(threadsMutex);
	for(auto& entry : threadsProcessing) {
		tasks.push_back(esl::processing::Task(entry.second));
	}

	return tasks;
}

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */
