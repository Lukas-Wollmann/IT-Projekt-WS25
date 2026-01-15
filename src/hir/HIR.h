#pragma once

namespace hir {
	enum struct NodeKind {
		IntLit,
		CharLit,
		BoolLit,
		UnitLit,
		UnaryExpr,
		BinaryExpr,
		HeapAlloc,
		Assignment,
		VarRef,
		FuncCall,
		LifetimeScope,
		IfStmt,
		WhileStmt,
		ReturnStmt,
		VarDef,
		FuncDecl,
		Module
	};
	// This is a expression that needs to free
	struct TempExpr {};
}
