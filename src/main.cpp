#include <iostream>
#include <memory>
#include <fstream>
#include "codegen/CodeGen.h"
#include "semantic/passes/ExplorationPass.h"
#include "semantic/passes/TypeCheckingPass.h"

using namespace ast;
using namespace type;

int main() {

    Vec<Box<Stmt>> stmts;
	
    // foo: i32 = 5
    stmts.push_back(std::make_unique<VarDef>(
        u8"foo",
        std::make_unique<Typename>(u8"i32"),
		std::make_unique<IntLit>(3)
    ));

    stmts.push_back(
        std::make_unique<ReturnStmt>(
            std::make_unique<BinaryExpr>(
                BinaryOpKind::Addition,
                std::make_unique<VarRef>(u8"foo"),
                std::make_unique<IntLit>(10)
            )
        )
    );

	auto funcDecl = std::make_unique<FuncDecl>(
        u8"main",
        Vec<Param>{}, 
        std::make_shared<Typename>(u8"i32"),
        std::make_unique<BlockStmt>(std::move(stmts))
    );

    Vec<Box<FuncDecl>> funcs;
    funcs.push_back(std::move(funcDecl));

    auto module = std::make_unique<Module>(u8"test", std::move(funcs));
    
    semantic::TypeCheckerContext ctx;
    semantic::ExplorationPass ep(ctx);
    semantic::TypeCheckingPass tc(ctx);
    
    ep.dispatch(*module);
    tc.dispatch(*module);

    if (!ctx.getErrors().empty()) {        
        for (auto &err : ctx.getErrors())
            std::cout << err << std::endl;

        return 0;
    }

    std::ofstream file("output.ll");
    codegen::CodeGen cg(file);
    cg.dispatch(*module);

    return 0;
}
