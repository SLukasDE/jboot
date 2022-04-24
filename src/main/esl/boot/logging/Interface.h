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

#ifndef ESL_BOOT_LOGGING_INTERFACE_H_
#define ESL_BOOT_LOGGING_INTERFACE_H_

#include <esl/Module.h>
#include <esl/module/Interface.h>

#include <boost/filesystem/path.hpp>

#include <memory>
#include <string>

namespace esl {
namespace boot {
namespace logging {

struct Interface : esl::module::Interface {
	/* ******************************************** *
	 * type definitions required for this interface *
	 * ******************************************** */

	using LoadData = void (*)(const std::string& configuration);
	using LoadFile = void (*)(const boost::filesystem::path& filename);

	/* ************************************ *
	 * standard API definition of interface *
	 * ************************************ */

	static inline const char* getType() {
		return "esl-boot-logging";
	}

	/* ************************************ *
	 * extended API definition of interface *
	 * ************************************ */

	static std::unique_ptr<const esl::module::Interface> createInterface(const char* implementation, LoadData loadData, LoadFile loadFile) {
		return std::unique_ptr<const esl::module::Interface>(new Interface(implementation, loadData, loadFile));
	}

	Interface(const char* implementation, LoadData aLoadData, LoadFile aLoadFile)
	: esl::module::Interface(esl::getModule().getId(), getType(), implementation, esl::getModule().getApiVersion()),
	  loadData(aLoadData),
	  loadFile(aLoadFile)
	{ }

	LoadData loadData;
	LoadFile loadFile;
};

} /* namespace logging */
} /* namespace boot */
} /* namespace esl */

#endif /* ESL_BOOT_LOGGING_INTERFACE_H_ */
