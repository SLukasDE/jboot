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

#ifndef JBOOT_CONFIG_SETTING_H_
#define JBOOT_CONFIG_SETTING_H_

#include <jboot/config/Config.h>

#include <tinyxml2/tinyxml2.h>

#include <string>
#include <ostream>

namespace jboot {
namespace config {

class Setting : public Config {
public:
	Setting(const std::string& fileName, const tinyxml2::XMLElement& element, bool isParameter);

	void saveParameter(std::ostream& oStream, std::size_t spaces) const;

	std::string key;
	std::string value;
	std::string language;
};

} /* namespace config */
} /* namespace jboot */

#endif /* JBOOT_CONFIG_SETTING_H_ */
