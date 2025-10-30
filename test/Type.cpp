#include "Doctest.h"
#include "../src/ast/Type.h"
#include <sstream>


TEST_CASE("ValueType: equal typenames means equal") 
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("i32");

    // Act
    bool equal = *valueType1 == *valueType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("ValueType: different typenames means not equal") 
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("f32");

    // Act
    bool equal = *valueType1 == *valueType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("PointerType: equal underlying types means equal") 
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("i32");
    auto ptrType1 = std::make_unique<PointerType>(std::move(valueType1));
    auto ptrType2 = std::make_unique<PointerType>(std::move(valueType2));

    // Act
    bool equal = *ptrType1 == *ptrType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("PointerType: different underlying types means not equal") 
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("f32");
    auto ptrType1 = std::make_unique<PointerType>(std::move(valueType1));
    auto ptrType2 = std::make_unique<PointerType>(std::move(valueType2));

    // Act
    bool equal = *ptrType1 == *ptrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ArrayType: equal size and equal type means equal")
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("i32");
    auto arrType1 = std::make_unique<ArrayType>(std::move(valueType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(valueType2), 42);

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("ArrayType: different size and equal type means not equal")
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("i32");
    auto arrType1 = std::make_unique<ArrayType>(std::move(valueType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(valueType2), 67);

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ArrayType: equal size and different type means not equal")
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("f32");
    auto arrType1 = std::make_unique<ArrayType>(std::move(valueType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(valueType2), 42);

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ArrayType: different size and different type means not equal")
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("f32");
    auto arrType1 = std::make_unique<ArrayType>(std::move(valueType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(valueType2), 67);

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ArrayType: unsized and sized array means not equal")
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("i32");
    auto arrType1 = std::make_unique<ArrayType>(std::move(valueType1), 42);
    auto arrType2 = std::make_unique<ArrayType>(std::move(valueType2));

    // Act
    bool equal = *arrType1 == *arrType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("FunctionType: equal parameters and equal return types means equal")
{
    // Arrange
    TypeList params1;
    params1.push_back(std::make_unique<ValueType>("i32"));
    params1.push_back(std::make_unique<ValueType>("f32"));
    params1.push_back(std::make_unique<ValueType>("char"));

    auto retType1 = std::make_unique<ValueType>("bool");
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    TypeList params2;
    params2.push_back(std::make_unique<ValueType>("i32"));
    params2.push_back(std::make_unique<ValueType>("f32"));
    params2.push_back(std::make_unique<ValueType>("char"));

    auto retType2 = std::make_unique<ValueType>("bool");
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
    params1.push_back(std::make_unique<ValueType>("i32"));
    params1.push_back(std::make_unique<ValueType>("f32"));
    params1.push_back(std::make_unique<ValueType>("char"));

    auto retType1 = std::make_unique<ValueType>("bool");
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    TypeList params2;
    params2.push_back(std::make_unique<ValueType>("char"));
    params2.push_back(std::make_unique<ValueType>("i32"));
    params2.push_back(std::make_unique<ValueType>("f32"));

    auto retType2 = std::make_unique<ValueType>("bool");
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
    params1.push_back(std::make_unique<ValueType>("i32"));
    params1.push_back(std::make_unique<ValueType>("char"));
    params1.push_back(std::make_unique<ValueType>("f32"));

    auto retType1 = std::make_unique<ValueType>("bool");
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    TypeList params2;
    params2.push_back(std::make_unique<ValueType>("i32"));
    params2.push_back(std::make_unique<ValueType>("char"));
    params2.push_back(std::make_unique<ValueType>("f32"));

    auto retType2 = std::make_unique<ValueType>("i32");
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
    params1.push_back(std::make_unique<ValueType>("i32"));
    params1.push_back(std::make_unique<ValueType>("char"));
    params1.push_back(std::make_unique<ValueType>("f32"));

    auto retType1 = std::make_unique<ValueType>("bool");
    auto funcType1 = std::make_unique<FunctionType>(std::move(params1), std::move(retType1));

    
    TypeList params2;
    params2.push_back(std::make_unique<ValueType>("f32"));
    params2.push_back(std::make_unique<ValueType>("i32"));
    params2.push_back(std::make_unique<ValueType>("char"));

    auto retType2 = std::make_unique<ValueType>("i32");
    auto funcType2 = std::make_unique<FunctionType>(std::move(params2), std::move(retType2));

    // Act
    bool equal = *funcType1 == *funcType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("ValueType: deep copy works")
{
    // Arrange
    auto valueType = std::make_unique<ValueType>("i32");

    // Act
    auto copy = valueType->copy();

    // Assert
    CHECK(copy.get() != valueType.get());
    CHECK(copy->getKind() == Type::Kind::Value);
    CHECK(static_cast<const ValueType&>(*copy).getTypename() == valueType->getTypename());
}

TEST_CASE("PointerType: deep copy works")
{
    // Arrange
    auto valueType = std::make_unique<ValueType>("i32");
    auto ptrType = std::make_unique<PointerType>(std::move(valueType));

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
    auto valueType = std::make_unique<ValueType>("i32");
    auto arrType = std::make_unique<ArrayType>(std::move(valueType), 42);

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
    params.push_back(std::make_unique<ValueType>("i32"));
    params.push_back(std::make_unique<ValueType>("f32"));
    params.push_back(std::make_unique<ValueType>("char"));

    auto retType = std::make_unique<ValueType>("bool");
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

TEST_CASE("ValueType: toString works")
{
    // Arrange
    auto valueType = std::make_unique<ValueType>("i32");

    // Act
    std::stringstream ss;
    ss << *valueType;
    std::string result = ss.str();

    // Assert
    CHECK(result == "i32");
}

TEST_CASE("PointerType: toString works")
{
    // Arrange
    auto valueType = std::make_unique<ValueType>("i32");
    auto ptrType = std::make_unique<PointerType>(std::move(valueType));

    // Act
    std::stringstream ss;
    ss << *ptrType;
    std::string result = ss.str();

    // Assert
    CHECK(result == "*i32");
}

TEST_CASE("ArrayType: toString works for sized")
{
    // Arrange
    auto valueType = std::make_unique<ValueType>("i32");
    auto arrType = std::make_unique<ArrayType>(std::move(valueType), 42);

    // Act
    std::stringstream ss;
    ss << *arrType;
    std::string result = ss.str();

    // Assert
    CHECK(result == "[42]i32");
}

TEST_CASE("ArrayType: toString works for unsized")
{
    // Arrange
    auto valueType = std::make_unique<ValueType>("i32");
    auto arrType = std::make_unique<ArrayType>(std::move(valueType));

    // Act
    std::stringstream ss;
    ss << *arrType;
    std::string result = ss.str();

    // Assert
    CHECK(result == "[]i32");
}

TEST_CASE("FunctionType: toString works")
{
    // Arrange
    TypeList params;
    params.push_back(std::make_unique<ValueType>("i32"));
    params.push_back(std::make_unique<ValueType>("f32"));
    params.push_back(std::make_unique<ValueType>("char"));

    auto retType = std::make_unique<ValueType>("bool");
    auto funcType = std::make_unique<FunctionType>(std::move(params), std::move(retType));

    // Act
    std::stringstream ss;
    ss << *funcType;
    std::string result = ss.str();

    // Assert
    CHECK(result == "(i32,f32,char)->(bool)");
}