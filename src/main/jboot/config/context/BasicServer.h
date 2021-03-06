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

#ifndef JBOOT_CONFIG_CONTEXT_BASICSERVER_H_
#define JBOOT_CONFIG_CONTEXT_BASICSERVER_H_

#include <jboot/config/Config.h>
#include <jboot/config/Setting.h>
#include <jboot/boot/context/Context.h>

#include <esl/object/Object.h>

#include <tinyxml2/tinyxml2.h>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace jboot {
namespace config {
namespace context {

class BasicServer : public Config {
public:
	BasicServer(const std::string& fileName, const tinyxml2::XMLElement& element);

	void save(std::ostream& oStream, std::size_t spaces) const;
	void install(boot::context::Context& context) const;

private:
	std::string id;
	std::string implementation;
	std::string refId;
	std::vector<Setting> settings;

	std::unique_ptr<esl::object::Object> create() const;
	void parseInnerElement(const tinyxml2::XMLElement& element);
};

} /* namespace context */
} /* namespace config */
} /* namespace jboot */

#endif /* JBOOT_CONFIG_CONTEXT_BASICSERVER_H_ */
