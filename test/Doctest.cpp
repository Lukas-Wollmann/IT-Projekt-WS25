#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "Doctest.h"
#include "../src/ast/Type.h"

TEST_CASE("Type: two equal value types are equal") 
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("i32");

    // Act
    bool equal = *valueType1 == *valueType2;

    // Assert
    CHECK(equal);
}

TEST_CASE("Type: two different value types are not equal") 
{
    // Arrange
    auto valueType1 = std::make_unique<ValueType>("i32");
    auto valueType2 = std::make_unique<ValueType>("f32");

    // Act
    bool equal = *valueType1 == *valueType2;

    // Assert
    CHECK(!equal);
}

TEST_CASE("Type: two equal pointer types are equal") 
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

TEST_CASE("Type: two different pointer types are not equal") 
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