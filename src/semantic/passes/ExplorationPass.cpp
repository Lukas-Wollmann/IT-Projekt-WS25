#include "ExplorationPass.h"

#include <ranges>

#include "semantic/common/ErrorMessages.h"
#include "semantic/common/OperatorTable.h"
#include "type/TypeFactory.h"

namespace sem {
using namespace ast;
using enum ErrorMessageKind;

void ExplorationPass::validateNoCycles(StructType *root) {
	if (m_ValidatedStructs.contains(root))
		return;

	for (const auto &[name, type] : root->fields) {
		if (!type->isTypeKind(TypeKind::Struct))
			continue;

		auto struct_ = static_cast<StructType *>(type);
		std::vector<StructType *> path = {root};
		checkRecursive(struct_, path, name); // pass root field
	}

	m_ValidatedStructs.insert(root);
}

bool ExplorationPass::checkRecursive(StructType *current, Vec<StructType *> &path,
									 const U8String &rootField) {
	for (const auto &[name, type] : current->fields) {
		if (!type->isTypeKind(TypeKind::Struct))
			continue;

		auto struct_ = static_cast<StructType *>(type);

		for (const auto *seen : path) {
			if (struct_ == seen) {
				if (path.front() == m_CurrentRootBeingValidated) {
					const auto msg =
							ErrorMessage<StructInfiniteSize>::str(m_CurrentRootBeingValidated,
																  rootField);
					m_Context.submitError(msg, {});
				}
				return false;
			}
		}

		path.push_back(struct_);
		bool success = checkRecursive(struct_, path, rootField); // propagate root field
		path.pop_back();

		if (!success)
			return false;
	}

	return true;
}

ExplorationPass::ExplorationPass(TypeCheckerContext &ctx)
	: m_Context(ctx) {}

void ExplorationPass::visit(const Module &n) {
	for (auto &s : n.structs) {
		dispatch(*s);
	}

	// Validate that all struct field types are declared
	for (auto &s : n.structs) {
		auto structType = TypeFactory::getStruct(s->ident);
		for (const auto &[fieldName, fieldType] : s->fields) {
			if (fieldType->isTypeKind(TypeKind::Struct)) {
				auto *fieldStructType = static_cast<StructType *>(fieldType);
				if (!fieldStructType->isDeclared) {
					const auto msg = ErrorMessage<UndefinedReference>::str(fieldStructType->name);
					m_Context.submitError(msg, {});
				}
			}
		}
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
			structType->orderedFields.emplace_back(name, type);
		}
	}

	structType->isDeclared = true;
}

void ExplorationPass::visit(const FuncDecl &n) {
	TypeList params;

	// Validate that all parameter types are defined
	for (const auto &[name, type] : n.params) {
		if (type->isTypeKind(TypeKind::Struct)) {
			auto *structType = static_cast<StructType *>(type);
			if (!structType->isDeclared) {
				const auto msg = ErrorMessage<UndefinedReference>::str(structType->name);
				m_Context.submitError(msg, {});
			}
		}
		params.push_back(type);
	}

	// Validate return type is defined
	if (n.returnType->isTypeKind(TypeKind::Struct)) {
		auto *structType = static_cast<StructType *>(n.returnType);
		if (!structType->isDeclared) {
			const auto msg = ErrorMessage<UndefinedReference>::str(structType->name);
			m_Context.submitError(msg, {});
		}
	}

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