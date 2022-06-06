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
#include <jboot/boot/context/Entry.h>

namespace jboot {
namespace boot {
namespace context {

Entry::Entry(std::unique_ptr<esl::object::Interface::Object> aObject)
: object(std::move(aObject)),
  refObject(*object),
  initializeContextPtr(dynamic_cast<esl::object::InitializeContext*>(object.get())),
  //context(dynamic_cast<Context*>(object.get())),
  event(dynamic_cast<esl::object::Event*>(object.get())),
  procedure(dynamic_cast<esl::processing::procedure::Interface::Procedure*>(&refObject))
{ }

Entry::Entry(esl::object::Interface::Object& refObject)
: refObject(refObject),
  initializeContextPtr(nullptr),
  event(dynamic_cast<esl::object::Event*>(&refObject)),
  procedure(dynamic_cast<esl::processing::procedure::Interface::Procedure*>(&refObject))
{ }


void Entry::initializeContext(esl::object::ObjectContext& objectContext) {
	if(initializeContextPtr) {
		initializeContextPtr->initializeContext(objectContext);
		initializeContextPtr = nullptr;
	}

	/* wurde schon oben aufgerufen, weil der JBoot-Context das Interface "InitializeContext" realisiert
	if(context) {
		context->initializeContext(objectContext);
	}
	*/
}

void Entry::onEvent(const esl::object::Interface::Object& object) {
	if(event) {
		event->onEvent(object);
	}
}

void Entry::procedureRun(esl::object::ObjectContext& objectContext) {
	if(procedure) {
		procedure->procedureRun(objectContext);
	}

	/* wurde schon oben aufgerufen, weil jeder ESL-Boot-Context das Interface "procedure" realisiert
	if(context) {
		context->procedureRun(objectContext);
	}
	*/
}

void Entry::procedureCancel() {
	if(procedure) {
		procedure->procedureCancel();
	}
}

} /* namespace context */
} /* namespace boot */
} /* namespace jboot */
