#include "Doctest.h"
#include "type_checker/TypeChecker.h"

TEST_CASE("TypeChecker: IntLit type will be infered as PrimitiveType::I32")
{   
    // Arrange
    auto intLit = std::make_unique<IntLit>(67);
    TypeChecker tc;

    // Act
    intLit->accept(tc);

    // Assert
    CHECK(tc.m_Errors.empty());
    CHECK(intLit->getType().has_value());

    const Type &type = intLit->getType().value().get();
    CHECK(intLit->getType().value().get().getKind() == TypeKind::Primitive);
    
    auto &primitive = static_cast<const PrimitiveType&>(type);
    CHECK(primitive.getPrimitive() == PrimitiveTypeKind::I32);
}

TEST_CASE("TypeChecker: FloatLit type will be infered as PrimitiveType::F32")
{   
    // Arrange
    auto floatLit = std::make_unique<FloatLit>(187.0f);
    TypeChecker tc;

    // Act
    floatLit->accept(tc);

    // Assert
    CHECK(tc.m_Errors.empty());
    CHECK(floatLit->getType().has_value());

    const Type &type = floatLit->getType().value().get();
    CHECK(floatLit->getType().value().get().getKind() == TypeKind::Primitive);
    
    auto &primitive = static_cast<const PrimitiveType&>(type);
    CHECK(primitive.getPrimitive() == PrimitiveTypeKind::F32);
}

TEST_CASE("TypeChecker: CharLit type will be infered as PrimitiveType::Char")
{   
    // Arrange
    auto charLit = std::make_unique<CharLit>('X');
    TypeChecker tc;

    // Act
    charLit->accept(tc);

    // Assert
    CHECK(tc.m_Errors.empty());
    CHECK(charLit->getType().has_value());

    const Type &type = charLit->getType().value().get();
    CHECK(charLit->getType().value().get().getKind() == TypeKind::Primitive);
    
    auto &primitive = static_cast<const PrimitiveType&>(type);
    CHECK(primitive.getPrimitive() == PrimitiveTypeKind::Char);
}

TEST_CASE("TypeChecker: BoolLit type will be infered as PrimitiveType::Bool")
{   
    // Arrange
    auto boolLit = std::make_unique<BoolLit>(false);
    TypeChecker tc;

    // Act
    boolLit->accept(tc);

    // Assert
    CHECK(tc.m_Errors.empty());
    CHECK(boolLit->getType().has_value());

    const Type &type = boolLit->getType().value().get();
    CHECK(boolLit->getType().value().get().getKind() == TypeKind::Primitive);
    
    auto &primitive = static_cast<const PrimitiveType&>(type);
    CHECK(primitive.getPrimitive() == PrimitiveTypeKind::Bool);
}

TEST_CASE("TypeChecker: StringLit type will be infered as PrimitiveType::String")
{   
    // Arrange
    auto strLit = std::make_unique<StringLit>("UwU");
    TypeChecker tc;

    // Act
    strLit->accept(tc);

    // Assert
    CHECK(tc.m_Errors.empty());
    CHECK(strLit->getType().has_value());

    const Type &type = strLit->getType().value().get();
    CHECK(strLit->getType().value().get().getKind() == TypeKind::Primitive);
    
    auto &primitive = static_cast<const PrimitiveType&>(type);
    CHECK(primitive.getPrimitive() == PrimitiveTypeKind::String);
}

TEST_CASE("TypeChecker: ReturnStmt works if return expression has correct type")
{
    StmtList stmts;
    stmts.push_back(std::make_unique<ReturnStmt>(std::make_unique<FloatLit>(10.0f)));

    auto funcDecl = std::make_unique<FuncDecl>(
        "testFunction", 
        ParamList{}, 
        std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32), 
        std::make_unique<CodeBlock>(std::move(stmts))
    );

    TypeChecker tc;
    funcDecl->accept(tc);

    for (const TypeError &err : tc.m_Errors)
        std::cout << err.m_Msg << std::endl;
}

TEST_CASE("TypeChecker: Sandbox")
{
    StmtList stmts;
    
    ParamList params;
    params.push_back({ "a", std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32) });
    params.push_back({ "b", std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32) });

    StmtList body;
    body.push_back(std::make_unique<ReturnStmt>(
        std::make_unique<IntLit>(20)
    ));

    stmts.push_back(std::make_unique<FuncDecl>(
        "add",
        std::move(params),
        std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32),
        std::make_unique<CodeBlock>(std::move(body))
    ));

    ExprList args;
    args.push_back(std::make_unique<IntLit>(20));
    args.push_back(std::make_unique<IntLit>(15));
    args.push_back(std::make_unique<IntLit>(15));

    stmts.push_back(std::make_unique<VarDecl>(
        "x",
        std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32),
        std::make_unique<FuncCall>(
            "add",
            std::move(args)
        )
    ));

    auto block = std::make_unique<CodeBlock>(std::move(stmts));

    TypeChecker tc;
    block->accept(tc);


    tc.m_SymbolTable.getGlobalScope()->toString(std::cout);

    for (const TypeError &err : tc.m_Errors)
        std::cout << err << std::endl;
}