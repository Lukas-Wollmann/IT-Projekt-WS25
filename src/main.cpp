#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <sstream>

#include "ast/AST.h"
#include "ast/Printer.h"
#include "codegen/CodeGen.h"
#include "codegen/CodeGenContext.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "semantic/passes/ExplorationPass.h"
#include "semantic/passes/TypeCheckingPass.h"

using namespace ast;
using namespace type;
using namespace semantic;
using namespace codegen;
using namespace lexer;
using namespace parser;

#define MAKE(t, ...) std::make_unique<t>(__VA_ARGS__)
#define MOVE(t)		 std::move(t)

int main(const int argc, const char *argv[]) {
	if (argc < 2) {
		std::cout << "Usage: \"" << argv[0] << "\" <input-filename> [options]\n";
		std::cout << "Options:\n";
		std::cout << "\t-o, --output <filename> Name of the generated output file\n";
		std::cout << "\t-d, --debug             Print debug information for AST and Tokens\n";
		std::cout << "\t-i, --keep-intermediate Keeps the generated intermediate files\n";
		std::cout << std::endl;
		return 1;
	}

	std::string filename = argv[1];
	std::string outputFilename = "out";
	bool debug = false, keepIntermediate = false;

	for (int i = 2; i < argc; ++i) {
		std::string opt = argv[i];

		if (opt == "-d" || opt == "--debug") {
			debug = true;
		} else if (opt == "-o" || opt == "--output") {
			if (i > argc - 2) {
				std::cout << "Expected filename after option '" << opt << "'" << std::endl;
				return 1;
			}

			outputFilename = argv[++i];
		} else if (opt == "-i" || opt == "--keep-intermediate") {
			keepIntermediate = true;
		} else {
			std::cout << "Unknown option: " << opt << std::endl;
			return 1;
		}
	}

	std::ifstream file(filename, std::ios::in | std::ios::binary);

	if (!file) {
		std::cout << "Failed to open file" << std::endl;
		return 3;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	U8String source(buffer.str());
	file.close();

	auto tokens = Lexer::tokenize(source);

	if (debug) {
		for (auto tok : tokens)
			std::cout << tok << "\n";
	}

	for (auto tok : tokens) {
		if (tok.error != TokenError::None) {
			std::cout << "Illegal token: " << tok << std::endl;
			return 1;
		}
	}

	auto [module, errs] = Parser::parse(tokens, u8"test-module");

	for (auto err : errs)
		std::cout << err << "\n";

	std::cout.flush();

	if (!errs.empty())
		return 2;

	if (debug)
		std::cout << *module << std::endl;

	TypeCheckerContext ctx;

	ExplorationPass pass1(ctx);
	pass1.dispatch(*module);

	TypeCheckingPass pass2(ctx);
	pass2.dispatch(*module);

	for (auto err : ctx.getErrors())
		std::cout << err << "\n";

	std::cout.flush();

	if (!ctx.getErrors().empty())
		return 3;

	std::string llFilename = outputFilename + ".ll";
	std::ofstream output(llFilename);
	CodeGen::generate(output, *module);
	output.close();

	pid_t pid = fork();

	if (pid == 0) {
		std::vector<const char *> clangArgs = {"clang", llFilename.c_str(), "-o",
											   outputFilename.c_str(), nullptr};
		execvp("clang", const_cast<char *const *>(clangArgs.data()));
		perror("execvp failed");
		return 4;
	} else if (pid > 0) {
		int status;
		waitpid(pid, &status, 0);

		if (!keepIntermediate) {
			std::vector<const char *> rmArgs = {"rm", llFilename.c_str(), nullptr};
			execvp("rm", const_cast<char *const *>(rmArgs.data()));
			perror("execvp failed");
		}

		std::cout << "Clang exited with " << status << "\n";
	} else {
		perror("fork failed");
		return 5;
	}

	std::cout << "Build finished sucessfully." << std::endl;

	return 0;
}
