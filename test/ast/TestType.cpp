#include "Doctest.h"
#include <sstream>
#include "type/Type.h"
#include "type/PrintVisitor.h"
#include "type/CompareVisitor.h"
#include "type/CloneVisitor.h"

using namespace type;

TEST_CASE("PrimitiveType: equal typenames means equal") 
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);

    // Act
    bool equal = *primitiveType1 == *primitiveType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("PrimitiveType: different typenames means not equal") 
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32);

    // Act
    bool equal = *primitiveType1 == *primitiveType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("PointerType: equal underlying types means equal") 
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto ptrType1 = std::make_unique<PointerType>(std::move(primitiveType1));
    auto ptrType2 = std::make_unique<PointerType>(std::move(primitiveType2));

    // Act
    bool equal = *ptrType1 == *ptrType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("PointerType: different underlying types means not equal") 
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32);
    auto ptrType1 = std::make_unique<PointerType>(std::move(primitiveType1));
    auto ptrType2 = std::make_unique<PointerType>(std::move(primitiveType2));

    // Act
    bool equal = *ptrType1 == *ptrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ArrayType: equal size and equal type means equal")
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto arrType1 = std::make_unique<ArrayType>(std::move(primitiveType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(primitiveType2), 42);

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("ArrayType: different size and equal type means not equal")
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto arrType1 = std::make_unique<ArrayType>(std::move(primitiveType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(primitiveType2), 67);

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ArrayType: equal size and different type means not equal")
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32);
    auto arrType1 = std::make_unique<ArrayType>(std::move(primitiveType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(primitiveType2), 42);

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ArrayType: different size and different type means not equal")
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32);
    auto arrType1 = std::make_unique<ArrayType>(std::move(primitiveType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(primitiveType2), 67);

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ArrayType: unsized and sized array means not equal")
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto arrType1 = std::make_unique<ArrayType>(std::move(primitiveType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(primitiveType2));

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("FunctionType: equal parameters and equal return types means equal")
{
    // Arrange
    Vec<Box<const Type>> params1;
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));

    auto retType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    Vec<Box<const Type>> params2;
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));

    auto retType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("FunctionType: different parameters and equal return types means not equal")
{
    // Arrange
    Vec<Box<const Type>> params1;
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));

    auto retType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    Vec<Box<const Type>> params2;
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));

    auto retType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("FunctionType: equal parameters and different return types means not equal")
{
    // Arrange
    Vec<Box<const Type>> params1;
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));

    auto retType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    Vec<Box<const Type>> params2;
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));

    auto retType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("FunctionType: different parameters and different return types means not equal")
{
    // Arrange
    Vec<Box<const Type>> params1;
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));

    auto retType1 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    Vec<Box<const Type>> params2;
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));

    auto retType2 = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("PrimitiveType: deep copy works")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);

    // Act
    auto copy = clone(*primitiveType);

    // Assert
    CHECK(copy.get() != primitiveType.get());
    CHECK(copy->kind == TypeKind::Primitive);
    CHECK(static_cast<const PrimitiveType&>(*copy).primitiveKind == primitiveType->primitiveKind);
}

TEST_CASE("PointerType: deep copy works")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto ptrType = std::make_unique<PointerType>(std::move(primitiveType));

    // Act
    auto copy = clone(*ptrType);

    // Assert
    CHECK(copy.get() != ptrType.get());
    CHECK(copy->kind == TypeKind::Pointer);
    CHECK(*static_cast<const PointerType&>(*copy).pointeeType == *ptrType->pointeeType);
}

TEST_CASE("ArrayType: deep copy works")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32);
    auto arrType = std::make_unique<ArrayType>(std::move(primitiveType), 42);

    // Act
    auto copy = clone(*arrType);

    // Assert
    CHECK(copy.get() != arrType.get());
    CHECK(copy->kind == TypeKind::Array);
    CHECK(*static_cast<const ArrayType&>(*copy).elementType == *arrType->elementType);
    CHECK(static_cast<const ArrayType&>(*copy).arraySize == 42);
}

TEST_CASE("FunctionType: deep copy works")
{
    Vec<Box<const Type>> params;
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::I32));
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::F32));
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveTypeKind::Char));

    auto retType = std::make_unique<PrimitiveType>(PrimitiveTypeKind::Bool);
    auto funcType = std::make_unique<FunctionType>(std::move(params), std::move(retType));

    // Act
    auto copy = clone(*funcType);

    // Assert
    CHECK(copy.get() != funcType.get());
    CHECK(copy->kind == TypeKind::Function);
    
    auto &copyFunc = static_cast<const FunctionType&>(*copy);

    CHECK(copyFunc.paramTypes.size() == funcType->paramTypes.size());
    
    for (size_t i = 0; i < copyFunc.paramTypes.size(); ++i)
        CHECK(*copyFunc.paramTypes[i] == *funcType->paramTypes[i]);

    CHECK(*copyFunc.returnType == *funcType->returnType);
}