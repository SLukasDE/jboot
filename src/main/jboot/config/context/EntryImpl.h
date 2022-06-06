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

#ifndef JBOOT_CONFIG_CONTEXT_ENTRYIMPL_H_
#define JBOOT_CONFIG_CONTEXT_ENTRYIMPL_H_

#include <jboot/boot/context/Context.h>
#include <jboot/config/Config.h>
#include <jboot/config/context/BasicClient.h>
#include <jboot/config/context/BasicServer.h>
#include <jboot/config/context/Database.h>
#include <jboot/config/context/Entry.h>
#include <jboot/config/context/HttpClient.h>
#include <jboot/config/context/HttpServer.h>
#include <jboot/config/context/Object.h>
#include <jboot/config/context/Procedure.h>

#include <memory>
#include <ostream>

#include <tinyxml2/tinyxml2.h>

namespace jboot {
namespace config {
namespace context {

class EntryImpl : public Entry {
public:
	EntryImpl(const std::string& fileName, const tinyxml2::XMLElement& element);

	void save(std::ostream& oStream, std::size_t spaces) const override;
	void install(boot::context::Context& context) const override;

private:
	std::unique_ptr<Object> object;
	std::unique_ptr<Database> database;

	std::unique_ptr<Procedure> procedure;

	std::unique_ptr<BasicClient> basicClient;
	std::unique_ptr<BasicServer> basicServer;

	std::unique_ptr<HttpClient> httpClient;
	std::unique_ptr<HttpServer> httpServer;
};

} /* namespace context */
} /* namespace config */
} /* namespace jboot */

#endif /* JBOOT_CONFIG_CONTEXT_ENTRYIMPL_H_ */
