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

#ifndef JBOOT_CONFIG_CONTEXT_ENTRY_H_
#define JBOOT_CONFIG_CONTEXT_ENTRY_H_

#include <jboot/config/Config.h>
#include <jboot/boot/context/Context.h>

#include <string>
#include <ostream>

namespace jboot {
namespace config {
namespace context {

class Entry : public Config {
public:
	using Config::Config;

	virtual void save(std::ostream& oStream, std::size_t spaces) const = 0;
	virtual void install(boot::context::Context& context) const = 0;
};

} /* namespace context */
} /* namespace config */
} /* namespace jboot */

#endif /* JBOOT_CONFIG_CONTEXT_ENTRY_H_ */
