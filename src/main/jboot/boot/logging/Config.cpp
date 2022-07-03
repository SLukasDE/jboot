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

#include <jboot/boot/logging/Config.h>
#include <jboot/config/logging/Logger.h>

#include <stdexcept>

namespace jboot {
namespace boot {
namespace logging {

std::unique_ptr<esl::boot::logging::Config> Config::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::boot::logging::Config>(new Config(settings));
}

Config::Config(const std::vector<std::pair<std::string, std::string>>& settings) {
    for(const auto& setting : settings) {
        throw std::runtime_error("unknown attribute '\"" + setting.first + "\"'.");
    }
}

void Config::addData(const std::string& configuration) {
	jboot::config::logging::Logger loggerConfig(configuration);
	loggerConfig.install();
}

void Config::addFile(const boost::filesystem::path& filename) {
	jboot::config::logging::Logger loggerConfig(filename);
	loggerConfig.install();
}

} /* namespace logging */
} /* namespace boot */
} /* namespace jboot */
