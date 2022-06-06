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

#ifndef JBOOT_CONFIG_CONFIG_H_
#define JBOOT_CONFIG_CONFIG_H_

#include <tinyxml2/tinyxml2.h>

#include <string>
#include <utility>

namespace jboot {
namespace config {

class Config {
public:
	Config() = delete;
	Config(const std::string& fileName);
	Config(const std::string& fileName, const tinyxml2::XMLElement& element);
	virtual ~Config() = default;

	std::string evaluate(const std::string& expression, const std::string& language) const;

	const std::string& getFileName() const noexcept;
	int getLineNo() const noexcept;
	std::pair<std::string, int> getXMLFile() const noexcept;

protected:
	std::pair<std::string, int> setXMLFile(const std::string& fileName, int lineNo);
	std::pair<std::string, int> setXMLFile(const std::string& fileName, const tinyxml2::XMLElement& element);
	std::pair<std::string, int> setXMLFile(const std::pair<std::string, int>& xmlFile);

	std::string makeSpaces(std::size_t spaces) const;
	static bool stringToBool(bool& b, std::string str);

private:
	std::string fileName;
	int lineNo;
};

} /* namespace config */
} /* namespace jboot */

#endif /* JBOOT_CONFIG_CONFIG_H_ */
