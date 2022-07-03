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
#include <esl/object/Object.h>

#include <map>
#include <memory>
#include <set>
#include <string>

#ifndef JBOOT_OBJECT_CONTEXT_H_
#define JBOOT_OBJECT_CONTEXT_H_

namespace jboot {
namespace object {

class Context : public esl::object::Context {
public:
	std::set<std::string> getObjectIds() const override;

protected:
	void addRawObject(const std::string& id, std::unique_ptr<esl::object::Object> object) override;
	esl::object::Object* findRawObject(const std::string& id) override;
	const esl::object::Object* findRawObject(const std::string& id) const override;

private:
	std::map<std::string, std::unique_ptr<esl::object::Object>> objects;
};

} /* namespace object */
} /* namespace jboot */

#endif /* JBOOT_OBJECT_CONTEXT_H_ */
