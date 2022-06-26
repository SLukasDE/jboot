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

#include <jboot/Logger.h>

#include <esl/logging/Config.h>

#include <iostream>
#include <map>
#include <stdexcept>
#include <sstream>
#include <utility>
#include <vector>

namespace jboot {

namespace {
std::map<std::string, std::unique_ptr<esl::logging::ILayout>> layouts;
std::vector<std::pair<std::string, std::unique_ptr<esl::logging::IAppender>>> appenders;
}

void Logger::flush(std::ostream& oStream) {
	for(auto& appender : appenders) {
		std::stringstream strStream;

		appender.second->flush();

		appender.second->flush(strStream);
		if(!strStream.str().empty()) {
			oStream << "\n\nFlush log messages from appender \"" << appender.first << "\":\n";
			oStream << strStream.str();
		}
	}
}

void Logger::flush(esl::logging::StreamReal& streamReal) {
	for(auto& appender : appenders) {
		std::stringstream strStream;

		appender.second->flush();

		appender.second->flush(strStream);
		if(!strStream.str().empty()) {
			streamReal << "\n\nFlush log messages from appender \"" << appender.first << "\":\n";
			streamReal << strStream.str();
		}
	}
}
/*
void Logger::flush() {
	for(auto& appender : appenders) {
		std::stringstream strStream;

		appender.second->flush();

		appender.second->flush(strStream);
		if(!strStream.str().empty()) {
			std::cerr << "\n\nFlush log messages from appender \"" << appender.first << "\":\n";
			std::cerr << strStream.str();
		}
	}
}
*/
void Logger::addLayout(const std::string& id, std::unique_ptr<esl::logging::ILayout> layout) {
	if(layouts.insert(std::make_pair(id, std::move(layout))).second == false) {
		throw std::runtime_error("Cannot add layout with id \"" + id + "\" because it exists already");
	}
}

void Logger::addAppender(const std::string& name, const std::string& layoutRefId, std::unique_ptr<esl::logging::IAppender> appender) {
	auto iter = layouts.find(layoutRefId);
	if(iter == std::end(layouts)) {
		throw std::runtime_error("Appender is referencing an undefined layout \"" + layoutRefId + "\"");
	}

	appender->setLayout(iter->second.get());

    esl::logging::Config::addAppender(*appender);
    appenders.push_back(std::make_pair(name, std::move(appender)));
}

} /* namespace jboot */




