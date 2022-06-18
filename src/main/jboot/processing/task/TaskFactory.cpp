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

std::unique_ptr<esl::processing::task::Interface::TaskFactory> TaskFactory::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::processing::task::Interface::TaskFactory>(new TaskFactory(settings));
}

TaskFactory::TaskFactory(const std::vector<std::pair<std::string, std::string>>& settings) {
    for(const auto& setting : settings) {
		if(setting.first == "max-threads") {
			if(maxThreads == 0) {
		        throw std::runtime_error("multiple definition of attribute 'max-threads'.");
			}
			maxThreads = std::stol(setting.second);
			if(maxThreads <= 0) {
	            throw std::runtime_error("jboot: Invalid value \"" + std::to_string(maxThreads) + "\" for attribute 'max-threads'.");
			}
		}
		else {
            throw std::runtime_error("unknown attribute '\"" + setting.first + "\"'.");
		}
    }

	if(maxThreads == 0) {
        throw std::runtime_error("Definition of 'max-threads' is missing.");
	}

}

TaskFactory::~TaskFactory() {
	{
		std::lock_guard<std::mutex> lockQueueMutex(queueMutex);
		for(auto& entry : queue) {
			entry.second->setStatus(esl::processing::task::Status::canceled);
		}
		queue.clear();
	}
	threadsCV.notify_all();
}

esl::processing::task::Task TaskFactory::createTask(esl::processing::task::Descriptor descriptor) {
	Binding* bindingPtr;
	std::shared_ptr<esl::processing::task::Task::Binding> binding;
	{
		std::unique_ptr<Binding> bindingTmp(new Binding(*this, std::move(descriptor)));
		bindingPtr = bindingTmp.get();
		binding = std::shared_ptr<esl::processing::task::Task::Binding>(bindingTmp.release());
	}

	{
		std::lock_guard<std::mutex> lockQueueMutex(queueMutex);
		queue.push_back(std::make_pair(binding, bindingPtr));
	}

	/*
	{
		std::lock_guard<std::mutex> lockThreadMutex(threadsMutex);
		threadsCV.notify_one();
	}
	*/
	threadsCV.notify_one();

	return esl::processing::task::Task(binding);
}

std::vector<esl::processing::task::Task> TaskFactory::getTasks() const {
	return std::vector<esl::processing::task::Task>();
}

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */
