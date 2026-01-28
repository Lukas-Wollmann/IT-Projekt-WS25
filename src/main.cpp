#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <fstream>
#include <sstream>

#include "ast/AST.h"
#include "ast/Printer.h"
#include "codegen/CodeGen.h"
#include "codegen/CodeGenContext.h"
#include "core/ErrorHandler.h"
#include "core/PrintUtil.h"
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

int main(const int argc, const char *argv[]) {
	if (argc < 2) {
		util::print("Usage: \"{}\" <input-filename> [options]\n", argv[0]);
		util::print("Options:\n");
		util::print("\t-o, --output <filename> Name of the generated output file\n");
		util::print("\t-d, --debug             Print debug information for AST and Tokens\n");
		util::print("\t-i, --keep-intermediate Keeps the generated intermediate files\n");
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
				util::print("Expected filename after option '{}'.\n", opt);
				return 1;
			}

			outputFilename = argv[++i];
		} else if (opt == "-i" || opt == "--keep-intermediate") {
			keepIntermediate = true;
		} else {
			util::print("Unknown option: '{}'.", opt);
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

	ErrorHandler err(filename, source);

	auto tokens = Lexer::tokenize(source, err);

	if (debug) {
		for (auto tok : tokens)
			util::print("{:?}\n", tok);
	}

	if (err.hasError()) {
		err.printErrors();
		return 1;
	}

	auto module = Parser::parse(tokens, err, filename);

	if (err.hasError()) {
		err.printErrors();
		return 2;
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
		std::vector<const char *> clangArgs = {"clang", llFilename.c_str(), "-o",
											   outputFilename.c_str(), nullptr};
		execvp("clang", const_cast<char *const *>(clangArgs.data()));
		perror("execvp failed");
	} else if (pid > 0) {
		int status;
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
