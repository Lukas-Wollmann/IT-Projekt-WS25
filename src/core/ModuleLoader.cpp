#include "ModuleLoader.h"

#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include <unordered_set>

#include "core/PrintUtil.h"
#include "lexer/Lexer.h"
#include "parser/Parser.h"
#include "type/Type.h"

namespace core {
using namespace ast;
using namespace lex;
using namespace prs;

ModuleLoader::ModuleLoader(ErrorHandler &err, bool debug)
	: m_ErrorHandler(err)
	, m_Debug(debug) {}

static void debugLog(bool debug, const std::string &message) {
	if (debug) {
		util::print("[module-loader] {}\n", message);
	}
}

U8String ModuleLoader::readFile(const std::filesystem::path &path) const {
	std::ifstream file(path, std::ios::in | std::ios::binary);
	if (!file) {
		return {};
	}

	std::stringstream buffer;
	buffer << file.rdbuf();
	return U8String(buffer.str());
}

Box<Module> ModuleLoader::parseModule(const std::filesystem::path &path) {
	const auto filename = U8String(path.string());
	debugLog(m_Debug, std::string("parse ") + path.string());

	if (!std::filesystem::exists(path)) {
		m_ErrorHandler.registerSource(filename, u8"");
		m_ErrorHandler.addError(U8String(u8"Could not open file: '") + filename + U8String(u8"'."),
								SourceLoc{1, 1, 0, 1, filename});
		return nullptr;
	}

	auto source = readFile(path);

	m_ErrorHandler.registerSource(filename, source);
	auto tokens = Lexer::tokenize(source, m_ErrorHandler, filename);
	return Parser::parse(tokens, m_ErrorHandler, filename);
}

Module *ModuleLoader::getParsedModule(const std::filesystem::path &path) {
	const auto norm = path.lexically_normal();
	const auto key = norm.string();
	auto it = m_ModuleCache.find(key);
	if (it != m_ModuleCache.end()) {
		return it->second.get();
	}

	auto module = parseModule(norm);
	if (!module) {
		return nullptr;
	}

	auto [insertedIt, inserted] = m_ModuleCache.emplace(key, std::move(module));
	(void) inserted;
	return insertedIt->second.get();
}

std::filesystem::path ModuleLoader::resolveImport(const std::filesystem::path &currentPath,
												  const U8String &importPath) const {
	const auto baseDir = currentPath.parent_path();
	return (baseDir / std::filesystem::path(importPath.asAscii() + ".ocn")).lexically_normal();
}

Box<Module> ModuleLoader::flatten(Box<Module> root, Vec<Box<Module>> imports) {
	Vec<U8String> importPaths = std::move(root->imports);
	Vec<Box<FuncDecl>> funcs = std::move(root->funcs);
	Vec<Box<StructDecl>> structs = std::move(root->structs);

	for (auto &module : imports) {
		for (auto &func : module->funcs) {
			funcs.push_back(std::move(func));
		}

		for (auto &structDecl : module->structs) {
			structs.push_back(std::move(structDecl));
		}
	}

	auto merged = std::make_unique<Module>(std::move(root->name), std::move(importPaths),
										   std::move(funcs), std::move(structs));
	merged->setLoc(root->loc);
	return merged;
}

Box<Module> ModuleLoader::load(const std::filesystem::path &entryPath) {
	debugLog(m_Debug, std::string("load entry ") + entryPath.string());
	auto root = parseModule(entryPath);
	if (!root) {
		return nullptr;
	}

	Vec<Box<Module>> imports;
	std::unordered_set<std::string> visited;
	std::function<void(const std::filesystem::path &)> collect =
			[&](const std::filesystem::path &path) {
				const auto norm = path.lexically_normal();
				const auto key = norm.string();
				if (visited.contains(key))
					return;

				visited.insert(key);

				auto module = parseModule(norm);
				if (!module)
					return;

				imports.push_back(std::move(module));

				for (const auto &importPath : imports.back()->imports) {
					auto resolved = resolveImport(norm, importPath);
					debugLog(m_Debug, std::string("resolve import ") + importPath.asAscii() +
												" -> " + resolved.string());
					collect(resolved);
				}
			};

	for (const auto &importPath : root->imports) {
		auto resolved = resolveImport(entryPath, importPath);
		debugLog(m_Debug, std::string("resolve import ") + importPath.asAscii() + " -> " +
								  resolved.string());
		collect(resolved);
	}

	return flatten(std::move(root), std::move(imports));
}

static bool checkTypeDeclared(Type type, const std::unordered_set<U8String> &localStructs,
							  const std::unordered_set<U8String> &importStructs) {
	if (!type)
		return true;

	switch (type->kind) {
		case TypeKind::Struct: {
			auto *st = static_cast<StructType *>(type);
			if (localStructs.contains(st->name) || importStructs.contains(st->name))
				return true;
			return false;
		}
		case TypeKind::Array:
			return checkTypeDeclared(static_cast<ArrayType *>(type)->elementType, localStructs,
									 importStructs);
		case TypeKind::Pointer:
			return checkTypeDeclared(static_cast<PointerType *>(type)->pointeeType, localStructs,
									 importStructs);
		case TypeKind::Function: {
			auto *fn = static_cast<FunctionType *>(type);
			if (!checkTypeDeclared(fn->returnType, localStructs, importStructs))
				return false;
			for (auto p : fn->paramTypes) {
				if (!checkTypeDeclared(p, localStructs, importStructs))
					return false;
			}
			return true;
		}
		default: return true;
	}
}

bool ModuleLoader::validateStandalone(const std::filesystem::path &modulePath) {
	auto beforeErrors = m_ErrorHandler.errorCount();
	debugLog(m_Debug, std::string("validate standalone ") + modulePath.string());

	auto module = getParsedModule(modulePath);
	if (!module)
		return false;

	std::unordered_set<U8String> localStructs;
	for (auto &s : module->structs)
		localStructs.insert(s->ident);

	std::unordered_set<U8String> importStructs;
	std::unordered_set<std::string> visited;
	std::function<void(const std::filesystem::path &)> collect =
			[&](const std::filesystem::path &path) {
				const auto norm = path.lexically_normal();
				const auto key = norm.string();
				if (visited.contains(key))
					return;

				visited.insert(key);

				auto imported = getParsedModule(norm);
				if (!imported)
					return;

				for (auto &s : imported->structs)
					importStructs.insert(s->ident);

				for (const auto &imp : imported->imports) {
					auto resolved = resolveImport(norm, imp);
					debugLog(m_Debug, std::string("standalone import ") + imp.asAscii() +
												" -> " + resolved.string());
					collect(resolved);
				}
			};

	for (const auto &imp : module->imports) {
		auto resolved = resolveImport(modulePath, imp);
		debugLog(m_Debug,
				 std::string("standalone import ") + imp.asAscii() + " -> " + resolved.string());
		collect(resolved);
	}

	// Validate struct field types and function signatures
	for (auto &s : module->structs) {
		for (const auto &field : s->fields) {
			if (!checkTypeDeclared(field.second, localStructs, importStructs)) {
				m_ErrorHandler.addError(std::format("Undefined reference: {}", field.second->str()),
										s->loc);
			}
		}
	}

	for (auto &f : module->funcs) {
		for (const auto &param : f->params) {
			if (!checkTypeDeclared(param.second, localStructs, importStructs)) {
				m_ErrorHandler.addError(std::format("Undefined reference: {}", param.second->str()),
										f->loc);
			}
		}
		if (!checkTypeDeclared(f->returnType, localStructs, importStructs)) {
			m_ErrorHandler.addError(std::format("Undefined reference: {}", f->returnType->str()),
									f->loc);
		}
	}

	return m_ErrorHandler.errorCount() == beforeErrors;
}

bool ModuleLoader::validateImportGraph(const std::filesystem::path &entryPath) {
	debugLog(m_Debug, std::string("validate import graph ") + entryPath.string());
	std::unordered_set<std::string> visited;
	bool ok = true;

	std::function<void(const std::filesystem::path &)> visit =
			[&](const std::filesystem::path &path) {
				if (!ok)
					return;

				const auto norm = path.lexically_normal();
				const auto key = norm.string();
				if (visited.contains(key))
					return;

				debugLog(m_Debug, std::string("visit ") + key);

				visited.insert(key);

				auto module = getParsedModule(norm);
				if (!module) {
					ok = false;
					return;
				}

				std::unordered_set<U8String> localStructs;
				for (auto &s : module->structs)
					localStructs.insert(s->ident);

				std::unordered_set<U8String> importStructs;
				std::unordered_set<std::string> visitedImports;
				std::function<void(const std::filesystem::path &)> collectImports =
						[&](const std::filesystem::path &path) {
							const auto importNorm = path.lexically_normal();
							const auto importKey = importNorm.string();
							if (visitedImports.contains(importKey))
								return;

							visitedImports.insert(importKey);

							auto imported = getParsedModule(importNorm);
							if (!imported)
								return;

							for (auto &s : imported->structs)
								importStructs.insert(s->ident);

							for (const auto &imp : imported->imports) {
								auto resolved = resolveImport(importNorm, imp);
								collectImports(resolved);
							}
						};

				for (const auto &imp : module->imports) {
					auto resolved = resolveImport(norm, imp);
					collectImports(resolved);
				}

				for (auto &s : module->structs) {
					for (const auto &field : s->fields) {
						if (!checkTypeDeclared(field.second, localStructs, importStructs)) {
							m_ErrorHandler.addError(std::format("Undefined reference: {}",
																field.second->str()),
													s->loc);
							ok = false;
						}
					}
				}

				for (auto &f : module->funcs) {
					for (const auto &param : f->params) {
						if (!checkTypeDeclared(param.second, localStructs, importStructs)) {
							m_ErrorHandler.addError(std::format("Undefined reference: {}",
																param.second->str()),
													f->loc);
							ok = false;
						}
					}
					if (!checkTypeDeclared(f->returnType, localStructs, importStructs)) {
						m_ErrorHandler.addError(std::format("Undefined reference: {}",
															f->returnType->str()),
												f->loc);
						ok = false;
					}
				}

				for (const auto &imp : module->imports) {
					auto resolved = resolveImport(norm, imp);
					debugLog(m_Debug, std::string("graph edge ") + norm.string() + " -> " +
											  resolved.string());
					visit(resolved);
				}
			};

	visit(entryPath);
	return ok;
}

Vec<std::filesystem::path>
ModuleLoader::collectAllModulePaths(const std::filesystem::path &entryPath) {
	debugLog(m_Debug, std::string("collect module paths from ") + entryPath.string());
	Vec<std::filesystem::path> out;
	std::unordered_set<std::string> seen;

	std::function<void(const std::filesystem::path &)> walk = [&](const std::filesystem::path &p) {
		const auto norm = p.lexically_normal();
		const auto key = norm.string();
		if (seen.contains(key))
			return;
		debugLog(m_Debug, std::string("collect ") + key);

		seen.insert(key);
		out.push_back(norm);

		auto mod = getParsedModule(norm);
		if (!mod)
			return;

		for (const auto &imp : mod->imports) {
			auto resolved = resolveImport(norm, imp);
			debugLog(m_Debug,
					 std::string("collect edge ") + norm.string() + " -> " + resolved.string());
			walk(resolved);
		}
	};

	walk(entryPath);
	return out;
}
}