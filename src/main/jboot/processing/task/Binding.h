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
#include <esl/object/Event.h>
#include <esl/processing/Procedure.h>
#include <esl/processing/TaskDescriptor.h>
#include <esl/processing/TaskFactory.h>
#include <esl/processing/Status.h>
#include <esl/processing/Task.h>

#include <atomic>
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

class Binding final : public esl::processing::Task::Binding {
public:
	friend class Thread;
	friend class Task;

	Binding(TaskFactory& taskFactory, esl::processing::TaskDescriptor descriptor);

	void sendEvent(const esl::object::Object& object) override;
	void cancel() override;

	esl::processing::Status getStatus() const override;
	esl::object::Context* getContext() const override;
	std::exception_ptr getException() const override;

	void setStatus(esl::processing::Status status);

	/* called by Thread::run() */
	void run() noexcept;

private:
	mutable std::mutex taskFactoryMutex;
	TaskFactory* taskFactory;

	esl::processing::TaskDescriptor descriptor;
	esl::object::Event* event = nullptr;

	std::atomic<esl::processing::Status> status { esl::processing::Status::waiting };
	std::exception_ptr exceptionPtr;
};

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */

#endif /* JBOOT_PROCESSING_TASK_BINDING_H_ */
