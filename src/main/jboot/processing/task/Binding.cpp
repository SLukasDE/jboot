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
#include <jboot/object/Context.h>
#include <jboot/processing/task/TaskFactory.h>

namespace jboot {
namespace processing {
namespace task {

Binding::Binding(TaskFactory& aTaskFactory, esl::processing::TaskDescriptor aDescriptor)
: taskFactory(&aTaskFactory),
  descriptor(std::move(aDescriptor)),
  event(dynamic_cast<esl::object::Event*>(descriptor.procedure.get()))
{ }

void Binding::sendEvent(const esl::object::Object& object) {
	if(event) {
		event->onEvent(object);
	}
}

void Binding::cancel() {
	std::lock_guard<std::mutex> lockTaskFactory(taskFactoryMutex);

	if(taskFactory == nullptr) {
		return;
	}

	{
		std::lock_guard<std::mutex> lockQueueMutext(taskFactory->queueMutex);
		for(auto iter = taskFactory->queue.begin(); iter != taskFactory->queue.end(); ++iter) {
			if(iter->first == this) {
				taskFactory->queue.erase(iter);
				setStatus(esl::processing::Status::canceled);
				return;
			}
		}
	}

	if(descriptor.procedure) {
		std::lock_guard<std::mutex> lockThreadsMutext(taskFactory->threadsMutex);
		if(taskFactory->threadsProcessing.count(this) != 0) {
			descriptor.procedure->procedureCancel();
		}
	}
}

esl::processing::Status Binding::getStatus() const {
	return status.load();
}

esl::object::Context* Binding::getContext() const {
	switch(getStatus()) {
	case esl::processing::Status::canceled:
	case esl::processing::Status::exception:
	case esl::processing::Status::done:
		return descriptor.context.get();
	default:
		break;
	}
	return nullptr;
}

std::exception_ptr Binding::getException() const {
	return exceptionPtr;

}

void Binding::setStatus(esl::processing::Status aStatus) {
	if(status.exchange(aStatus) == aStatus) {
		return;
	}

	if(aStatus == esl::processing::Status::canceled) {
		std::lock_guard<std::mutex> lockTaskFactory(taskFactoryMutex);
		taskFactory = nullptr;
	}

	if(descriptor.onStateChanged) {
		descriptor.onStateChanged(aStatus);
	}
}

void Binding::run() noexcept {
	try {
		setStatus(esl::processing::Status::running);
		if(!descriptor.context) {
			descriptor.context.reset(new object::Context);
		}
		if(descriptor.procedure) {
			descriptor.procedure->procedureRun(*descriptor.context);
		}
		setStatus(esl::processing::Status::done);
	}
	catch(...) {
		exceptionPtr = std::current_exception();
		try {
			setStatus(esl::processing::Status::exception);
		} catch(...) { }
	}

	std::lock_guard<std::mutex> lockTaskFactory(taskFactoryMutex);
	taskFactory = nullptr;
}

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */
