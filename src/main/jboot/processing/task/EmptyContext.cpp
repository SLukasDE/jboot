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

#include <jboot/processing/task/EmptyContext.h>

namespace jboot {
namespace processing {
namespace task {

std::set<std::string> EmptyContext::getObjectIds() const {
	return std::set<std::string>();
}

void EmptyContext::addRawObject(const std::string& id, std::unique_ptr<esl::object::Interface::Object> object) {
}

esl::object::Interface::Object* EmptyContext::findRawObject(const std::string& id) {
	return nullptr;
}

const esl::object::Interface::Object* EmptyContext::findRawObject(const std::string& id) const {
	return nullptr;
}

} /* namespace task */
} /* namespace processing */
} /* namespace jboot */
