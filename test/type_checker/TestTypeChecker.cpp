#include "Doctest.h"
#include "type_checker/TypeChecker.h"

TEST_CASE("TypeChecker: FloatLit type get set to PrimitiveType::F32")
{   
    // Arrange
    auto floatLit = std::make_unique<FloatLit>(10.0f);
    TypeChecker tc;

    // Act
    tc.checkExpr(*floatLit);

    // Assert
    CHECK(floatLit->getType().has_value());

    const Type &type = floatLit->getType().value().get();
    CHECK(floatLit->getType().value().get().getKind() == Type::Kind::Primitive);
    
    auto &primitive = dynamic_cast<const PrimitiveType&>(type);
    CHECK(primitive.getPrimitive() == PrimitiveKind::F32);

    CHECK(tc.m_Errors.empty());
}   

TEST_CASE("TypeChecker: Sandbox")
{
    StmtList stmts;

    stmts.push_back(std::make_unique<VarDecl>(
        "foo", 
        std::make_unique<PrimitiveType>(PrimitiveKind::I32),
        std::make_unique<FloatLit>(10.0f)
    ));
    
    stmts.push_back(std::make_unique<BinaryExpr>(
        BinaryOperatorKind::Addition,
        std::make_unique<FloatLit>(10.0f),
        std::make_unique<VarRef>("foo")
    ));

    auto block = std::make_unique<CodeBlock>(std::move(stmts));


    TypeChecker tc;

    tc.checkStmt(*block);

    for (const TypeError &err : tc.m_Errors)
        std::cout << err.m_Msg << std::endl;
}