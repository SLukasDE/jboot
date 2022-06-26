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

#include <jboot/config/context/BasicServer.h>
#include <jboot/config/XMLException.h>

#include <esl/com/basic/server/ISocket.h>
#include <esl/plugin/Registry.h>

#include <utility>

namespace jboot {
namespace config {
namespace context {

BasicServer::BasicServer(const std::string& fileName, const tinyxml2::XMLElement& element)
: Config(fileName, element)
{
	if(element.GetUserData() != nullptr) {
		throw XMLException(*this, "Element has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		std::string attributeName = attribute->Name();

		if(attributeName == "id") {
			if(!id.empty()) {
				throw XMLException(*this, "Multiple definitions of attribute 'id'.");
			}
			id = attribute->Value();
			if(id.empty()) {
				throw XMLException(*this, "Invalid value \"\" for attribute 'id'");
			}
		}
		else if(attributeName == "implementation") {
			if(!implementation.empty()) {
				throw std::runtime_error("Multiple definition of attribute 'implementation'.");
			}
			implementation = attribute->Value();
			if(implementation.empty()) {
				throw XMLException(*this, "Invalid value \"\" for attribute 'implementation'");
			}
			if(!refId.empty()) {
				throw XMLException(*this, "Attribute 'implementation' is not allowed together with attribute 'ref-id'.");
			}
		}
		else if(attributeName == "ref-id") {
			if(!refId.empty()) {
				throw XMLException(*this, "Multiple definition of attribute 'ref-id'");
			}
			refId = attribute->Value();
			if(refId.empty()) {
				throw XMLException(*this, "Invalid value \"\" for attribute 'ref-id'");
			}
			if(!implementation.empty()) {
				throw XMLException(*this, "Attribute 'ref-id' is not allowed together with attribute 'implementation'.");
			}
		}
		else {
			throw XMLException(*this, "Unknown attribute '" + attributeName + "'");
		}
	}

	if(id.empty()) {
		throw XMLException(*this, "Missing attribute 'id'");
	}
	if(refId.empty() && implementation.empty()) {
		throw XMLException(*this, "Attribute 'implementation' is missing.");
	}

	for(const tinyxml2::XMLNode* node = element.FirstChild(); node != nullptr; node = node->NextSibling()) {
		const tinyxml2::XMLElement* innerElement = node->ToElement();

		if(innerElement == nullptr) {
			continue;
		}

		auto oldXmlFile = setXMLFile(getFileName(), *innerElement);
		if(!refId.empty()) {
			throw XMLException(*this, "Definition of elements is not allowed together with attribute 'ref-id'.");
		}

		parseInnerElement(*innerElement);
		setXMLFile(oldXmlFile);
	}
}

void BasicServer::save(std::ostream& oStream, std::size_t spaces) const {
	oStream << makeSpaces(spaces) << "<basic-server";

	if(!id.empty()) {
		oStream << " id=\"" << id << "\"";
	}

	if(!implementation.empty()) {
		oStream << " implementation=\"" << implementation << "\"";
	}

	if(!refId.empty()) {
		oStream << " ref-id=\"" << refId << "\"";
	}

	if(settings.empty()) {
		oStream << "/>\n";
	}
	else {
		oStream << ">\n";

		for(const auto& setting : settings) {
			setting.saveParameter(oStream, spaces+2);
		}

		oStream << makeSpaces(spaces) << "</basic-server>\n";
	}
}


void BasicServer::install(boot::context::Context& context) const {
	if(refId.empty()) {
		context.addObject(id, create());
	}
	else {
		context.addReference(id, refId);

		if(context.findObject<esl::com::basic::server::ISocket>(refId) == nullptr) {
			if(id.empty()) {
				throw XMLException(*this, "Could not add basic server reference, because referenced object with id '" + refId + "' is not a basic server socket.");
			}
			else {
				throw XMLException(*this, "Could not add basic server reference with id '" + id + "', because referenced object with id '" + refId + "' is not a basic server socket.");
			}
		}
	}
	context.addObject(id, create());
}

std::unique_ptr<esl::object::IObject> BasicServer::create() const {
	std::vector<std::pair<std::string, std::string>> eslSettings;
	for(const auto& setting : settings) {
		eslSettings.push_back(std::make_pair(setting.key, evaluate(setting.value, setting.language)));
	}

	std::unique_ptr<esl::com::basic::server::ISocket> socket;
	try {
		socket = esl::plugin::Registry::get().getPlugin<esl::com::basic::server::ISocket::Plugin>(implementation).create(eslSettings);
	}
	catch(const std::exception& e) {
		throw XMLException(*this, e.what());
	}
	catch(...) {
		throw XMLException(*this, "Could not create a basic server socket with id '" + id + "' for implementation '" + implementation + "' because an unknown exception occurred.");
	}

	if(!socket) {
		throw XMLException(*this, "Could not create a basic server socket with id '" + id + "' for implementation '" + implementation + "' because interface method createSocket() returns nullptr.");
	}

	return std::unique_ptr<esl::object::IObject>(socket.release());
}

void BasicServer::parseInnerElement(const tinyxml2::XMLElement& element) {
	if(element.Name() == nullptr) {
		throw XMLException(*this, "Element name is empty");
	}

	std::string elementName(element.Name());

	if(elementName == "parameter") {
		settings.push_back(Setting(getFileName(), element, true));
	}
	else {
		throw XMLException(*this, "Unknown element name \"" + elementName + "\"");
	}
}

} /* namespace context */
} /* namespace config */
} /* namespace jboot */
