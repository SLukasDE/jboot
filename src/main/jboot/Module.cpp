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

#include <jboot/Module.h>
#include <jboot/boot/context/Context.h>
#include <jboot/boot/logging/Config.h>
#include <jboot/processing/task/TaskFactory.h>

#include <eslx/Module.h>

#include <esl/boot/context/Interface.h>
#include <esl/boot/logging/Interface.h>
#include <esl/processing/task/Interface.h>
#include <esl/Module.h>
#include <esl/logging/appender/Interface.h>
#include <esl/logging/layout/Interface.h>


namespace jboot {

void Module::install(esl::module::Module& module) {
	esl::setModule(module);

	eslx::Module::install(module);

	module.addInterface(esl::boot::context::Interface::createInterface(
			"jboot@/esl/boot/context",
//			"jboot@/esl/boot/context/Context",
//			"jboot/context",
			&boot::context::Context::create));

	module.addInterface(esl::boot::logging::Interface::createInterface(
			"jboot@/esl/boot/logging",
//			"jboot@/esl/boot/logging/Config",
//			"jboot/logging",
			&boot::logging::Config::loadData, &boot::logging::Config::loadFile));

	module.addInterface(esl::processing::task::Interface::createInterface(
			"jboot@/esl/processing/task",
//			"jboot@/esl/processing/task/TaskFactory",
//			"jboot/task",
			&processing::task::TaskFactory::create));

#if 1
	/* ************************* *
	 * builtin logging appenders *
	 * ************************* */
	module.addInterface(esl::logging::appender::Interface::createInterface(
			"jboot:MemBuffer@/esl/logging/appender",
//			"jboot/membuffer",
			esl::getModule().getInterface<esl::logging::appender::Interface>("eslx/membuffer").createAppender));

	module.addInterface(esl::logging::appender::Interface::createInterface(
			"jboot:OStream@/esl/logging/appender",
//			"jboot/ostream",
			esl::getModule().getInterface<esl::logging::appender::Interface>("eslx/ostream").createAppender));

	module.addInterface(esl::logging::appender::Interface::createInterface(
			"jboot:Humio@/esl/logging/appender",
//			"jboot/ostream",
			esl::getModule().getInterface<esl::logging::appender::Interface>("eslx:Humio@/esl/logging/appender").createAppender));

	/* *********************** *
	 * builtin logging layouts *
	 * *********************** */
	module.addInterface(esl::logging::layout::Interface::createInterface(
			"jboot:Default@/esl/logging/layout",
//			"jboot/default",
			esl::getModule().getInterface<esl::logging::layout::Interface>("eslx/default").createLayout));
#endif
}

} /* namespace jboot */
