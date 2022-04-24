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
 * License along with Jerry.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <jboot/logging/Config.h>

#include <jerry/config/logging/Logger.h>

namespace jboot {
namespace logging {

void Config::loadData(const std::string& configuration) {
	jerry::config::logging::Logger loggerConfig(configuration);
	loggerConfig.install();
}

void Config::loadFile(const boost::filesystem::path& filename) {
	jerry::config::logging::Logger loggerConfig(filename);
	loggerConfig.install();
}

} /* namespace logging */
} /* namespace jboot */
