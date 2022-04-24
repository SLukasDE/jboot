/*
MIT License
Copyright (c) 2022 Sven Lukas

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#if 0
#ifndef ESL_BOOT_APPLICATION_PROCEDURECONTEXT_H_
#define ESL_BOOT_APPLICATION_PROCEDURECONTEXT_H_

#include <esl/boot/implementation/Application.h>

#include <esl/object/Interface.h>
#include <esl/processing/procedure/Interface.h>

#include <memory>
#include <string>

namespace esl {
namespace boot {
namespace application {

class ProcedureContext final {
friend class implementation::Application;
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

} /* namespace application */
} /* namespace boot */
} /* namespace esl */

#endif /* ESL_BOOT_APPLICATION_PROCEDURECONTEXT_H_ */
#endif
