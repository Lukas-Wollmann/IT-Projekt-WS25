#include "ast/AST.h"
#include <iostream>

int main() {
    // Build inner while loop statements
    StmtList whileStmts;
    whileStmts.push_back(std::make_unique<VarDecl>(
        "x",
        std::make_unique<PrimitiveType>(PrimitiveKind::I32),
        std::make_unique<BinaryExpr>(
            BinaryOperatorKind::Multiplication,
            std::make_unique<VarRef>("x"),
            std::make_unique<CharLit>(U'\U0001F600')
        )
    ));
    auto whileLoop = std::make_unique<WhileStmt>(
        std::make_unique<BinaryExpr>(
            BinaryOperatorKind::LessThan,
            std::make_unique<VarRef>("x"),
            std::make_unique<IntLit>(100)
        ),
        std::make_unique<CodeBlock>(std::move(whileStmts))
    );

    // Build if-else statements
    StmtList thenStmts;
    thenStmts.push_back(std::make_unique<VarDecl>(
        "x",
        std::make_unique<PrimitiveType>(PrimitiveKind::I32),
        std::make_unique<BinaryExpr>(
            BinaryOperatorKind::Subtraction,
            std::make_unique<VarRef>("x"),
            std::make_unique<IntLit>(10)
        )
    ));
    StmtList elseStmts;
    elseStmts.push_back(std::make_unique<VarDecl>(
        "x",
        std::make_unique<PrimitiveType>(PrimitiveKind::I32),
        std::make_unique<BinaryExpr>(
            BinaryOperatorKind::Addition,
            std::make_unique<VarRef>("x"),
            std::make_unique<IntLit>(10)
        )
    ));
    auto ifStmt = std::make_unique<IfStmt>(
        std::make_unique<BinaryExpr>(
            BinaryOperatorKind::GreaterThan,
            std::make_unique<VarRef>("x"),
            std::make_unique<IntLit>(10)
        ),
        std::make_unique<CodeBlock>(std::move(thenStmts)),
        std::make_unique<CodeBlock>(std::move(elseStmts))
    );

    // Build initial variable declaration
    auto varX = std::make_unique<VarDecl>(
        "x",
        std::make_unique<PrimitiveType>(PrimitiveKind::I32),
        std::make_unique<BinaryExpr>(
            BinaryOperatorKind::Addition,
            std::make_unique<VarRef>("a"),
            std::make_unique<VarRef>("b")
        )
    );

    // Assemble function body
    StmtList funcStmts;
    funcStmts.push_back(std::move(varX));
    funcStmts.push_back(std::move(ifStmt));
    funcStmts.push_back(std::move(whileLoop));
    funcStmts.push_back(std::make_unique<ReturnStmt>(
        std::make_unique<VarRef>("x")
    ));

    auto funcBody = std::make_unique<CodeBlock>(std::move(funcStmts));

    // Build function parameters
    ParamList params;
    params.push_back(std::make_unique<Param>("a", std::make_unique<PrimitiveType>(PrimitiveKind::I32)));
    params.push_back(std::make_unique<Param>("b", std::make_unique<PrimitiveType>(PrimitiveKind::I32)));

    // Build function declaration
    auto func = std::make_unique<FuncDecl>(
        "complexFunction",
        std::move(params),
        std::make_unique<PrimitiveType>(PrimitiveKind::I32),
        std::move(funcBody)
    );

    // Print AST
    func->toString(std::cout);
    std::cout << std::endl;

    return 0;
}
