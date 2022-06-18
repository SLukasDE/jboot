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
#include <jboot/processing/task/TaskFactory.h>

namespace jboot {
namespace processing {
namespace task {

Binding::Binding(TaskFactory& aTaskFactory, esl::processing::task::Descriptor aDescriptor)
: taskFactory(&aTaskFactory),
  descriptor(std::move(aDescriptor))
{ }

void Binding::sendEvent(const esl::object::Interface::Object& object) {
}

void Binding::cancel() {
	std::lock_guard<std::mutex> lockTaskMutext(taskFactoryMutex);
	if(taskFactory) {
		std::lock_guard<std::mutex> lockQueueMutext(taskFactory->queueMutex);
		for(auto iter = taskFactory->queue.begin(); iter != taskFactory->queue.end(); ++iter) {
			if(iter->second == this) {
				taskFactory->queue.erase(iter);
				return;
			}
		}
	}
}

esl::processing::task::Status Binding::getStatus() const {
	return status;
}

esl::object::Context* Binding::getContext() const {
	switch(status) {
	case esl::processing::task::Status::canceled:
	case esl::processing::task::Status::exception:
	case esl::processing::task::Status::done:
		return descriptor.context.get();
	default:
		break;
	}
	return nullptr;
}

std::exception_ptr Binding::getException() const {
	return exceptionPtr;

}

void Binding::setStatus(esl::processing::task::Status aStatus) {
	if(status == aStatus) {
		return;
	}

	status = aStatus;

	if(status == esl::processing::task::Status::canceled) {
		taskFactory = nullptr;
	}

	if(descriptor.onStateChanged) {
		descriptor.onStateChanged(status);
	}
}

void Binding::run() {
	status = esl::processing::task::Status::running;
	descriptor.procedure->procedureRun(*descriptor.context);
	status = esl::processing::task::Status::done;
	taskFactory = nullptr;
}
} /* namespace task */
} /* namespace processing */
} /* namespace jboot */
