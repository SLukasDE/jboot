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

#ifndef JBOOT_BOOT_CONTEXT_CONTEXT_H_
#define JBOOT_BOOT_CONTEXT_CONTEXT_H_

#include <jboot/boot/context/Entry.h>

#include <esl/boot/context/IContext.h>
#include <esl/logging/StreamReal.h>
#include <esl/logging/StreamEmpty.h>
#include <esl/logging/Location.h>
#include <esl/object/IInitializeContext.h>
#include <esl/object/IObject.h>
#include <esl/object/IContext.h>
#include <esl/processing/procedure/IProcedure.h>
#include <esl/system/stacktrace/IStacktrace.h>

#include <boost/filesystem/path.hpp>

#include <exception>
#include <map>
#include <memory>
#include <ostream>
#include <set>
#include <string>
#include <utility>
#include <vector>

namespace jboot {
namespace boot {
namespace context {

class Context : public esl::boot::context::IContext, public esl::object::IInitializeContext  {
public:
	static std::unique_ptr<esl::boot::context::IContext> create(const std::vector<std::pair<std::string, std::string>>& settings);

	Context(const std::vector<std::pair<std::string, std::string>>& settings);

	void setParent(Context* parentContext);

	esl::boot::context::IContext& addData(const std::string& configuration) override;
	esl::boot::context::IContext& addFile(const boost::filesystem::path& filename) override;
	esl::boot::context::IContext& addReference(const std::string& destinationId, const std::string& sourceId) override;
	int getReturnCode() const override;

	void onEvent(const esl::object::IObject& object) override;
	std::set<std::string> getObjectIds() const override;
	void procedureRun(esl::object::IContext& context) override;

	void initializeContext(esl::object::IContext&) override;

protected:
	esl::object::IObject* findRawObject(const std::string& id) override;
	const esl::object::IObject* findRawObject(const std::string& id) const override;
	void addRawObject(const std::string& id, std::unique_ptr<esl::object::IObject> object) override;

private:
	Context* parent = nullptr;

	enum HandleException {
		rethrow,
		stop,         // (add or override exception object)
		stopAndShow,  // (add or override exception object)
		ignore,       // (continue)
		ignoreAndShow // (continue)
	};
	bool hasHandleException = false;
	HandleException handleException = stop;

	bool hasShowStacktrace = false;
	bool showStacktrace = true;

	struct ShowOutput {
		ShowOutput(std::ostream& aOStream)
		: ostream(&aOStream)
		{}

		ShowOutput(esl::logging::StreamReal& aStreamReal)
		: streamReal(&aStreamReal)
		{}

		ShowOutput(esl::logging::StreamEmpty& aStreamEmpty)
		: streamEmpty(&aStreamEmpty)
		{}

		std::ostream* ostream = nullptr;
		esl::logging::StreamReal* streamReal = nullptr;
		esl::logging::StreamEmpty* streamEmpty = nullptr;
	};
	std::unique_ptr<ShowOutput> showOutput;

	std::vector<std::unique_ptr<Entry>> entries;
	struct IdElement {
		IdElement(std::unique_ptr<esl::object::IObject> aObject)
		: object(std::move(aObject)),
		  refObject(*object),
		  initializeContext(dynamic_cast<esl::object::IInitializeContext*>(&refObject))
		{ }

		IdElement(esl::object::IObject& refObject)
		: refObject(refObject),
		  initializeContext(nullptr)
		{ }

		std::unique_ptr<esl::object::IObject> object;
		esl::object::IObject& refObject;
		esl::object::IInitializeContext* initializeContext;
	};
	std::map<std::string, IdElement> objects;

	int returnCode = 0;

	void printException(std::ostream& stream, int level, const std::exception& e, const std::string& plainException, const std::string& plainDetails, const esl::system::stacktrace::IStacktrace* stacktrace);
	void printException(esl::logging::StreamReal& stream, int level, const std::exception& e, const std::string& plainException, const std::string& plainDetails, const esl::system::stacktrace::IStacktrace* stacktrace, esl::logging::Location location);
//	static inline void printException(esl::logging::StreamEmpty& stream, int level, const std::exception& e, const std::string& plainException, const std::string& plainDetails, esl::logging::Location location) { }

	void printException(std::ostream& stream, std::exception_ptr exceptionPointer);
	void printException(esl::logging::StreamReal& stream, std::exception_ptr exceptionPointer, esl::logging::Location location = esl::logging::Location{});
//	static inline void printException(esl::logging::StreamEmpty& stream, std::exception_ptr exceptionPointer, esl::logging::Location location) { }
};

} /* namespace context */
} /* namespace boot */
} /* namespace jboot */

#endif /* JBOOT_BOOT_CONTEXT_CONTEXT_H_ */
