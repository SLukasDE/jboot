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

#ifndef JBOOT_APPLICATION_H_
#define JBOOT_APPLICATION_H_

#include <jboot/ObjectContext.h>
#include <jboot/ProcedureContext.h>

#include <esl/com/basic/client/ConnectionFactory.h>
#include <esl/com/http/client/ConnectionFactory.h>
#include <esl/database/ConnectionFactory.h>
#include <esl/object/Interface.h>
#include <esl/object/ObjectContext.h>
#include <esl/processing/procedure/Interface.h>

#include <boost/filesystem/path.hpp>

#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace jboot {

class Application final {
public:
	static void loadLoggerConfiguration(const std::string& configuration);
	static void loadLoggerConfiguration(const boost::filesystem::path& filename);

	Application(const std::vector<std::pair<std::string, std::string>>& settings);
	Application(const Application& application) = delete;
	Application(Application&& application);

	~Application();

	Application& operator=(const Application& application) = delete;
	Application& operator=(Application&& application);

    explicit operator bool() const;

    Application& addConfiguration(const std::string& configuration);
	Application& addConfiguration(const boost::filesystem::path& filename);

	Application& addProcedure(std::unique_ptr<esl::processing::procedure::Interface::Procedure> procedure);
	Application& addProcedureContext(ProcedureContext procedureContext);

	Application& addObject(const std::string& id, std::unique_ptr<esl::object::Interface::Object> object);

	/* Helper methods */
	Application& addDatabase(const std::string& id, std::unique_ptr<esl::database::Interface::ConnectionFactory> connectionFactory);
	Application& addBasicClient(const std::string& id, std::unique_ptr<esl::com::basic::client::Interface::ConnectionFactory> connectionFactory);
	Application& addHttpClient(const std::string& id, std::unique_ptr<esl::com::http::client::Interface::ConnectionFactory> connectionFactory);

	Application& run(esl::object::ObjectContext& objectContext);
	Application& run(esl::object::ObjectContext& objectContext, int argc, const char *argv[]);
	Application& run();
	Application& run(int argc, const char *argv[]);

	/* Helper methods */
	int main(esl::object::ObjectContext& objectContext);
	int main(esl::object::ObjectContext& objectContext, int argc, const char *argv[]);
	int main();
	int main(int argc, const char *argv[]);

	int getReturnCode() const;

private:
	void* object;
	int returnCode = 0;
};

} /* namespace jboot */

#endif /* JBOOT_APPLICATION_H_ */
