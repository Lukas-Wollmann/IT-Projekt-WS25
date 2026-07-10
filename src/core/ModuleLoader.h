#pragma once

#include <filesystem>
#include <unordered_map>

#include "ast/AST.h"
#include "core/ErrorHandler.h"

namespace core {
struct ModuleLoader {
private:
	ErrorHandler &m_ErrorHandler;
	bool m_Debug;
	mutable std::unordered_map<std::string, Box<ast::Module>> m_ModuleCache;

public:
	explicit ModuleLoader(ErrorHandler &err, bool debug = false);

	Box<ast::Module> load(const std::filesystem::path &entryPath);

	// Collect all reachable module paths starting from entryPath (including entryPath).
	Vec<std::filesystem::path> collectAllModulePaths(const std::filesystem::path &entryPath);
	// Validate that a module is self-contained (all referenced struct types
	// are declared either in the module itself or anywhere in its import graph).
	bool validateStandalone(const std::filesystem::path &modulePath);
	bool validateImportGraph(const std::filesystem::path &entryPath);

private:
	Box<ast::Module> parseModule(const std::filesystem::path &path);
	ast::Module *getParsedModule(const std::filesystem::path &path);
	std::filesystem::path resolveImport(const std::filesystem::path &currentPath,
										const U8String &importPath) const;
	U8String readFile(const std::filesystem::path &path) const;
	static Box<ast::Module> flatten(Box<ast::Module> root, Vec<Box<ast::Module>> imports);
};
}