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

#include <jboot/application/Application.h>

#include <jerry/config/logging/Logger.h>
#include <jerry/config/main/Context.h>
#include <jerry/engine/main/Context.h>

namespace jboot {
namespace application {

namespace {
jerry::engine::procedure::Context& getProcedureContext(void* object) {
	if(!object) {
		throw std::runtime_error("Null pointer exception");
	}

	return *static_cast<jerry::engine::procedure::Context*>(object);
}
} /* anonymous namespace */

std::unique_ptr<esl::boot::application::Interface::Application> Application::create(const std::vector<std::pair<std::string, std::string>>& settings) {
	return std::unique_ptr<esl::boot::application::Interface::Application>(new Application(settings));
}

Application::Application(const std::vector<std::pair<std::string, std::string>>& settings)
: context(settings)
{ }

esl::boot::application::Interface::Application& Application::addData(const std::string& configuration) {
	jerry::config::main::Context config(configuration, true);

	config.loadLibraries();
	config.install(context);

	return *this;
}

esl::boot::application::Interface::Application& Application::addFile(const boost::filesystem::path& filename) {
	jerry::config::main::Context config(filename, true);

	config.loadLibraries();
	config.install(context);

	return *this;
}

esl::boot::application::Interface::Application& Application::addProcedure(std::unique_ptr<esl::processing::procedure::Interface::Procedure> procedure) {
	context.addProcedure(std::move(procedure));

	return *this;
}
/*
esl::boot::application::Interface::Application& Application::addProcedureContext(application::ProcedureContext procedureContext) {
	if(procedureContext) {
		std::unique_ptr<jerry::engine::procedure::Context> jerryProcedureContext(&getProcedureContext(procedureContext.object));
		procedureContext.object = nullptr;

		jerryProcedureContext->setParent(&context);
		jerryProcedureContext->setProcessRegistry(context.getProcessRegistry());
		context.addProcedureContext(std::move(jerryProcedureContext));
	}

	return *this;
}
*/
esl::boot::application::Interface::Application& Application::addObject(const std::string& id, std::unique_ptr<esl::object::Interface::Object> aObject) {
	context.addObject(id, std::move(aObject));

	return *this;
}

esl::boot::application::Interface::Application& Application::run(esl::object::ObjectContext& objectContext) {
	context.procedureRun(objectContext);

	ReturnCodeObject* returnCodeObject = objectContext.findObject<ReturnCodeObject>("return-code");
	if(returnCodeObject) {
		returnCode = **returnCodeObject;
	}

	return *this;
}

int Application::getReturnCode() const {
	return returnCode;
}

} /* namespace application */
} /* namespace jboot */
