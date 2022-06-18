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

#include <set>
#include <string>
#include <memory>

#ifndef JBOOT_PROCESSING_TASK_EMPTYCONTEXT_H_
#define JBOOT_PROCESSING_TASK_EMPTYCONTEXT_H_

namespace jboot {
namespace processing {
namespace task {

class EmptyContext : public esl::object::Context {
public:
	std::set<std::string> getObjectIds() const override;

protected:
	void addRawObject(const std::string& id, std::unique_ptr<esl::object::Interface::Object> object) override;
	esl::object::Interface::Object* findRawObject(const std::string& id) override;
	const esl::object::Interface::Object* findRawObject(const std::string& id) const override;
};

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */

#endif /* JBOOT_PROCESSING_TASK_EMPTYCONTEXT_H_ */
