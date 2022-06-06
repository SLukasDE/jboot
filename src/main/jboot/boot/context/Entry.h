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

#ifndef JBOOT_BOOT_CONTEXT_ENTRY_H_
#define JBOOT_BOOT_CONTEXT_ENTRY_H_

#include <esl/object/Event.h>
#include <esl/object/Interface.h>
#include <esl/object/InitializeContext.h>
#include <esl/object/ObjectContext.h>
#include <esl/processing/procedure/Interface.h>

#include <memory>

namespace jboot {
namespace boot {
namespace context {

class Entry {
public:
	Entry(std::unique_ptr<esl::object::Interface::Object> object);
	Entry(esl::object::Interface::Object& refObject);

	void initializeContext(esl::object::ObjectContext& objectContext);
	void onEvent(const esl::object::Interface::Object& object);
	void procedureRun(esl::object::ObjectContext& objectContext);
	void procedureCancel();

private:
	std::unique_ptr<esl::object::Interface::Object> object;
	esl::object::Interface::Object& refObject;

	esl::object::InitializeContext* initializeContextPtr = nullptr;
	//Context* context = nullptr;
	esl::object::Event* event = nullptr;
	esl::processing::procedure::Interface::Procedure* procedure = nullptr;
};

} /* namespace context */
} /* namespace boot */
} /* namespace jboot */

#endif /* JBOOT_BOOT_CONTEXT_ENTRY_H_ */
