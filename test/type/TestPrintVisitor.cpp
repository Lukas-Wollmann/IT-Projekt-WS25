#include <sstream>

#include "Doctest.h"
#include "type/PrintVisitor.h"

using namespace type;

TEST_CASE("Typename: toString works") {
	// Arrange
	auto primitiveType = std::make_unique<Typename>(u8"i32");
	std::stringstream ss;

	// Act
	ss << *primitiveType;
	std::string result = ss.str();

	// Assert
	CHECK(result == "i32");
}

TEST_CASE("PointerType: toString works") {
	// Arrange
	auto primitiveType = std::make_unique<Typename>(u8"i32");
	auto ptrType = std::make_unique<PointerType>(std::move(primitiveType));
	std::stringstream ss;

	// Act
	ss << *ptrType;
	std::string result = ss.str();

	// Assert
	CHECK(result == "*i32");
}

TEST_CASE("ArrayType: toString works for sized") {
	// Arrange
	auto primitiveType = std::make_unique<Typename>(u8"i32");
	auto arrType = std::make_unique<ArrayType>(std::move(primitiveType), 42);
	std::stringstream ss;

	// Act
	ss << *arrType;
	std::string result = ss.str();

	// Assert
	CHECK(result == "[42]i32");
}

TEST_CASE("ArrayType: toString works for unsized") {
	// Arrange
	auto primitiveType = std::make_unique<Typename>(u8"i32");
	auto arrType = std::make_unique<ArrayType>(std::move(primitiveType));
	std::stringstream ss;

	// Act
	ss << *arrType;
	std::string result = ss.str();

	// Assert
	CHECK(result == "[]i32");
}

TEST_CASE("FunctionType: toString works") {
	// Arrange
	Vec<Box<const Type>> params;
	params.push_back(std::make_unique<Typename>(u8"i32"));
	params.push_back(std::make_unique<Typename>(u8"f32"));
	params.push_back(std::make_unique<Typename>(u8"char"));

	auto retType = std::make_unique<Typename>(u8"bool");
	auto funcType = std::make_unique<FunctionType>(std::move(params), std::move(retType));
	std::stringstream ss;

	// Act
	ss << *funcType;
	std::string result = ss.str();

	// Assert
	CHECK(result == "(i32, f32, char)->(bool)");
}

TEST_CASE("ErrorType: toString works") {
	// Arrange
	auto errorType = std::make_unique<ErrorType>();
	std::stringstream ss;

	// Act
	ss << *errorType;
	std::string result = ss.str();

	// Assert
	CHECK(result == "<error-type>");
}

TEST_CASE("UnitType: toString works") {
	// Arrange
	auto unitType = std::make_unique<UnitType>();
	std::stringstream ss;

	// Act
	ss << *unitType;
	std::string result = ss.str();

	// Assert
	CHECK(result == "()");
}