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

#include <jboot/object/Context.h>

#include <esl/system/Stacktrace.h>

#include <stdexcept>
#include <utility>

namespace jboot {
namespace object {

std::set<std::string> Context::getObjectIds() const {
	std::set<std::string> rv;
	for(const auto& object : objects) {
		rv.insert(object.first);
	}
	return rv;
}

void Context::addRawObject(const std::string& id, std::unique_ptr<esl::object::Object> object) {
	if(objects.insert(std::make_pair(id, std::move(object))).second == false) {
		throw esl::system::Stacktrace::add(std::runtime_error("Cannot add element \"" + id + "\" to context because there exists already an object with same id"));
	}
}

esl::object::Object* Context::findRawObject(const std::string& id) {
	auto iter = objects.find(id);
	return iter == objects.end() ? nullptr : iter->second.get();
}

const esl::object::Object* Context::findRawObject(const std::string& id) const {
	auto iter = objects.find(id);
	return iter == objects.end() ? nullptr : iter->second.get();
}

} /* namespace object */
} /* namespace jboot */
