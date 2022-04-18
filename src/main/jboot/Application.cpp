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
 * License along with Jerry.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <jboot/Application.h>

#include <jerry/config/logging/Logger.h>
#include <jerry/config/main/Context.h>
#include <jerry/engine/main/Context.h>

#include <esl/object/Value.h>

#include <stdexcept>

namespace jboot {

namespace {
jerry::engine::main::Context& getMainContext(void* object) {
	if(!object) {
		throw std::runtime_error("Null pointer exception");
	}

	return *static_cast<jerry::engine::main::Context*>(object);
}

jerry::engine::procedure::Context& getProcedureContext(void* object) {
	if(!object) {
		throw std::runtime_error("Null pointer exception");
	}

	return *static_cast<jerry::engine::procedure::Context*>(object);
}
} /* anonymous namespace */

using ReturnCodeObject = esl::object::Value<int>;
using ArgumentsByVector = esl::object::Value<std::vector<std::string>>;

void Application::loadLoggerConfiguration(const std::string& configuration) {
	jerry::config::logging::Logger loggerConfig(configuration);
	loggerConfig.install();
}

void Application::loadLoggerConfiguration(const boost::filesystem::path& filename) {
	jerry::config::logging::Logger loggerConfig(filename);
	loggerConfig.install();
}

Application::Application(const std::vector<std::pair<std::string, std::string>>& settings)
: object(new jerry::engine::main::Context(settings))
{ }

Application::Application(Application&& application)
: object(application.object),
  returnCode(application.returnCode)
{
	application.object = nullptr;
}

Application::~Application() {
	if(object) {
		delete static_cast<jerry::engine::main::Context*>(object);
		object = nullptr;
	}
}

Application& Application::operator=(Application&& application) {
	if(object) {
		delete static_cast<jerry::engine::main::Context*>(object);
		object = nullptr;
	}

	object = application.object;
	application.object = nullptr;

	returnCode = application.returnCode;

	return *this;
}

Application::operator bool() const {
	return object != nullptr;
}

Application& Application::addConfiguration(const std::string& configuration) {
	jerry::config::main::Context config(configuration);

	config.loadLibraries();
	config.install(getMainContext(object));

	return *this;
}

Application& Application::addConfiguration(const boost::filesystem::path& filename) {
	jerry::config::main::Context config(filename);

	config.loadLibraries();
	config.install(getMainContext(object));

	return *this;
}

Application& Application::addProcedure(std::unique_ptr<esl::processing::procedure::Interface::Procedure> procedure) {
	getMainContext(object).addProcedure(std::move(procedure));

	return *this;
}

Application& Application::addProcedureContext(ProcedureContext procedureContext) {
	if(procedureContext) {
		std::unique_ptr<jerry::engine::procedure::Context> jerryProcedureContext(&getProcedureContext(procedureContext.object));
		procedureContext.object = nullptr;

		jerryProcedureContext->setParent(&getMainContext(object));
		jerryProcedureContext->setProcessRegistry(getMainContext(object).getProcessRegistry());
		getMainContext(object).addProcedureContext(std::move(jerryProcedureContext));
	}

	return *this;
}

Application& Application::addObject(const std::string& id, std::unique_ptr<esl::object::Interface::Object> aObject) {
	getMainContext(object).addObject(id, std::move(aObject));

	return *this;
}

Application& Application::addDatabase(const std::string& id, std::unique_ptr<esl::database::Interface::ConnectionFactory> connectionFactory) {
	return addObject(id, std::unique_ptr<esl::object::Interface::Object>(connectionFactory.release()));
}

Application& Application::addBasicClient(const std::string& id, std::unique_ptr<esl::com::basic::client::Interface::ConnectionFactory> connectionFactory) {
	return addObject(id, std::unique_ptr<esl::object::Interface::Object>(connectionFactory.release()));
}

Application& Application::addHttpClient(const std::string& id, std::unique_ptr<esl::com::http::client::Interface::ConnectionFactory> connectionFactory) {
	return addObject(id, std::unique_ptr<esl::object::Interface::Object>(connectionFactory.release()));
}

Application& Application::run(esl::object::ObjectContext& objectContext) {
	getMainContext(object).procedureRun(objectContext);

	ReturnCodeObject* returnCodeObject = objectContext.findObject<ReturnCodeObject>("return-code");
	if(returnCodeObject) {
		returnCode = **returnCodeObject;
	}

	return *this;
}

Application& Application::run(esl::object::ObjectContext& objectContext, int argc, const char *argv[]) {
	std::unique_ptr<ArgumentsByVector> argumentsObject(new ArgumentsByVector({}));
	for(int i=0; i<argc; ++i) {
		argumentsObject->get().push_back(argv[i]);
	}

	esl::object::Interface::Object* objectPtr = objectContext.findObject<esl::object::Interface::Object>("arguments");
	ArgumentsByVector* argumentsObjectPtr = dynamic_cast<ArgumentsByVector*>(objectPtr);

	if(argumentsObjectPtr) {
		*argumentsObjectPtr = std::move(argumentsObject->get());
	}
	else if(objectPtr == nullptr) {
		objectContext.addObject("arguments", std::unique_ptr<esl::object::Interface::Object>(argumentsObject.release()));
	}

	return run(objectContext);
}

Application& Application::run() {
	ObjectContext objectContext;
	return run(objectContext);
}

Application& Application::run(int argc, const char *argv[]) {
	ObjectContext objectContext;
	return run(objectContext, argc, argv);
}

int Application::main(esl::object::ObjectContext& objectContext) {
	return run(objectContext).getReturnCode();
}

int Application::main(esl::object::ObjectContext& objectContext, int argc, const char *argv[]) {
	return run(objectContext, argc, argv).getReturnCode();
}

int Application::main(int argc, const char *argv[]) {
	return run(argc, argv).getReturnCode();
}

int Application::main() {
	return run().getReturnCode();
}

int Application::getReturnCode() const {
	return returnCode;

}

} /* namespace jboot */
