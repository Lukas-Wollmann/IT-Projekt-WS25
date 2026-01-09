#include "Doctest.h"
#include "semantic/passes/ExplorationPass.h"

using namespace semantic;
using namespace ast;
using namespace type;

TEST_CASE("ExplorationPass: Visit Module visits all FuncDecls") {
    // Arrange
    TypeCheckerContext ctx;
    ExplorationPass ep(ctx);

    Vec<Box<FuncDecl>> funcDecls;
    funcDecls.push_back(std::make_unique<FuncDecl>(
        u8"foo",
        Vec<Param>{},
        std::make_shared<Typename>(u8"i32"),
        std::make_unique<BlockStmt>(Vec<Box<Stmt>>{})
    ));
    funcDecls.push_back(std::make_unique<FuncDecl>(
        u8"bar",
        Vec<Param>{},
        std::make_shared<UnitType>(),
        std::make_unique<BlockStmt>(Vec<Box<Stmt>>{})
    ));

    auto module = std::make_unique<Module>(u8"moduleName", std::move(funcDecls));

    // Act
    ep.dispatch(*module);

    // Assert
    auto &ns = ctx.getGlobalNamespace();
    CHECK(ns.getFunction(u8"foo").has_value());
    CHECK(ns.getFunction(u8"bar").has_value());
}