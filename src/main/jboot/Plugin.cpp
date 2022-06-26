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

#include <jboot/Plugin.h>
#include <jboot/boot/context/Context.h>
#include <jboot/boot/logging/Config.h>
#include <jboot/processing/task/TaskFactory.h>

#include <eslx/Plugin.h>

#include <esl/boot/context/IContext.h>
#include <esl/boot/logging/IConfig.h>
#include <esl/com/basic/client/IConnectionFactory.h>
#include <esl/com/basic/server/ISocket.h>
#include <esl/com/http/client/IConnectionFactory.h>
#include <esl/com/http/server/ISocket.h>
#include <esl/database/IConnectionFactory.h>
#include <esl/logging/ILogger.h>
#include <esl/logging/IAppender.h>
#include <esl/logging/ILayout.h>
#include <esl/plugin/Registry.h>
#include <esl/processing/task/ITaskFactory.h>
#include <esl/system/stacktrace/IStacktrace.h>
#include <esl/system/process/IProcess.h>
#include <esl/system/signal/ISignal.h>

namespace jboot {

void Plugin::install(esl::plugin::Registry& registry, const char* data) {
	esl::plugin::Registry::set(registry);

	eslx::Plugin::install(registry, data);

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::boot::context::IContext::Plugin(
			"jboot/boot/context/Context",
//			"jboot::boot::context::Context",
//			"jboot@/esl/boot/context",
//			"jboot@/esl/boot/context/Context",
//			"jboot/context",
			&boot::context::Context::create)));

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::boot::logging::IConfig::Plugin(
			"jboot/boot/logging/Config",
			&boot::logging::Config::create)));

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::processing::task::ITaskFactory::Plugin(
			"jboot/processing/task/TaskFactory",
			&processing::task::TaskFactory::create)));

	/* ************************* *
	 * builtin logging appenders *
	 * ************************* */

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::logging::ILogger::Plugin(
			"jboot/logging/Logger",
			esl::plugin::Registry::get().getPlugin<esl::logging::ILogger::Plugin>("logbook4esl/logging/Logger").create)));

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::logging::IAppender::Plugin(
			"jboot/logging/appender/MemBuffer",
			esl::plugin::Registry::get().getPlugin<esl::logging::IAppender::Plugin>("eslx/logging/appender/MemBuffer").create)));

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::logging::IAppender::Plugin(
			"jboot/logging/appender/OStream",
			esl::plugin::Registry::get().getPlugin<esl::logging::IAppender::Plugin>("eslx/logging/appender/OStream").create)));

#if 0
	module.addInterface(esl::logging::appender::Interface::createInterface(
			"jboot/logging/appender/Humio",
			esl::getModule().getInterface<esl::logging::appender::Interface>("humio4esl/logging/appender/Appender").createAppender));
#endif
	/* *********************** *
	 * builtin logging layouts *
	 * *********************** */

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::logging::ILayout::Plugin(
			"jboot/logging/layout/Default",
			esl::plugin::Registry::get().getPlugin<esl::logging::ILayout::Plugin>("eslx/logging/layout/Default").create)));
/*
	module.addInterface(esl::object::Interface::createInterface(
			"jboot/object/KafkaBroker",
			esl::getModule().getInterface<esl::object::Interface>("rdkafka4esl").createObject));

	module.addInterface(esl::com::basic::client::Interface::createInterface(
			"jboot/com/basic/client/KafkaConnectionFactory",
			esl::getModule().getInterface<esl::com::basic::client::Interface>("rdkafka4esl").createConnectionFactory));

	module.addInterface(esl::com::basic::server::Interface::createInterface(
			"jboot/com/basic/server/KafkaSocket",
			esl::getModule().getInterface<esl::com::basic::server::Interface>("rdkafka4esl").createSocket));
*/

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::com::http::client::IConnectionFactory::Plugin(
			"jboot/com/http/client/ConnectionFactory",
			esl::plugin::Registry::get().getPlugin<esl::com::http::client::IConnectionFactory::Plugin>("curl4esl/com/http/client/ConnectionFactory").create)));

/*
	module.addInterface(esl::com::http::server::Interface::createInterface(
			"jboot/com/http/server/Socket",
			esl::getModule().getInterface<esl::com::http::server::Interface>("mhd4esl").createSocket));
*/

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::system::stacktrace::IStacktrace::Plugin(
			"jboot/system/stacktrace/Stacktrace",
			esl::plugin::Registry::get().getPlugin<esl::system::stacktrace::IStacktrace::Plugin>("zsystem4esl/system/stacktrace/Stacktrace").create)));

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::system::process::IProcess::Plugin(
			"jboot/system/process/Process",
			esl::plugin::Registry::get().getPlugin<esl::system::process::IProcess::Plugin>("zsystem4esl/system/process/Process").create)));

	registry.addPlugin(std::unique_ptr<const esl::plugin::BasePlugin>(new esl::system::signal::ISignal::Plugin(
			"jboot/system/signal/Signal",
			esl::plugin::Registry::get().getPlugin<esl::system::signal::ISignal::Plugin>("zsystem4esl/system/signal/Signal").create)));

/*
	module.addInterface(esl::database::Interface::createInterface(
			"jboot/database/SQLiteConnectionFactory",
			esl::getModule().getInterface<esl::database::Interface>("sqlite4esl").createConnectionFactory));

	module.addInterface(esl::database::Interface::createInterface(
			"jboot/database/UnixODBCConnectionFactory",
			esl::getModule().getInterface<esl::database::Interface>("unixODBC4esl").createConnectionFactory));
*/
}

} /* namespace jboot */
