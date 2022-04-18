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

#ifndef JBOOT_PROCEDURECONTEXT_H_
#define JBOOT_PROCEDURECONTEXT_H_

#include <esl/object/Interface.h>
#include <esl/processing/procedure/Interface.h>

#include <memory>
#include <string>

namespace jboot {

class ProcedureContext final {
friend class Application;
public:
	ProcedureContext();
	ProcedureContext(const ProcedureContext& application) = delete;
	ProcedureContext(ProcedureContext&& application);

	~ProcedureContext();

	ProcedureContext& operator=(const ProcedureContext& application) = delete;
	ProcedureContext& operator=(ProcedureContext&& application);

	explicit operator bool() const;

	ProcedureContext& addProcedure(std::unique_ptr<esl::processing::procedure::Interface::Procedure> procedure);
	ProcedureContext& addProcedureContext(ProcedureContext procedureContext);
	ProcedureContext& addObject(const std::string& id, std::unique_ptr<esl::object::Interface::Object> object);

private:
	void* object;
};

} /* namespace jboot */

#endif /* JBOOT_PROCEDURECONTEXT_H_ */
