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

#include <jboot/boot/context/Context.h>
#include <jboot/config/context/Context.h>
#include <jboot/Logger.h>

#include <esl/com/http/server/exception/StatusCode.h>
#include <esl/database/exception/SqlError.h>
#include <esl/utility/String.h>

#include <stdexcept>
#include <sstream>

namespace jboot {
namespace boot {
namespace context {

namespace {
Logger logger("jboot::boot::context::Context");

void addException(esl::object::IContext& context, std::exception_ptr exceptionPtr) {
	esl::object::IObject* objectPtr = context.findObject<esl::object::IObject>("exception");
	esl::object::Value<std::exception_ptr>* exceptionObjectPtr = dynamic_cast<esl::object::Value<std::exception_ptr>*>(objectPtr);

	if(exceptionObjectPtr) {
		*exceptionObjectPtr = std::current_exception();
	}
	else if(objectPtr == nullptr) {
		context.addObject("exception", std::unique_ptr<esl::object::Value<std::exception_ptr>>(new esl::object::Value<std::exception_ptr>(std::current_exception())));
	}
}
} /* anonymous namespace */

std::unique_ptr<esl::boot::context::IContext> Context::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::boot::context::IContext>(new Context(settings));
}

Context::Context(const std::vector<std::pair<std::string, std::string>>& settings) {
    for(const auto& setting : settings) {
		if(setting.first == "handle-exception") {
			if(hasHandleException) {
		        throw std::runtime_error("multiple definition of attribute 'handle-exception'.");
			}
			hasHandleException = true;
			if(setting.second == "rethrow") {
				handleException = rethrow;
			}
			else if(setting.second == "stop") {
				handleException = stop;
			}
			else if(setting.second == "stop-and-show") {
				handleException = stopAndShow;
			}
			else if(setting.second == "ignore") {
				handleException = ignore;
			}
			else if(setting.second == "ignore-and-show") {
				handleException = ignoreAndShow;
			}
			else {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for attribute 'show-exception'");
			}
		}
		else if(setting.first == "show-stacktrace") {
			if(hasShowStacktrace) {
		        throw std::runtime_error("multiple definition of attribute 'show-stacktrace'.");
			}
			std::string value = esl::utility::String::toLower(setting.second);
			hasShowStacktrace = true;
			if(value == "true") {
				showStacktrace = true;
			}
			else if(value == "false") {
				showStacktrace = false;
			}
			else {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for attribute 'show-stacktrace'");
			}
		}
		else if(setting.first == "show-output") {
			if(showOutput) {
		        throw std::runtime_error("multiple definition of attribute 'show-output'.");
			}

			if(setting.second == "stdout") {
				showOutput.reset(new ShowOutput(std::cout));
			}
			else if(setting.second == "stderr") {
				showOutput.reset(new ShowOutput(std::cerr));
			}
			else if(setting.second == "trace") {
				showOutput.reset(new ShowOutput(logger.trace));
			}
			else if(setting.second == "debug") {
				showOutput.reset(new ShowOutput(logger.debug));
			}
			else if(setting.second == "info") {
				showOutput.reset(new ShowOutput(logger.info));
			}
			else if(setting.second == "warn") {
				showOutput.reset(new ShowOutput(logger.warn));
			}
			else if(setting.second == "error") {
				showOutput.reset(new ShowOutput(logger.error));
			}
			else {
		    	throw std::runtime_error("Invalid value \"" + setting.second + "\" for attribute 'show-output'");
			}
		}
		else {
            throw std::runtime_error("unknown attribute '\"" + setting.first + "\"'.");
		}
    }

	if(!showOutput && handleException == stopAndShow) {
        throw std::runtime_error("Definition of 'handle-exception' = 'stop-and-show' without definition of 'show-output'.");
	}
	if(!showOutput && handleException == ignoreAndShow) {
        throw std::runtime_error("Definition of 'handle-exception' = 'ignore-and-show' without definition of 'show-output'.");
	}
	if(showOutput && handleException != stopAndShow && handleException != ignoreAndShow) {
		logger.warn << "Definition of 'show-output' is useless if definition of 'handle-exception' is neither 'stop-and-show' nor 'ignore-and-show'";
	}
	if(hasShowStacktrace && showStacktrace && handleException != stopAndShow && handleException != ignoreAndShow) {
		logger.warn << "Definition of 'show-stacktrace' is useless if definition of 'handle-exception' is neither 'stop-and-show' nor 'ignore-and-show'";
	}
}

void Context::setParent(Context* parentContext) {
	parent = parentContext;

	if(!hasHandleException) {
		handleException = parent ? rethrow : showOutput ? stopAndShow : stop;
	}
}

esl::boot::context::IContext& Context::addData(const std::string& configuration) {
	config::context::Context config(configuration);

	config.loadLibraries();
	config.install(*this);

	return *this;
}

esl::boot::context::IContext& Context::addFile(const boost::filesystem::path& filename) {
	config::context::Context config(filename);

	config.loadLibraries();
	config.install(*this);

	return *this;
}

esl::boot::context::IContext& Context::addReference(const std::string& destinationId, const std::string& sourceId) {
	esl::object::IObject* object = findRawObject(sourceId);

	if(object == nullptr) {
        throw std::runtime_error("Cannot add reference with id '" + destinationId + "' because source object with id '" + sourceId + "' does not exists.");
	}

	if(destinationId.empty()) {
		entries.push_back(std::unique_ptr<Entry>(new Entry(*object)));
	}
	else {
		if(objects.count(destinationId) == 0) {
	        throw std::runtime_error("Cannot add reference with id '" + destinationId + "' because there exists already an object with same id.");
		}
		objects.insert(std::make_pair(destinationId, IdElement(*object)));
	}

	return *this;
}

int Context::getReturnCode() const {
	return returnCode;
}

void Context::onEvent(const esl::object::IObject& object) {
	initializeContext(*this);

#if 1
	for(auto& entry : entries) {
		entry->onEvent(object);
	}
#else
	for(auto& entry : entries) {
		if(handleException == rethrow) {
			entry->onEvent(object);
		}
		else {
			try {
				entry->onEvent(object);
			}
			catch(...) {
				if(showOutput && (handleException == stopAndShow || handleException == ignoreAndShow)) {
					if(showOutput->ostream) {
						printException(*showOutput->ostream, std::current_exception());
					}
					else if(showOutput->streamReal) {
						printException(*showOutput->streamReal, std::current_exception(), esl::logging::Location{});
					}
					else if(showOutput->streamEmpty) {
						printException(*showOutput->streamEmpty, std::current_exception(), esl::logging::Location{});
					}
				}

				if(handleException == stop || handleException == stopAndShow) {
					break;
				}
			}
		}
	}
#endif
}

std::set<std::string> Context::getObjectIds() const {
	std::set<std::string> rv;

	for(const auto& object : objects) {
		rv.insert(object.first);
	}

	return rv;
}

void Context::procedureRun(esl::object::IContext& context) {
	initializeContext(*this);

	for(auto& entry : entries) {
		if(handleException == rethrow) {
			entry->procedureRun(context);
		}
		else {
			try {
				entry->procedureRun(context);
			}
			catch(...) {
				if(showOutput && (handleException == stopAndShow || handleException == ignoreAndShow)) {
					if(showOutput->ostream) {
						Logger::flush(*showOutput->ostream);
						printException(*showOutput->ostream, std::current_exception());
					}
					else if(showOutput->streamReal) {
						Logger::flush(*showOutput->streamReal);
						printException(*showOutput->streamReal, std::current_exception(), esl::logging::Location{});
					}
					else if(showOutput->streamEmpty) {
//						printException(*showOutput->streamEmpty, std::current_exception(), esl::logging::Location{});
					}
				}

				if(handleException == stop || handleException == stopAndShow) {
					addException(context, std::current_exception());
					break;
				}
			}
		}
	}

	ReturnCodeObject* returnCodeObject = context.findObject<ReturnCodeObject>("return-code");
	if(returnCodeObject) {
		returnCode = **returnCodeObject;
	}
}

void Context::initializeContext(esl::object::IContext&) {
	for(auto& entry : entries) {
		entry->initializeContext(*this);
	}
	for(auto& object : objects) {
		if(object.second.initializeContext) {
			object.second.initializeContext->initializeContext(*this);
			object.second.initializeContext = nullptr;
		}
	}
}

esl::object::IObject* Context::findRawObject(const std::string& id) {
	auto iter = objects.find(id);
	esl::object::IObject* object = iter == std::end(objects) ? nullptr : &iter->second.refObject;
	if(object) {
		return object;
	}
	if(parent) {
		parent->findObject<esl::object::IObject>(id);
	}
	return nullptr;
}

const esl::object::IObject* Context::findRawObject(const std::string& id) const {
	auto iter = objects.find(id);
	esl::object::IObject* object = iter == std::end(objects) ? nullptr : &iter->second.refObject;
	if(object) {
		return object;
	}
	if(parent) {
		parent->findObject<esl::object::IObject>(id);
	}
	return nullptr;
}

void Context::addRawObject(const std::string& id, std::unique_ptr<esl::object::IObject> object) {
	Context* context = dynamic_cast<Context*>(object.get());

	if(id.empty()) {
		entries.push_back(std::unique_ptr<Entry>(new Entry(std::move(object))));
	}
	else {
		if(objects.count(id) == 0) {
	        throw std::runtime_error("Cannot add object with id '" + id + "' because there exists already an object with same id.");
		}
		objects.insert(std::make_pair(id, IdElement(std::move(object))));
	}

	if(context) {
		context->setParent(this);
	}
}


void Context::printException(std::ostream& stream, int level, const std::exception& e, const std::string& plainException, const std::string& plainDetails, const esl::system::stacktrace::IStacktrace* stacktrace) {
	std::string levelStr = "[" + std::to_string(level) + "]";

	stream << levelStr << " Exception : " << plainException << "\n";
	stream << levelStr << " What      : " << (e.what() ? e.what() : "") << "\n";
	if(plainDetails.empty() == false) {
		stream << levelStr << " Details   : " << plainDetails << "\n";
	}

	if(showStacktrace) {
		if(stacktrace) {
			stream << levelStr << " Stacktrace:\n";
			stacktrace->dump(stream);
		}
		else {
			stream << levelStr << " Stacktrace: not available\n";
		}
	}

	stream << "\n";

	try {
		std::rethrow_if_nested(e);
	}
	catch(const esl::com::http::server::exception::StatusCode& nestedException) {
		printException(stream, level+1, nestedException, "esl::com::http::server::exception::StatusCode", "status code " + std::to_string(nestedException.getStatusCode()), showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr);
	}
	catch(const esl::database::exception::SqlError& nestedException) {
		std::stringstream s;
		nestedException.getDiagnostics().dump(s);
		printException(stream, level+1, nestedException, "esl::database::exception::SqlError", "error code " + std::to_string(nestedException.getSqlReturnCode()) + "\n" + s.str(), showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr);
	}
	catch(const std::runtime_error& nestedException) {
		printException(stream, level+1, nestedException, "std::runtime_error", "", showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr);
	}
	catch(const std::exception& nestedException) {
		printException(stream, level+1, nestedException, "std::exception", "", showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr);
	}
	catch(...) {
		stream << "[" + std::to_string(level+1) + "] Exception : unknown\n";
	}
}

void Context::printException(esl::logging::StreamReal& stream, int level, const std::exception& e, const std::string& plainException, const std::string& plainDetails, const esl::system::stacktrace::IStacktrace* stacktrace, esl::logging::Location location) {
	std::string levelStr = "[" + std::to_string(level) + "]";

	stream(location.object, location.function, location.file, location.line) << levelStr << " Exception : " << plainException << "\n";
	stream(location.object, location.function, location.file, location.line) << levelStr << " What      : " << (e.what() ? e.what() : "") << "\n";
	if(plainDetails.empty() == false) {
		stream(location.object, location.function, location.file, location.line) << levelStr << " Details   : " << plainDetails << "\n";
	}

	if(showStacktrace) {
		if(stacktrace) {
			stream(location.object, location.function, location.file, location.line) << levelStr << " Stacktrace:\n";
			stacktrace->dump(stream, location);
		}
		else {
			stream(location.object, location.function, location.file, location.line) << levelStr << " Stacktrace: not available\n";
		}
	}

	stream(location.object, location.function, location.file, location.line) << "\n";

	try {
		std::rethrow_if_nested(e);
	}
	catch(const esl::com::http::server::exception::StatusCode& nestedException) {
		printException(stream, level+1, nestedException, "esl::com::http::server::exception::StatusCode", "status code " + std::to_string(nestedException.getStatusCode()), showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr, location);
	}
	catch(const esl::database::exception::SqlError& nestedException) {
		std::stringstream s;
		nestedException.getDiagnostics().dump(s);
		printException(stream, level+1, nestedException, "esl::database::exception::SqlError", "error code " + std::to_string(nestedException.getSqlReturnCode()) + "\n" + s.str(), showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr, location);
	}
	catch(const std::runtime_error& nestedException) {
		printException(stream, level+1, nestedException, "std::runtime_error", "", showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr, location);
	}
	catch(const std::exception& nestedException) {
		printException(stream, level+1, nestedException, "std::exception", "", showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr, location);
	}
	catch(...) {
		stream(location.object, location.function, location.file, location.line) << "[" + std::to_string(level+1) + "] Exception : unknown\n";
	}
}

void Context::printException(std::ostream& stream, std::exception_ptr exceptionPointer) {
	try {
		std::rethrow_exception(exceptionPointer);
    }
	catch(const esl::com::http::server::exception::StatusCode& nestedException) {
		printException(stream, 1, nestedException, "esl::com::http::server::exception::StatusCode", "status code " + std::to_string(nestedException.getStatusCode()), showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr);
	}
	catch(const esl::database::exception::SqlError& nestedException) {
		std::stringstream s;
		nestedException.getDiagnostics().dump(s);
		printException(stream, 1, nestedException, "esl::database::exception::SqlError", "error code " + std::to_string(nestedException.getSqlReturnCode()) + "\n" + s.str(), showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr);
	}
	catch(const std::runtime_error& nestedException) {
		printException(stream, 1, nestedException, "std::runtime_error", "", showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr);
	}
	catch(const std::exception& nestedException) {
		printException(stream, 1, nestedException, "std::exception", "", showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr);
	}
	catch(...) {
		stream << "[1] Exception : unknown\n";
	}
}

void Context::printException(esl::logging::StreamReal& stream, std::exception_ptr exceptionPointer, esl::logging::Location location) {
	try {
		std::rethrow_exception(exceptionPointer);
    }
	catch(const esl::com::http::server::exception::StatusCode& nestedException) {
		printException(stream, 1, nestedException, "esl::com::http::server::exception::StatusCode", "status code " + std::to_string(nestedException.getStatusCode()), showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr, location);
	}
	catch(const esl::database::exception::SqlError& nestedException) {
		std::stringstream s;
		nestedException.getDiagnostics().dump(s);
		printException(stream, 1, nestedException, "esl::database::exception::SqlError", "error code " + std::to_string(nestedException.getSqlReturnCode()) + "\n" + s.str(), showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr, location);
	}
	catch(const std::runtime_error& nestedException) {
		printException(stream, 1, nestedException, "std::runtime_error", "", showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr, location);
	}
	catch(const std::exception& nestedException) {
		printException(stream, 1, nestedException, "std::exception", "", showStacktrace ? esl::system::stacktrace::IStacktrace::get(nestedException) : nullptr, location);
	}
	catch(...) {
		stream(location.object, location.function, location.file, location.line) << "[1] Exception : unknown\n";
	}
}

} /* namespace context */
} /* namespace boot */
} /* namespace jboot */
