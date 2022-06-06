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

#ifndef JBOOT_BOOT_LOGGING_CONFIG_H_
#define JBOOT_BOOT_LOGGING_CONFIG_H_

#include <boost/filesystem/path.hpp>

#include <string>

namespace jboot {
namespace boot {
namespace logging {

class Config final {
public:
	Config() = delete;

	static void loadData(const std::string& configuration);
	static void loadFile(const boost::filesystem::path& filename);
};

} /* namespace logging */
} /* namespace boot */
} /* namespace jboot */

#endif /* JBOOT_BOOT_LOGGING_CONFIG_H_ */