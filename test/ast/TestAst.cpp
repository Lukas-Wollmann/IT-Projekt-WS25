#include "Doctest.h"
#include <sstream>
#include "ast/AST.h"

TEST_CASE("IntLit: toString works")
{
    // Arrange
    auto intLit = std::make_unique<IntLit>(42);
    std::stringstream ss;

    // Act
    intLit->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "IntLit(42)");
}

TEST_CASE("FloatLit: toString works")
{
    // Arrange
    auto floatLit = std::make_unique<FloatLit>(67.4f);
    std::stringstream ss;

    // Act
    floatLit->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "FloatLit(67.4)");
}


TEST_CASE("CharLit: toString works")
{
    // Arrange
    char32_t flowerEmoji = U'\U0001f33a';
    auto charLit = std::make_unique<CharLit>(flowerEmoji);
    std::stringstream ss;
    
    // Act
    charLit->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "CharLit('\U0001f33a')");
}

TEST_CASE("BoolLit: toString works")
{
    // Arrange
    auto boolLit = std::make_unique<BoolLit>(true);
    std::stringstream ss;
    
    // Act
    boolLit->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "BoolLit(1)");
}

TEST_CASE("StringLit: toString works")
{
    // Arrange
    auto strLit = std::make_unique<StringLit>("burger king");
    std::stringstream ss;

    // Act
    strLit->toString(ss);
    std::string result = ss.str();

    CHECK(result == "StringLit(\"burger king\")");
}

TEST_CASE("ArrayExpr: toString works")
{
    // Arrange
    auto arrayType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto value1 = std::make_unique<IntLit>(187);
    auto value2 = std::make_unique<IntLit>(67);
    
    ExprList exprs;
    exprs.push_back(std::move(value1));
    exprs.push_back(std::move(value2));

    auto arrayExpr = std::make_unique<ArrayExpr>(std::move(arrayType), std::move(exprs));

    std::stringstream ss;

    // Act
    arrayExpr->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "ArrayExpr(i32, {IntLit(187), IntLit(67)})");
}

TEST_CASE("UnaryExpr: toString works")
{
    // Arrange
    auto operand = std::make_unique<IntLit>(7);
    auto unary = std::make_unique<UnaryExpr>(UnaryOpKind::Negative, std::move(operand));
    std::stringstream ss;

    // Act
    unary->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "UnaryExpr(IntLit(7), Negative)");
}

TEST_CASE("BinaryExpr: toString works")
{
    // Arrange
    auto left = std::make_unique<IntLit>(2);
    auto right = std::make_unique<IntLit>(3);
    auto bin = std::make_unique<BinaryExpr>(BinaryOpKind::Addition, std::move(left), std::move(right));
    std::stringstream ss;

    // Act
    bin->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "BinaryExpr(IntLit(2), IntLit(3), Addition)");
}

TEST_CASE("VarRef: toString works")
{
    // Arrange
    auto var = std::make_unique<VarRef>("x");
    std::stringstream ss;

    // Act
    var->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "VarRef(x)");
}

TEST_CASE("FuncCall: toString works")
{
    // Arrange
    ExprList args;
    args.push_back(std::make_unique<IntLit>(1));
    args.push_back(std::make_unique<IntLit>(2));
    
    auto call = std::make_unique<FuncCall>("sum", std::move(args));
    std::stringstream ss;

    // Act
    call->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "FuncCall(sum, {IntLit(1), IntLit(2)})");
}

TEST_CASE("CodeBlock: toString work")
{
    // Arrange
    StmtList stmts;
    stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(1)));
    stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(2)));
    
    auto block = std::make_unique<CodeBlock>(std::move(stmts));
    std::stringstream ss;
    
    // Act
    block->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "CodeBlock({ReturnStmt(IntLit(1)), ReturnStmt(IntLit(2))})");
}

TEST_CASE("IfStmt: toString works")
{
    // Arrange
    auto cond = std::make_unique<BoolLit>(true);
    auto thenBlock = std::make_unique<CodeBlock>(StmtList{});
    auto elseBlock = std::make_unique<CodeBlock>(StmtList{});
    
    auto ifs = std::make_unique<IfStmt>(std::move(cond), std::move(thenBlock), std::move(elseBlock));
    std::stringstream ss;

    // Act
    ifs->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "IfStmt(BoolLit(1), CodeBlock({}), CodeBlock({}))");
}

TEST_CASE("WhileStmt: toString works")
{
    // Arrange
    StmtList stmts;
    stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(0)));
    
    auto body = std::make_unique<CodeBlock>(std::move(stmts));
    auto whileCond = std::make_unique<BoolLit>(false);
    auto wh = std::make_unique<WhileStmt>(std::move(whileCond), std::move(body));
    std::stringstream ss;
    
    // Act
    wh->toString(ss);
    std::string result = ss.str();

    // Assert 
    CHECK(result == "WhileStmt(BoolLit(0), CodeBlock({ReturnStmt(IntLit(0))}))");
}

TEST_CASE("ReturnStmt: toString works")
{
    // Arrange
    auto retExpr = std::make_unique<IntLit>(7);
    auto ret = std::make_unique<ReturnStmt>(std::move(retExpr));
    std::stringstream ss;
    
    // Act    
    ret->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "ReturnStmt(IntLit(7))");
}

TEST_CASE("VarDecl: toString works")
{
    // Arrange
    auto type = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto val = std::make_unique<IntLit>(99);
    auto decl = std::make_unique<VarDecl>("myVar", std::move(type), std::move(val));
    std::stringstream ss;

    // Act
    decl->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "VarDecl(myVar, i32, IntLit(99))");
}

TEST_CASE("FuncDecl: toString works")
{
    // Arrange
    ParamList params;
    params.push_back({ "a", std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32) });
    params.push_back({ "b", std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32) });

    StmtList stmts;
    stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<IntLit>(0)));

    auto body = std::make_unique<CodeBlock>(std::move(stmts));
    auto retType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
    auto func = std::make_unique<FuncDecl>("foo", std::move(params), std::move(retType), std::move(body));
    std::stringstream ss;

    // Act
    func->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result =="FuncDecl(foo, {a: i32, b: f32}, bool, CodeBlock({ReturnStmt(IntLit(0))}))");
}