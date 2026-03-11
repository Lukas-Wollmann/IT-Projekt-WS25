#include "ExplorationPass.h"

#include <ranges>

#include "semantic/common/ErrorMessages.h"
#include "semantic/common/OperatorTable.h"
#include "type/TypeFactory.h"

namespace sem {
using namespace ast;
using enum ErrorMessageKind;

void ExplorationPass::validateNoCycles(StructType *root) {
	if (m_ValidatedStructs.contains(root)) {
		return;
	}

	std::vector<StructType *> path = {root};
	checkRecursive(root, path);

	m_ValidatedStructs.insert(root);
}

bool ExplorationPass::checkRecursive(StructType *current, Vec<StructType *> &path) {
	for (const auto &[name, type] : current->fields) {
		if (type->isTypeKind(TypeKind::Struct)) {
			auto struct_ = static_cast<StructType *>(type);

			for (const auto *seen : path) {
				if (struct_ == seen) {
					// Only error for the root
					if (path.front() == m_CurrentRootBeingValidated) {
						const auto msg = ErrorMessage<StructInfiniteSize>::str(path.front(), name);
						m_Context.submitError(msg, {});
					}

					return false;
				}
			}

			path.push_back(struct_);
			bool success = checkRecursive(struct_, path);
			path.pop_back();

			if (!success) {
				return false;
			}
		}
	}

	return true;
}

ExplorationPass::ExplorationPass(TypeCheckerContext &ctx)
	: m_Context(ctx) {}

void ExplorationPass::visit(const Module &n) {
	for (auto &s : n.structs) {
		dispatch(*s);
	}

	m_ValidatedStructs.clear();
	for (auto &s : n.structs) {
		auto root = TypeFactory::getStruct(s->ident);
		m_CurrentRootBeingValidated = root;

		validateNoCycles(root);
	}

	for (auto &d : n.funcs) {
		dispatch(*d);
	}
}

void ExplorationPass::visit(const StructDecl &n) {
	const auto structType = TypeFactory::getStruct(n.ident);

	if (structType->isDeclared) {
		const auto msg = ErrorMessage<SymbolRedefinition>::str(n.ident);
		m_Context.submitError(msg, {});

		return;
	}

	for (const auto &[name, type] : n.fields) {
		if (structType->fields.contains(name)) {
			const auto msg = ErrorMessage<StructFieldRedefinition>::str(name);
			m_Context.submitError(msg, {});
		} else {
			structType->fields.insert({name, type});
		}
	}

	structType->isDeclared = true;
}

void ExplorationPass::visit(const FuncDecl &n) {
	TypeList params;

	std::ranges::copy(n.params | std::views::values, std::back_inserter(params));

	const auto funcType = TypeFactory::getFunction(std::move(params), n.returnType);
	auto &global = m_Context.getGlobalNamespace();

	if (global.getFunction(n.ident)) {
		const auto msg = ErrorMessage<SymbolRedefinition>::str(n.ident);
		m_Context.submitError(msg, {});

		return;
	}

	global.addFunction(n.ident, funcType);
}
}