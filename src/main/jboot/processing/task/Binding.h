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

#include <esl/object/Context.h>
#include <esl/processing/procedure/Interface.h>
#include <esl/processing/task/Descriptor.h>
#include <esl/processing/task/Interface.h>
#include <esl/processing/task/Status.h>
#include <esl/processing/task/Task.h>

#include <chrono>
#include <exception>
#include <functional>
#include <memory>
#include <mutex>

#ifndef JBOOT_PROCESSING_TASK_BINDING_H_
#define JBOOT_PROCESSING_TASK_BINDING_H_

namespace jboot {
namespace processing {
namespace task {

class TaskFactory;
class Thread;

class Binding final : public esl::processing::task::Task::Binding {
public:
	friend class Thread;
	friend class Task;

	Binding(TaskFactory& taskFactory, esl::processing::task::Descriptor descriptor);

	void sendEvent(const esl::object::Interface::Object& object) override;
	void cancel() override;

	esl::processing::task::Status getStatus() const override;
	esl::object::Context* getContext() const override;
	std::exception_ptr getException() const override;

	void setStatus(esl::processing::task::Status status);

	/* called by Thread::run() */
	void run();

private:
	std::mutex taskFactoryMutex;
	TaskFactory* taskFactory;

	esl::processing::task::Descriptor descriptor;

	esl::processing::task::Status status = esl::processing::task::Status::waiting;
	std::exception_ptr exceptionPtr;
};

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */

#endif /* JBOOT_PROCESSING_TASK_BINDING_H_ */
