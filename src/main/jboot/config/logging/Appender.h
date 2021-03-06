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

#ifndef JBOOT_CONFIG_LOGGING_APPENDER_H_
#define JBOOT_CONFIG_LOGGING_APPENDER_H_

#include <jboot/config/Config.h>
#include <jboot/config/Setting.h>

#include <esl/logging/Appender.h>

#include <tinyxml2/tinyxml2.h>

#include <memory>
#include <ostream>
#include <string>
#include <vector>

namespace jboot {
namespace config {
namespace logging {

class Appender : public Config {
public:
	Appender(const std::string& fileName, const tinyxml2::XMLElement& element);

	const std::string& getName() const;
	const std::string& getLayoutId() const;

	void save(std::ostream& oStream, std::size_t spaces) const;

	std::unique_ptr<esl::logging::Appender> create() const;

private:
	std::string name;
	esl::logging::Appender::RecordLevel recordLevel = esl::logging::Appender::RecordLevel::SELECTED;
	std::string layoutId;
	std::string implementation;
	std::vector<Setting> parameters;

	void parseInnerElement(const tinyxml2::XMLElement& element);
};

} /* namespace logging */
} /* namespace config */
} /* namespace jboot */

#endif /* JBOOT_CONFIG_LOGGING_APPENDER_H_ */
