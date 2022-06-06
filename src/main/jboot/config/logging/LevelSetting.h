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

#ifndef JBOOT_CONFIG_LOGGING_LEVELSETTING_H_
#define JBOOT_CONFIG_LOGGING_LEVELSETTING_H_

#include <jboot/config/Config.h>

#include <esl/logging/Level.h>

#include <tinyxml2/tinyxml2.h>

#include <string>
#include <ostream>

namespace jboot {
namespace config {
namespace logging {

class LevelSetting : public Config {
public:
	LevelSetting(const std::string& fileName, const tinyxml2::XMLElement& element);

	esl::logging::Level getLevel() const;
	const std::string& getScope() const;

	void save(std::ostream& oStream, std::size_t spaces) const;

private:
	esl::logging::Level level = esl::logging::Level::SILENT;
	std::string scope;
};

} /* namespace logging */
} /* namespace config */
} /* namespace jboot */

#endif /* JBOOT_CONFIG_LOGGING_LEVELSETTING_H_ */
