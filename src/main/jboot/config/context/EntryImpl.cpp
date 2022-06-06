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

#include <jboot/config/context/EntryImpl.h>
#include <jboot/config/XMLException.h>
#include <jboot/boot/context/Context.h>

namespace jboot {
namespace config {
namespace context {

EntryImpl::EntryImpl(const std::string& fileName, const tinyxml2::XMLElement& element)
: Entry(fileName, element)
{
	if(element.Name() == nullptr) {
		throw XMLException(*this, "Element name is empty");
	}

	std::string elementName(element.Name());

	if(elementName == "basic-client") {
		basicClient = std::unique_ptr<BasicClient>(new BasicClient(getFileName(), element));
	}
	else if(elementName == "basic-server") {
		basicServer = std::unique_ptr<BasicServer>(new BasicServer(getFileName(), element));
	}
	else if(elementName == "database") {
		database = std::unique_ptr<Database>(new Database(getFileName(), element));
	}
	else if(elementName == "http-client") {
		httpClient = std::unique_ptr<HttpClient>(new HttpClient(getFileName(), element));
	}
	else if(elementName == "http-server") {
		httpServer = std::unique_ptr<HttpServer>(new HttpServer(getFileName(), element));
	}
	else if(elementName == "object") {
		object = std::unique_ptr<Object>(new Object(getFileName(), element));
	}
	else if(elementName == "procedure") {
		procedure = std::unique_ptr<Procedure>(new Procedure(getFileName(), element));
	}
	else {
		throw XMLException(*this, "Unknown element name \"" + elementName + "\".");
	}
}

void EntryImpl::save(std::ostream& oStream, std::size_t spaces) const {
	if(basicClient) {
		basicClient->save(oStream, spaces);
	}
	if(basicServer) {
		basicServer->save(oStream, spaces);
	}
	if(database) {
		database->save(oStream, spaces);
	}
	if(httpClient) {
		httpClient->save(oStream, spaces);
	}
	if(httpServer) {
		httpServer->save(oStream, spaces);
	}
	if(object) {
		object->save(oStream, spaces);
	}
	if(procedure) {
		procedure->save(oStream, spaces);
	}
}

void EntryImpl::install(boot::context::Context& context) const {
	if(basicClient) {
		basicClient->install(context);
	}
	if(basicServer) {
		basicServer->install(context);
	}
	if(database) {
		database->install(context);
	}
	if(httpClient) {
		httpClient->install(context);
	}
	if(httpServer) {
		httpServer->install(context);
	}
	if(object) {
		object->install(context);
	}
	if(procedure) {
		procedure->install(context);
	}
}

} /* namespace context */
} /* namespace config */
} /* namespace jboot */
