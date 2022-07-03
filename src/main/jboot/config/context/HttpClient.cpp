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

#include <jboot/config/context/HttpClient.h>
#include <jboot/config/XMLException.h>

#include <esl/com/http/client/ConnectionFactory.h>
#include <esl/plugin/Registry.h>

#include <utility>

namespace jboot {
namespace config {
namespace context {

HttpClient::HttpClient(const std::string& fileName, const tinyxml2::XMLElement& element)
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

void HttpClient::save(std::ostream& oStream, std::size_t spaces) const {
	oStream << makeSpaces(spaces) << "<http-client";

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

		oStream << makeSpaces(spaces) << "</http-client>\n";
	}
}


void HttpClient::install(boot::context::Context& context) const {
	if(refId.empty()) {
		context.addObject(id, create());
	}
	else {
		context.addReference(id, refId);

		if(context.findObject<esl::com::http::client::ConnectionFactory>(refId) == nullptr) {
			if(id.empty()) {
				throw XMLException(*this, "Could not add http client reference, because referenced object with id '" + refId + "' is not a http client connection factory.");
			}
			else {
				throw XMLException(*this, "Could not add http client reference with id '" + id + "', because referenced object with id '" + refId + "' is not a http client connection factory.");
			}
		}
	}
	context.addObject(id, create());
}

std::unique_ptr<esl::object::Object> HttpClient::create() const {
	std::vector<std::pair<std::string, std::string>> eslSettings;
	for(const auto& setting : settings) {
		eslSettings.push_back(std::make_pair(setting.key, evaluate(setting.value, setting.language)));
	}

	std::unique_ptr<esl::com::http::client::ConnectionFactory> connectionFactory;
	try {
		connectionFactory = esl::plugin::Registry::get().create<esl::com::http::client::ConnectionFactory>(implementation, eslSettings);
	}
	catch(const std::exception& e) {
		throw XMLException(*this, e.what());
	}
	catch(...) {
		throw XMLException(*this, "Could not create a http client connection factory with id '" + id + "' for implementation '" + implementation + "' because an unknown exception occurred.");
	}

	if(!connectionFactory) {
		throw XMLException(*this, "Could not create a http client connection factory with id '" + id + "' for implementation '" + implementation + "' because interface method createConnectionFactory() returns nullptr.");
	}

	return std::unique_ptr<esl::object::Object>(connectionFactory.release());
}

void HttpClient::parseInnerElement(const tinyxml2::XMLElement& element) {
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
