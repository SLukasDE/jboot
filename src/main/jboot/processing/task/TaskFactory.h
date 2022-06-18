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

#ifndef JBOOT_PROCESSING_TASK_TASKFACTORY_H_
#define JBOOT_PROCESSING_TASK_TASKFACTORY_H_

#include <jboot/processing/task/Binding.h>
#include <jboot/processing/task/Thread.h>

#include <esl/processing/task/Descriptor.h>
#include <esl/processing/task/Interface.h>
#include <esl/processing/task/Status.h>
#include <esl/processing/task/Task.h>
#include <esl/processing/procedure/Interface.h>

#include <chrono>
#include <condition_variable>
#include <functional>
#include <list>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace jboot {
namespace processing {
namespace task {

class TaskFactory final : public esl::processing::task::Interface::TaskFactory {
public:
	friend class Binding;
	friend class Thread;

	static std::unique_ptr<esl::processing::task::Interface::TaskFactory> create(const std::vector<std::pair<std::string, std::string>>& settings);

	TaskFactory(const std::vector<std::pair<std::string, std::string>>& settings);
	~TaskFactory();

	esl::processing::task::Task createTask(esl::processing::task::Descriptor descriptor) override;

	std::vector<esl::processing::task::Task> getTasks() const override;

private:
	long maxThreads = 0;

	std::mutex queueMutex;
	std::list<std::pair<std::shared_ptr<esl::processing::task::Task::Binding>, Binding*>> queue;

	std::condition_variable threadsCV;
	std::mutex threadsMutex;
	std::vector<std::unique_ptr<Thread>> threads;
};

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */

#endif /* JBOOT_PROCESSING_TASK_TASKFACTORY_H_ */
