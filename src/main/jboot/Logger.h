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

#ifndef JBOOT_LOGGER_H_
#define JBOOT_LOGGER_H_

#include <esl/logging/Appender.h>
#include <esl/logging/Layout.h>
#include <esl/logging/Level.h>
#include <esl/logging/Logger.h>
#include <esl/logging/StreamReal.h>
#include <esl/logging/StreamEmpty.h>

#include <memory>
#include <ostream>
#include <string>

namespace jboot {

#ifdef JBOOT_LOGGING_LEVEL_DEBUG
class Logger : public esl::logging::Logger<esl::logging::Level::TRACE> {
public:
	using esl::logging::Logger<esl::logging::Level::TRACE>::Logger;
#else
class Logger : public esl::logging::Logger<esl::logging::Level::ERROR> {
public:
	using esl::logging::Logger<esl::logging::Level::ERROR>::Logger;
#endif

	static void flush(std::ostream& oStream);
	static void flush(esl::logging::StreamReal& streamReal);
	//static void flush();
	static void addLayout(const std::string& id, std::unique_ptr<esl::logging::Layout> layout);
	static void addAppender(const std::string& name, const std::string& layoutRefId, std::unique_ptr<esl::logging::Appender> appender);
};

} /* namespace jboot */

#endif /* JBOOT_LOGGER_H_ */
