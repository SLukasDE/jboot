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

#include <jboot/config/context/Object.h>
#include <jboot/config/XMLException.h>

#include <esl/plugin/Registry.h>

#include <utility>

namespace jboot {
namespace config {
namespace context {

Object::Object(const std::string& fileName, const tinyxml2::XMLElement& element)
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

void Object::save(std::ostream& oStream, std::size_t spaces) const {
	oStream << makeSpaces(spaces) << "<object";

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

		oStream << makeSpaces(spaces) << "</object>\n";
	}
}

void Object::install(boot::context::Context& context) const {
	if(refId.empty()) {
		context.addObject(id, create());
	}
	else {
		context.addReference(id, refId);
	}
}

std::unique_ptr<esl::object::IObject> Object::create() const {
	std::vector<std::pair<std::string, std::string>> eslSettings;
	for(const auto& setting : settings) {
		eslSettings.push_back(std::make_pair(setting.key, evaluate(setting.value, setting.language)));
	}

	std::unique_ptr<esl::object::IObject> eslObject;
	try {
		eslObject = esl::plugin::Registry::get().getPlugin<esl::object::IObject::Plugin>(implementation).create(eslSettings);
	}
	catch(const std::exception& e) {
		throw XMLException(*this, e.what());
	}
	catch(...) {
		throw XMLException(*this, "Could not create an object with id '" + id + "' for implementation '" + implementation + "' because an unknown exception occurred.");
	}

	if(!eslObject) {
		throw XMLException(*this, "Could not create an object with id '" + id + "' for implementation '" + implementation + "'");
	}

	return eslObject;
}

void Object::parseInnerElement(const tinyxml2::XMLElement& element) {
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
