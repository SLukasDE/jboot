/*
 * This file is part of Jerry application server.
 * Copyright (C) 2020-2022 Sven Lukas
 *
 * Jerry is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as
 * published by the Free Software Foundation, either version 3 of
 * the License, or (at your option) any later version.
 *
 * Jerry is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public
 * License along with Jerry.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <jboot/ProcedureContext.h>

#include <jerry/engine/procedure/Context.h>

#include <stdexcept>
//#include <utility>
//#include <vector>

namespace jboot {

namespace {
jerry::engine::procedure::Context& getProcedureContext(void* object) {
	if(!object) {
		throw std::runtime_error("Null pointer exception");
	}

	return *static_cast<jerry::engine::procedure::Context*>(object);
}
} /* anonymous namespace */


ProcedureContext::ProcedureContext()
: object(new jerry::engine::procedure::Context(nullptr))
{ }

ProcedureContext::ProcedureContext(ProcedureContext&& procedureContext)
: object(procedureContext.object)
{
	procedureContext.object = nullptr;
}

ProcedureContext::~ProcedureContext() {
	if(object) {
		delete static_cast<jerry::engine::procedure::Context*>(object);
		object = nullptr;
	}
}

ProcedureContext& ProcedureContext::operator=(ProcedureContext&& procedureContext) {
	if(object) {
		delete static_cast<jerry::engine::procedure::Context*>(object);
		object = nullptr;
	}

	object = procedureContext.object;
	procedureContext.object = nullptr;

	return *this;
}

ProcedureContext::operator bool() const {
	return object != nullptr;
}

ProcedureContext& ProcedureContext::addProcedure(std::unique_ptr<esl::processing::procedure::Interface::Procedure> procedure) {
	getProcedureContext(object).addProcedure(std::move(procedure));

	return *this;
}

ProcedureContext& ProcedureContext::addProcedureContext(ProcedureContext procedureContext) {
	if(procedureContext) {
		std::unique_ptr<jerry::engine::procedure::Context> jerryProcedureContext(&getProcedureContext(procedureContext.object));
		procedureContext.object = nullptr;

		jerryProcedureContext->setParent(&getProcedureContext(object));
		jerryProcedureContext->setProcessRegistry(getProcedureContext(object).getProcessRegistry());
		getProcedureContext(object).addContext(std::move(jerryProcedureContext));
	}

	return *this;
}

ProcedureContext& ProcedureContext::addObject(const std::string& id, std::unique_ptr<esl::object::Interface::Object> aObject) {
	getProcedureContext(object).addObject(id, std::move(aObject));

	return *this;
}

} /* namespace jboot */
