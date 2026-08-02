#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <sstream>

#include "ast/AST.h"
#include "ast/Printer.h"
#include "codegen/CodeGen.h"
#include "core/ErrorHandler.h"
#include "core/ModuleLoader.h"
#include "core/PrintUtil.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "semantic/passes/ExplorationPass.h"
#include "semantic/passes/TypeCheckingPass.h"
#include "type/TypeFactory.h"

using namespace lex;
using namespace prs;
using namespace ast;
using namespace sem;
using namespace gen;

#define MAJOR_VERSION "1"

#ifndef BUILD_NUMBER
#define BUILD_NUMBER "0"
#endif
#ifndef BRANCH_NAME
#define BRANCH_NAME "unknown"
#endif

int main(const int argc, const char *argv[]) {
	if (argc < 2) {
		util::print("Ocean compiler v{}.{}-{}\n\n", MAJOR_VERSION, BUILD_NUMBER, BRANCH_NAME);
		util::print("Usage: \"{}\" <input-filename> [options]\n", argv[0]);
		util::print("Options:\n");
		util::print("\t-o, --output <filename> Name of the generated output file\n");
		util::print("\t-d, --debug             Print debug information for AST and Tokens\n");
		util::print("\t-i, --keep-intermediate Keeps the generated intermediate files\n");
		return 1;
	}

	std::string filename = argv[1];
	std::string outputFilename = "out";
	bool debug = false;
	bool keepIntermediate = false;

	for (int i = 2; i < argc; ++i) {
		std::string opt = argv[i];

		if (opt == "-o" || opt == "--output") {
			if (i > argc - 2) {
				util::print("Expected filename after option '{}'.\n", opt);
				return 1;
			}

			outputFilename = argv[++i];
		} else if (opt == "-d" || opt == "--debug") {
			debug = true;
		} else if (opt == "-i" || opt == "--keep-intermediate") {
			keepIntermediate = true;
		} else {
			util::print("Unknown option: '{}'.\n", opt);
			return 1;
		}
	}

	std::ifstream file(filename, std::ios::in | std::ios::binary);

	if (!file) {
		util::print("Could not open file: '{}'.\n", filename);
		return 3;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	U8String source(buffer.str());
	file.close();

	ErrorHandler err(U8String(filename), source);
	err.registerSource(U8String(filename), source);

	core::ModuleLoader loader(err, debug);
	const auto entryPath = std::filesystem::path(filename).lexically_normal();
	auto module = loader.load(entryPath);

	if (!module) {
		err.printErrors();
		return 2;
	}

	if (err.hasError()) {
		err.printErrors();
		return 3;
	}

	// Validate the full import graph once; each module is checked a single time.
	if (!loader.validateImportGraph(entryPath)) {
		err.printErrors();
		return 2;
	}

	if (err.hasError()) {
		err.printErrors();
		return 3;
	}

	if (debug)
		util::print("{}\n", *module);

	TypeCheckerContext ctx(err);

	ExplorationPass pass1(ctx);
	pass1.dispatch(*module);

	TypeCheckingPass pass2(ctx);
	pass2.dispatch(*module);

	err.printErrors();

	if (err.hasError())
		return 3;

	std::string llFilename = outputFilename + ".ll";
	std::ofstream output(llFilename);

	CodeGen::generate(output, *module);

	output.close();

	pid_t pid = fork();

	if (pid == 0) {
		std::vector<const char *> clangArgs = {"clang", llFilename.c_str(),
											   "-o",	outputFilename.c_str(),
											   "-O3",	"ocn_stdlib.c",
											   nullptr};
		execvp("clang", const_cast<char *const *>(clangArgs.data()));
		perror("execvp failed");
	} else if (pid > 0) {
		int status = 0;
		waitpid(pid, &status, 0);

		if (!keepIntermediate) {
			std::vector<const char *> rmArgs = {"rm", llFilename.c_str(), nullptr};
			execvp("rm", const_cast<char *const *>(rmArgs.data()));
			perror("execvp failed");
		}
	} else {
		perror("fork failed");
		return 5;
	}

	return 0;
}
