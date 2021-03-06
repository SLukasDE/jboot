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

#include <jboot/config/logging/Layout.h>
#include <jboot/config/XMLException.h>

#include <esl/plugin/Registry.h>

#include <utility>

namespace jboot {
namespace config {
namespace logging {

Layout::Layout(const std::string& fileName, const tinyxml2::XMLElement& element)
: Config(fileName, element)
{
	if(element.GetUserData() != nullptr) {
		throw XMLException(*this, "Element has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		if(std::string(attribute->Name()) == "id") {
			if(id != "") {
				throw XMLException(*this, "Multiple definition of attribute 'id'");
			}
			id = attribute->Value();
			if(id == "") {
				throw XMLException(*this, "Invalid value \"\" for attribute 'id'");
			}
		}
		else if(std::string(attribute->Name()) == "implementation") {
			if(implementation != "") {
				throw XMLException(*this, "Multiple definition of attribute 'implementation'");
			}
			implementation = attribute->Value();
			if(implementation == "") {
				throw XMLException(*this, "Invalid value \"\" for attribute 'implementation'");
			}
		}
		else {
			throw XMLException(*this, "Unknown attribute '" + std::string(attribute->Name()) + "'");
		}
	}

	if(id == "") {
		throw XMLException(*this, "Missing attribute 'id'");
	}
	if(implementation == "") {
		throw XMLException(*this, "Missing attribute 'implementation'");
	}

	for(const tinyxml2::XMLNode* node = element.FirstChild(); node != nullptr; node = node->NextSibling()) {
		const tinyxml2::XMLElement* innerElement = node->ToElement();

		if(innerElement == nullptr) {
			continue;
		}

		parseInnerElement(*innerElement);
	}
}

const std::string& Layout::getId() const noexcept {
	return id;
}

void Layout::save(std::ostream& oStream, std::size_t spaces) const {
	oStream << makeSpaces(spaces) << "<layout id=\"" << id << "\" implementation=\"" << implementation << "\">\n";
	for(const auto& parameter : parameters) {
		parameter.saveParameter(oStream, spaces+2);
	}
	oStream << makeSpaces(spaces) << "</layout>\n";
}

std::unique_ptr<esl::logging::Layout> Layout::create() const {
	std::vector<std::pair<std::string, std::string>> eslSettings;

	for(auto const& setting : parameters) {
		eslSettings.push_back(std::make_pair(setting.key, setting.value));
	}

	return esl::plugin::Registry::get().create<esl::logging::Layout>(implementation, eslSettings);
}

void Layout::parseInnerElement(const tinyxml2::XMLElement& element) {
	if(element.Name() == nullptr) {
		throw XMLException(*this, "Element name is empty");
	}

	std::string elementName(element.Name());

	if(elementName == "parameter") {
		parameters.push_back(Setting(getFileName(), element, false));
	}
	else {
		throw XMLException(*this, "Unknown element name \"" + elementName + "\"");
	}
}

} /* namespace logging */
} /* namespace config */
} /* namespace jboot */
