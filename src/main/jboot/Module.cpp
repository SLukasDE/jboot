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

#include <jboot/Module.h>
#include <jboot/application/Application.h>
#include <jboot/logging/Config.h>

#include <esl/boot/application/Interface.h>
#include <esl/boot/logging/Interface.h>

#include <jerry/Module.h>

#include <esl/Module.h>
#include <esl/logging/appender/Interface.h>
#include <esl/logging/layout/Interface.h>


namespace jboot {

void Module::install(esl::module::Module& module) {
	esl::setModule(module);

	jerry::Module::install(module);

	module.addInterface(esl::boot::application::Interface::createInterface(
			"jboot/application",
			&application::Application::create));

	module.addInterface(esl::boot::logging::Interface::createInterface(
			"jboot/logging",
			&logging::Config::loadData, &logging::Config::loadFile));

	/* ************************* *
	 * builtin logging appenders *
	 * ************************* */
	module.addInterface(esl::logging::appender::Interface::createInterface(
			"jboot/membuffer",
			esl::getModule().getInterface<esl::logging::appender::Interface>("eslx/membuffer").createAppender));

	module.addInterface(esl::logging::appender::Interface::createInterface(
			"jboot/ostream",
			esl::getModule().getInterface<esl::logging::appender::Interface>("eslx/ostream").createAppender));

	/* *********************** *
	 * builtin logging layouts *
	 * *********************** */
	module.addInterface(esl::logging::layout::Interface::createInterface(
			"jboot/default",
			esl::getModule().getInterface<esl::logging::layout::Interface>("eslx/default").createLayout));
}

} /* namespace jboot */
