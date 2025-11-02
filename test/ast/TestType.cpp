#include "../Doctest.h"
#include "ast/Type.h"
#include <sstream>

TEST_CASE("PrimitiveType: equal typenames means equal") 
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);

    // Act
    bool equal = *primitiveType1 == *primitiveType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("PrimitiveType: different typenames means not equal") 
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::F32);

    // Act
    bool equal = *primitiveType1 == *primitiveType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("PointerType: equal underlying types means equal") 
{
    // Arrange
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
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
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::F32);
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
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
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
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
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
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::F32);
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
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::F32);
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
    auto primitiveType1 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto primitiveType2 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
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
    TypeList params1;
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));

    auto retType1 = std::make_unique<PrimitiveType>(PrimitiveKind::Bool);
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    TypeList params2;
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));

    auto retType2 = std::make_unique<PrimitiveType>(PrimitiveKind::Bool);
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("FunctionType: different parameters and equal return types means not equal")
{
    // Arrange
    TypeList params1;
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));

    auto retType1 = std::make_unique<PrimitiveType>(PrimitiveKind::Bool);
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    TypeList params2;
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));

    auto retType2 = std::make_unique<PrimitiveType>(PrimitiveKind::Bool);
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("FunctionType: equal parameters and different return types means not equal")
{
    // Arrange
    TypeList params1;
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));

    auto retType1 = std::make_unique<PrimitiveType>(PrimitiveKind::Bool);
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    TypeList params2;
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));

    auto retType2 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("FunctionType: different parameters and different return types means not equal")
{
    // Arrange
    TypeList params1;
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));
    params1.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));

    auto retType1 = std::make_unique<PrimitiveType>(PrimitiveKind::Bool);
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    TypeList params2;
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params2.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));

    auto retType2 = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("PrimitiveType: deep copy works")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveKind::I32);

    // Act
    auto copy = primitiveType->copy();

    // Assert
    CHECK(copy.get() != primitiveType.get());
    CHECK(copy->getKind() == Type::Kind::Primitive);
    CHECK(static_cast<const PrimitiveType&>(*copy).getPrimitive() == primitiveType->getPrimitive());
}

TEST_CASE("PointerType: deep copy works")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto ptrType = std::make_unique<PointerType>(std::move(primitiveType));

    // Act
    auto copy = ptrType->copy();

    // Assert
    CHECK(copy.get() != ptrType.get());
    CHECK(copy->getKind() == Type::Kind::Pointer);
    CHECK(static_cast<const PointerType&>(*copy).getPointeeType() == ptrType->getPointeeType());
}

TEST_CASE("ArrayType: deep copy works")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto arrType = std::make_unique<ArrayType>(std::move(primitiveType), 42);

    // Act
    auto copy = arrType->copy();

    // Assert
    CHECK(copy.get() != arrType.get());
    CHECK(copy->getKind() == Type::Kind::Array);
    CHECK(static_cast<const ArrayType&>(*copy).getElementType() == arrType->getElementType());
    CHECK(static_cast<const ArrayType&>(*copy).getArraySize() == 42);
}

TEST_CASE("FunctionType: deep copy works")
{
    TypeList params;
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));

    auto retType = std::make_unique<PrimitiveType>(PrimitiveKind::Bool);
    auto funcType = std::make_unique<FunctionType>(std::move(params), std::move(retType));

    // Act
    auto copy = funcType->copy();
    
    // Assert
    CHECK(copy.get() != funcType.get());
    CHECK(copy->getKind() == Type::Kind::Function);
    
    auto &copyFunc = static_cast<const FunctionType&>(*copy);

    CHECK(copyFunc.getParameterTypes() == funcType->getParameterTypes());
    CHECK(copyFunc.getReturnType() == funcType->getReturnType());
}

TEST_CASE("PrimitiveType: toString works")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    std::stringstream ss;

    // Act
    primitiveType->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "i32");
}

TEST_CASE("PointerType: toString works")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto ptrType = std::make_unique<PointerType>(std::move(primitiveType));
    std::stringstream ss;

    // Act
    ptrType->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "*i32");
}

TEST_CASE("ArrayType: toString works for sized")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto arrType = std::make_unique<ArrayType>(std::move(primitiveType), 42);
    std::stringstream ss;

    // Act
    arrType->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "[42]i32");
}

TEST_CASE("ArrayType: toString works for unsized")
{
    // Arrange
    auto primitiveType = std::make_unique<PrimitiveType>(PrimitiveKind::I32);
    auto arrType = std::make_unique<ArrayType>(std::move(primitiveType));
    std::stringstream ss;

    // Act
    arrType->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "[]i32");
}

TEST_CASE("FunctionType: toString works")
{
    // Arrange
    TypeList params;
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::I32));
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::F32));
    params.push_back(std::make_unique<PrimitiveType>(PrimitiveKind::Char));

    auto retType = std::make_unique<PrimitiveType>(PrimitiveKind::Bool);
    auto funcType = std::make_unique<FunctionType>(std::move(params), std::move(retType));
    std::stringstream ss;

    // Act
    funcType->toString(ss);
    std::string result = ss.str();

    // Assert
    CHECK(result == "(i32,f32,char)->(bool)");
}