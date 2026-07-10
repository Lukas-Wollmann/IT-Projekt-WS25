#pragma once

#include <filesystem>
#include <unordered_map>

#include "ast/AST.h"
#include "core/ErrorHandler.h"

namespace comp {
struct ModuleLoader {
private:
	ErrorHandler &m_ErrorHandler;
	mutable std::unordered_map<std::string, Box<ast::Module>> m_ModuleCache;

public:
	explicit ModuleLoader(ErrorHandler &err);

	Box<ast::Module> load(const std::filesystem::path &entryPath);

	Vec<std::filesystem::path> collectAllModulePaths(const std::filesystem::path &entryPath);
	bool validateStandalone(const std::filesystem::path &modulePath);
	bool validateImportGraph(const std::filesystem::path &entryPath);

private:
	Box<ast::Module> parseModule(const std::filesystem::path &path);
	const ast::Module *getParsedModule(const std::filesystem::path &path) const;
	std::filesystem::path resolveImport(const std::filesystem::path &currentPath,
										const U8String &importPath) const;
	U8String readFile(const std::filesystem::path &path) const;
	static Box<ast::Module> flatten(Box<ast::Module> root, Vec<Box<ast::Module>> imports);
};
}