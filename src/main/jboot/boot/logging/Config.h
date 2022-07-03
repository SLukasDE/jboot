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

#include <esl/boot/logging/Config.h>

#include <boost/filesystem/path.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace jboot {
namespace boot {
namespace logging {

class Config : public esl::boot::logging::Config {
public:
	static std::unique_ptr<esl::boot::logging::Config> create(const std::vector<std::pair<std::string, std::string>>& settings);

	Config(const std::vector<std::pair<std::string, std::string>>& settings);

	void addData(const std::string& configuration) override;
	void addFile(const boost::filesystem::path& filename) override;
};

} /* namespace logging */
} /* namespace boot */
} /* namespace jboot */

#endif /* JBOOT_BOOT_LOGGING_CONFIG_H_ */
