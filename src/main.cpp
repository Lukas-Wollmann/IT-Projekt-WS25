#include <fstream>
#include <iostream>
#include <memory>

#include "codegen/CodeGen.h"
#include "semantic/passes/ExplorationPass.h"
#include "semantic/passes/TypeCheckingPass.h"
#include "ast/PrintVisitor.h"

using namespace ast;
using namespace type;
using namespace semantic;
using namespace codegen;

int main() {
    TypePtr i32Type = std::make_shared<Typename>(u8"i32");

    Vec<Box<Stmt>> stmts;

    // int n = 20;
    stmts.push_back(std::make_unique<VarDef>(
        u8"n",
        std::make_unique<Typename>(u8"i32"),
        std::make_unique<IntLit>(20)
    ));

    // int a = 0;
    stmts.push_back(std::make_unique<VarDef>(
        u8"a",
        i32Type,
        std::make_unique<IntLit>(0)
    ));

    // int b = 1;
    stmts.push_back(std::make_unique<VarDef>(
        u8"b",
        i32Type,
        std::make_unique<IntLit>(1)
    ));

    // int temp = 0;  <-- Declare temp outside loop
    stmts.push_back(std::make_unique<VarDef>(
        u8"temp",
        i32Type,
        std::make_unique<IntLit>(0)
    ));

    // int i = 2;
    stmts.push_back(std::make_unique<VarDef>(
        u8"i",
        i32Type,
        std::make_unique<IntLit>(2)
    ));

    // while (i <= n) { ... }
    auto condWhile = std::make_unique<BinaryExpr>(
        BinaryOpKind::LessThanOrEqual,
        std::make_unique<VarRef>(u8"i"),
        std::make_unique<VarRef>(u8"n")
    );

    Vec<Box<Stmt>> bodyWhile;

    // temp = b;
    bodyWhile.push_back(std::make_unique<Assignment>(
        AssignmentKind::Simple,
        std::make_unique<VarRef>(u8"temp"),
        std::make_unique<VarRef>(u8"b")
    ));

    // b = a + b;
    bodyWhile.push_back(std::make_unique<Assignment>(
        AssignmentKind::Simple,
        std::make_unique<VarRef>(u8"b"),
        std::make_unique<BinaryExpr>(
            BinaryOpKind::Addition,
            std::make_unique<VarRef>(u8"a"),
            std::make_unique<VarRef>(u8"b")
        )
    ));

    // a = temp;
    bodyWhile.push_back(std::make_unique<Assignment>(
        AssignmentKind::Simple,
        std::make_unique<VarRef>(u8"a"),
        std::make_unique<VarRef>(u8"temp")
    ));

    // i = i + 1;
    bodyWhile.push_back(std::make_unique<Assignment>(
        AssignmentKind::Simple,
        std::make_unique<VarRef>(u8"i"),
        std::make_unique<BinaryExpr>(
            BinaryOpKind::Addition,
            std::make_unique<VarRef>(u8"i"),
            std::make_unique<IntLit>(1)
        )
    ));

    auto whileBlock = std::make_unique<BlockStmt>(std::move(bodyWhile));
    stmts.push_back(std::make_unique<WhileStmt>(
        std::move(condWhile),
        std::move(whileBlock)
    ));

    // return b;
    stmts.push_back(std::make_unique<ReturnStmt>(
        std::make_unique<VarRef>(u8"b")
    ));

    // function body
    auto body = std::make_unique<BlockStmt>(std::move(stmts));

    // function declaration
    Vec<Param> params; // no parameters needed
    auto func = std::make_unique<FuncDecl>(
        U8String("main"),
        std::move(params),
        i32Type,
        std::move(body)
    );

    Vec<Box<FuncDecl>> funcs;
    funcs.push_back(std::move(func));
    auto module = std::make_unique<Module>(u8"fib_module", std::move(funcs));

    // Semantic passes
    TypeCheckerContext ctx;
    ExplorationPass ep(ctx);
    TypeCheckingPass tc(ctx);

    ep.dispatch(*module);
    tc.dispatch(*module);

    if (!ctx.getErrors().empty()) {
        for (auto &err : ctx.getErrors())
            std::cout << err << std::endl;
        return 0;
    }

    // Code generation
    std::ofstream file("fib.ll");
    CodeGen cg(file);
    cg.dispatch(*module);

    return 0;
}
