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

#include <jboot/config/context/Context.h>
#include <jboot/config/XMLException.h>
#include <jboot/Logger.h>

#include <jboot/config/context/EntryImpl.h>

#include <esl/Module.h>
#include <esl/logging/appender/Appender.h>
#include <esl/logging/layout/Layout.h>
#include <esl/logging/Logger.h>

#include <iostream>
#include <stdexcept>

namespace jboot {
namespace config {
namespace context {

namespace {
Logger logger("jboot::config::context::Context");
} /* anonymous namespace */

Context::Context(const std::string& fileName, const tinyxml2::XMLElement& element)
: Config(fileName, element),
  isRoot(false)
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

Context::Context(const std::string& configuration)
: config::Config("{mem}"),
  isRoot(true)
{
	tinyxml2::XMLError xmlError = xmlDocument.Parse(configuration.c_str(), configuration.size());
	if(xmlError != tinyxml2::XML_SUCCESS) {
		throw XMLException(*this, xmlError);
	}

	const tinyxml2::XMLElement* element = xmlDocument.RootElement();
	if(element == nullptr) {
		throw XMLException(*this, "No root element");
	}

	setXMLFile(getFileName(), *element);
	loadXML(*element);
}

Context::Context(const boost::filesystem::path& filename)
: config::Config(filename.generic_string()),
  isRoot(true)
{
	filesLoaded.insert(filename.generic_string());

	tinyxml2::XMLError xmlError = xmlDocument.LoadFile(filename.generic_string().c_str());
	if(xmlError != tinyxml2::XML_SUCCESS) {
		throw XMLException(*this, xmlError);
	}

	const tinyxml2::XMLElement* element = xmlDocument.RootElement();
	if(element == nullptr) {
		throw XMLException(*this, "No root element");
	}

	setXMLFile(filename.generic_string(), *element);
	loadXML(*element);
}

void Context::save(std::ostream& oStream, std::size_t spaces) const {
	if(isRoot) {
		oStream << "\n<jboot>\n";

		for(const auto& entry : libraries) {
			oStream << "  <library file=\"" << entry.first << "\"/>\n";
		}

		for(const auto& entry : entries) {
			entry->save(oStream, 2);
		}

		oStream << "</jboot>\n";
	}
	else {
		oStream << makeSpaces(spaces) << "<context";

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

			oStream << makeSpaces(spaces) << "</context>\n";
		}
	}
}

void Context::install(boot::context::Context& context) {
	if(isRoot) {
		for(const auto& eslLogger : eslLoggers) {
			eslLogger.install();
		}

		for(const auto& entry : entries) {
			entry->install(context);
		}
	}
	else {
		if(refId.empty()) {
			context.addObject(id, create());
		}
		else {
			context.addReference(id, refId);
		}
	}
}

void Context::loadLibraries() {
	/* ************************
	 * load and add libraries *
	 * ********************** */
	for(auto& library : libraries) {
		/*
		if(library.second) {
			throw esl::addStacktrace(std::runtime_error(std::string("Library \"") + library.first + "\" loaded already."));
		}
		*/
		library.second = &esl::module::Library::load(library.first);
		library.second->install(esl::getModule());
	}
}

std::unique_ptr<esl::object::Interface::Object> Context::create() const {
	std::vector<std::pair<std::string, std::string>> eslSettings;
	for(const auto& setting : settings) {
		eslSettings.push_back(std::make_pair(setting.key, evaluate(setting.value, setting.language)));
	}

	std::unique_ptr<esl::boot::context::Interface::Context> bootContext;
	try {
		bootContext = esl::getModule().getInterface<esl::boot::context::Interface>(implementation).create(eslSettings);
	}
	catch(const std::exception& e) {
		throw XMLException(*this, e.what());
	}
	catch(...) {
		throw XMLException(*this, "Could not create a boot context with id '" + id + "' for implementation '" + implementation + "' because an unknown exception occurred.");
	}

	if(!bootContext) {
		throw XMLException(*this, "Could not create a boot context with id '" + id + "' for implementation '" + implementation + "'");
	}

	return std::unique_ptr<esl::object::Interface::Object>(bootContext.release());
}

void Context::parseInnerElement(const tinyxml2::XMLElement& element) {
	if(element.Name() == nullptr) {
		throw XMLException(*this, "Element name is empty");
	}

	const std::string elementName(element.Name());

	if(elementName == "include" && isRoot == true) {
		parseInclude(element);
	}
	else if(elementName == "library" && isRoot == true) {
		parseLibrary(element);
	}
	else if(elementName == "esl-logger" && isRoot == true) {
		eslLoggers.push_back(logging::Logger(getFileName(), element));
	}
	else if(elementName == "parameter" && isRoot == false) {
		settings.push_back(Setting(getFileName(), element, true));
	}
	else {
		entries.emplace_back(new EntryImpl(getFileName(), element));
	}
}

void Context::loadXML(const tinyxml2::XMLElement& element) {
	if(element.Name() == nullptr) {
		throw XMLException(*this, "Name of XML root element is empty");
	}

	const std::string elementName(element.Name());

	if(elementName != "jboot") {
		throw XMLException(*this, "Name of XML root element is \"" + std::string(element.Name()) + "\" but should be \"jboot\"");
	}

	if(element.GetUserData() != nullptr) {
		throw XMLException(*this, "Node has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		throw XMLException(*this, "Unknown attribute '" + std::string(attribute->Name()) + "'");
	}

	for(const tinyxml2::XMLNode* node = element.FirstChild(); node != nullptr; node = node->NextSibling()) {
		const tinyxml2::XMLElement* innerElement = node->ToElement();

		if(innerElement == nullptr) {
			continue;
		}

		auto oldXmlFile = setXMLFile(getFileName(), *innerElement);
		parseInnerElement(*innerElement);
		setXMLFile(oldXmlFile);
	}
}

void Context::parseInclude(const tinyxml2::XMLElement& element) {
	std::string fileName;

	if(element.GetUserData() != nullptr) {
		throw XMLException(*this, "Element has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		std::string attributeName = attribute->Name();

		if(attributeName == "file") {
			fileName = attribute->Value();
			if(fileName.empty()) {
				throw XMLException(*this, "Value \"\" of attribute 'file' is invalid.");
			}
		}
		else {
			throw XMLException(*this, "Unknown attribute '" + attributeName + "'");
		}
	}

	if(fileName.empty()) {
		throw XMLException(*this, "Missing attribute 'file'");
	}

	if(filesLoaded.count(fileName) == 0) {
		auto oldXmlFile = setXMLFile(fileName, -1);
		filesLoaded.insert(fileName);

		tinyxml2::XMLDocument doc;
		tinyxml2::XMLError xmlError = doc.LoadFile(fileName.c_str());
		if(xmlError != tinyxml2::XML_SUCCESS) {
			throw XMLException(*this, xmlError);
		}

		const tinyxml2::XMLElement* element = doc.RootElement();
		if(element == nullptr) {
			throw XMLException(*this, "No root element");
		}

		setXMLFile(fileName, *element);
		loadXML(*element);
		setXMLFile(oldXmlFile);
	}
}

void Context::parseLibrary(const tinyxml2::XMLElement& element) {
	std::string fileName;

	if(element.GetUserData() != nullptr) {
		throw XMLException(*this, "Element has user data but it should be empty");
	}

	for(const tinyxml2::XMLAttribute* attribute = element.FirstAttribute(); attribute != nullptr; attribute = attribute->Next()) {
		if(std::string(attribute->Name()) == "file") {
			fileName = attribute->Value();
			if(fileName == "") {
				throw XMLException(*this, "Value \"\" of attribute 'file' is invalid.");
			}
		}
		else {
			throw XMLException(*this, "Unknown attribute '" + std::string(attribute->Name()) + "'");
		}
	}

	if(fileName == "") {
		throw XMLException(*this, "Missing attribute 'file'");
	}

	libraries.push_back(std::make_pair(fileName, nullptr));
}

} /* namespace context */
} /* namespace config */
} /* namespace jboot */
