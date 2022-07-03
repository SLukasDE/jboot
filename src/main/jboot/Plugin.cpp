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

#include <esl/boot/context/Context.h>
#include <esl/boot/logging/Config.h>
#include <esl/com/basic/client/ConnectionFactory.h>
#include <esl/com/basic/server/Socket.h>
#include <esl/com/http/client/ConnectionFactory.h>
#include <esl/com/http/server/Socket.h>
#include <esl/database/ConnectionFactory.h>
#include <esl/logging/Logging.h>
#include <esl/logging/Appender.h>
#include <esl/logging/Layout.h>
#include <esl/plugin/Registry.h>
#include <esl/processing/TaskFactory.h>
#include <esl/system/Process.h>
#include <esl/system/Signal.h>
#include <esl/system/Stacktrace.h>

#include <iostream>
namespace jboot {

void Plugin::install(esl::plugin::Registry& registry, const char* data) {
	esl::plugin::Registry::set(registry);

	eslx::Plugin::install(registry, data);

	/* ************ *
	 * esl::logging *
	 * ************ */
	registry.copyPlugin<esl::logging::Logging>("logbook4esl/logging/Logging", "jboot/logging/Logging");
	registry.copyPlugin<esl::logging::Layout>("eslx/logging/DefaultLayout", "jboot/logging/DefaultLayout");
	registry.copyPlugin<esl::logging::Appender>("eslx/logging/MemBufferAppender", "jboot/logging/MemBufferAppender");
	registry.copyPlugin<esl::logging::Appender>("eslx/logging/OStreamAppender", "jboot/logging/OStreamAppender");
#if 0
	module.addInterface(esl::logging::appender::Interface::createInterface(
			"jboot/logging/appender/Humio",
			esl::getModule().getInterface<esl::logging::appender::Interface>("humio4esl/logging/appender/Appender").createAppender));
#endif

	/* ********* *
	 * esl::boot *
	 * ********* */
	registry.addPlugin<esl::boot::context::Context>("jboot/boot/context/Context", &boot::context::Context::create);
	registry.addPlugin<esl::boot::logging::Config>("jboot/boot/logging/Config", &boot::logging::Config::create);

	/* *************** *
	 * esl::com::basic *
	 * *************** */
	registry.copyPlugin<esl::object::Object>("rdkafka4esl/com/basic/broker/Client", "jboot/object/KafkaClient");
	registry.copyPlugin<esl::com::basic::client::ConnectionFactory>("rdkafka4esl/com/basic/client/ConnectionFactory", "jboot/com/basic/client/KafkaConnectionFactory");
	registry.copyPlugin<esl::com::basic::server::Socket>("rdkafka4esl/com/basic/server/Socket", "jboot/com/basic/server/KafkaSocket");

	/* ************** *
	 * esl::com::http *
	 * ************** */
	registry.copyPlugin<esl::com::http::client::ConnectionFactory>("curl4esl/com/http/client/ConnectionFactory", "jboot/com/http/client/ConnectionFactory");
	registry.copyPlugin<esl::com::http::server::Socket>("mhd4esl/com/http/server/Socket", "jboot/com/http/server/Socket");

	/* ************* *
	 * esl::database *
	 * ************* */
	registry.copyPlugin<esl::database::ConnectionFactory>("sqlite4esl/database/ConnectionFactory", "jboot/database/SqliteConnectionFactory");
	registry.copyPlugin<esl::database::ConnectionFactory>("unixODBC4esl/database/ConnectionFactory", "jboot/database/UnixODBCConnectionFactory");

	/* *************** *
	 * esl::processing *
	 * *************** */
	registry.addPlugin<esl::processing::TaskFactory>("jboot/processing/TaskFactory", &processing::task::TaskFactory::create);

	/* *********** *
	 * esl::system *
	 * *********** */
	registry.copyPlugin<esl::system::Stacktrace>("zsystem4esl/system/stacktrace/Stacktrace", "jboot/system/Stacktrace");
	registry.copyPlugin<esl::system::Process>("zsystem4esl/system/process/Process", "jboot/system/Process");
	registry.copyPlugin<esl::system::Signal>("zsystem4esl/system/signal/Signal", "jboot/system/Signal");

}

} /* namespace jboot */
