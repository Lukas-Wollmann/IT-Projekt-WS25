#include "Doctest.h"
#include "type/CompareVisitor.h"

using namespace type;

TEST_CASE("Typename: equal typenames means equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"i32");

	// Act
	bool equal = *primitiveType1 == *primitiveType2;

	// Assert
	CHECK(equal);
}

TEST_CASE("Typename: different typenames means not equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"f32");

	// Act
	bool equal = *primitiveType1 == *primitiveType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("PointerType: equal underlying types means equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"i32");
	auto ptrType1 = std::make_shared<PointerType>(std::move(primitiveType1));
	auto ptrType2 = std::make_shared<PointerType>(std::move(primitiveType2));

	// Act
	bool equal = *ptrType1 == *ptrType2;

	// Assert
	CHECK(equal);
}

TEST_CASE("PointerType: different underlying types means not equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"f32");
	auto ptrType1 = std::make_shared<PointerType>(std::move(primitiveType1));
	auto ptrType2 = std::make_shared<PointerType>(std::move(primitiveType2));

	// Act
	bool equal = *ptrType1 == *ptrType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("ArrayType: equal size and equal type means equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"i32");
	auto arrType1 = std::make_shared<ArrayType>(std::move(primitiveType1), 42);
	auto arrType2 = std::make_shared<ArrayType>(std::move(primitiveType2), 42);

	// Act
	bool equal = *arrType1 == *arrType2;

	// Assert
	CHECK(equal);
}

TEST_CASE("ArrayType: different size and equal type means not equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"i32");
	auto arrType1 = std::make_shared<ArrayType>(std::move(primitiveType1), 42);
	auto arrType2 = std::make_shared<ArrayType>(std::move(primitiveType2), 67);

	// Act
	bool equal = *arrType1 == *arrType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("ArrayType: equal size and different type means not equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"f32");
	auto arrType1 = std::make_shared<ArrayType>(std::move(primitiveType1), 42);
	auto arrType2 = std::make_shared<ArrayType>(std::move(primitiveType2), 42);

	// Act
	bool equal = *arrType1 == *arrType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("ArrayType: different size and different type means not equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"f32");
	auto arrType1 = std::make_shared<ArrayType>(std::move(primitiveType1), 42);
	auto arrType2 = std::make_shared<ArrayType>(std::move(primitiveType2), 67);

	// Act
	bool equal = *arrType1 == *arrType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("ArrayType: unsized and sized array means not equal") {
	// Arrange
	auto primitiveType1 = std::make_shared<Typename>(u8"i32");
	auto primitiveType2 = std::make_shared<Typename>(u8"i32");
	auto arrType1 = std::make_shared<ArrayType>(std::move(primitiveType1), 42);
	auto arrType2 = std::make_shared<ArrayType>(std::move(primitiveType2));

	// Act
	bool equal = *arrType1 == *arrType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("FunctionType: equal parameters and equal return types means equal") {
	// Arrange
	TypeList params1;
	params1.push_back(std::make_shared<Typename>(u8"i32"));
	params1.push_back(std::make_shared<Typename>(u8"f32"));
	params1.push_back(std::make_shared<Typename>(u8"char"));

	auto retType1 = std::make_shared<Typename>(u8"bool");
	auto funcType1 = std::make_shared<FunctionType>(std::move(params1), std::move(retType1));

	TypeList params2;
	params2.push_back(std::make_shared<Typename>(u8"i32"));
	params2.push_back(std::make_shared<Typename>(u8"f32"));
	params2.push_back(std::make_shared<Typename>(u8"char"));

	auto retType2 = std::make_shared<Typename>(u8"bool");
	auto funcType2 = std::make_shared<FunctionType>(std::move(params2), std::move(retType2));

	// Act
	bool equal = *funcType1 == *funcType2;

	// Assert
	CHECK(equal);
}

TEST_CASE("FunctionType: different parameters and equal return types means not equal") {
	// Arrange
	TypeList params1;
	params1.push_back(std::make_shared<Typename>(u8"i32"));
	params1.push_back(std::make_shared<Typename>(u8"f32"));
	params1.push_back(std::make_shared<Typename>(u8"char"));

	auto retType1 = std::make_shared<Typename>(u8"bool");
	auto funcType1 = std::make_shared<FunctionType>(std::move(params1), std::move(retType1));

	TypeList params2;
	params2.push_back(std::make_shared<Typename>(u8"char"));
	params2.push_back(std::make_shared<Typename>(u8"i32"));
	params2.push_back(std::make_shared<Typename>(u8"f32"));

	auto retType2 = std::make_shared<Typename>(u8"bool");
	auto funcType2 = std::make_shared<FunctionType>(std::move(params2), std::move(retType2));

	// Act
	bool equal = *funcType1 == *funcType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("FunctionType: equal parameters and different return types means not equal") {
	// Arrange
	TypeList params1;
	params1.push_back(std::make_shared<Typename>(u8"i32"));
	params1.push_back(std::make_shared<Typename>(u8"char"));
	params1.push_back(std::make_shared<Typename>(u8"f32"));

	auto retType1 = std::make_shared<Typename>(u8"bool");
	auto funcType1 = std::make_shared<FunctionType>(std::move(params1), std::move(retType1));

	TypeList params2;
	params2.push_back(std::make_shared<Typename>(u8"i32"));
	params2.push_back(std::make_shared<Typename>(u8"char"));
	params2.push_back(std::make_shared<Typename>(u8"i32"));

	auto retType2 = std::make_shared<Typename>(u8"i32");
	auto funcType2 = std::make_shared<FunctionType>(std::move(params2), std::move(retType2));

	// Act
	bool equal = *funcType1 == *funcType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("FunctionType: different parameters and different return types means not equal") {
	// Arrange
	TypeList params1;
	params1.push_back(std::make_shared<Typename>(u8"i32"));
	params1.push_back(std::make_shared<Typename>(u8"char"));
	params1.push_back(std::make_shared<Typename>(u8"f32"));

	auto retType1 = std::make_shared<Typename>(u8"bool");
	auto funcType1 = std::make_shared<FunctionType>(std::move(params1), std::move(retType1));

	TypeList params2;
	params2.push_back(std::make_shared<Typename>(u8"f32"));
	params2.push_back(std::make_shared<Typename>(u8"i32"));
	params2.push_back(std::make_shared<Typename>(u8"char"));

	auto retType2 = std::make_shared<Typename>(u8"i32");
	auto funcType2 = std::make_shared<FunctionType>(std::move(params2), std::move(retType2));

	// Act
	bool equal = *funcType1 == *funcType2;

	// Assert
	CHECK(!equal);
}

TEST_CASE("ErrorType: two different error types are equal") {
	// Arrange
	auto errorType1 = std::make_shared<ErrorType>();
	auto errorType2 = std::make_shared<ErrorType>();

	// Act
	bool equal = *errorType1 == *errorType2;

	// Assert
	CHECK(equal);
}

TEST_CASE("UnitType: two different error types are equal") {
	// Arrange
	auto unitType1 = std::make_shared<ErrorType>();
	auto unitType2 = std::make_shared<ErrorType>();

	// Act
	bool equal = *unitType1 == *unitType2;

	// Assert
	CHECK(equal);
}