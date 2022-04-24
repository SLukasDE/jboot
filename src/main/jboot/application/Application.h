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

#ifndef JBOOT_APPLICATION_APPLICATION_H_
#define JBOOT_APPLICATION_APPLICATION_H_

#include <esl/boot/application/Interface.h>
#include <esl/boot/application/ProcedureContext.h>

#include <jerry/engine/main/Context.h>

namespace jboot {
namespace application {

class Application : public esl::boot::application::Interface::Application {
public:
	static std::unique_ptr<esl::boot::application::Interface::Application> create(const std::vector<std::pair<std::string, std::string>>& settings);

	Application(const std::vector<std::pair<std::string, std::string>>& settings);

	esl::boot::application::Interface::Application& addData(const std::string& configuration) override;
	esl::boot::application::Interface::Application& addFile(const boost::filesystem::path& filename) override;

	esl::boot::application::Interface::Application& addProcedure(std::unique_ptr<esl::processing::procedure::Interface::Procedure> procedure) override;
	// application::Interface::Application& addProcedureContext(application::ProcedureContext procedureContext) override;

	esl::boot::application::Interface::Application& addObject(const std::string& id, std::unique_ptr<esl::object::Interface::Object> object) override;

	esl::boot::application::Interface::Application& run(esl::object::ObjectContext& objectContext) override;
	int getReturnCode() const override;

private:
	jerry::engine::main::Context context;
	int returnCode = 0;
};

} /* namespace application */
} /* namespace jboot */

#endif /* JBOOT_APPLICATION_APPLICATION_H_ */
