#include <cstdlib>
#include <filesystem>
#include <string>

#include "core/PrintUtil.h"

bool checkExecutableExists(const std::string &exeName) {
	std::string cmd = "command -v " + exeName + " > /dev/null 2>&1";
	return std::system(cmd.c_str()) == 0;
}

bool checkFileExists(const std::string &path) {
	return std::filesystem::exists(path);
}

bool checkRuntimeDependencies() {
	bool valid = true;

	if (!checkExecutableExists("clang") && !checkExecutableExists("clang-18")) {
		util::print("Error: 'clang' binary (LLVM/Clang) is not installed or not in PATH.\n");
		valid = false;
	}

	if (!checkFileExists("ocn_stdlib.c")) {
		util::print("Error: Required runtime component 'ocn_stdlib.c' was not found.\n");
		valid = false;
	}

	return valid;
}