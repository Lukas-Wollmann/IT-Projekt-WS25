#include "Doctest.h"
#include "type/CloneVisitor.h"
#include "type/CompareVisitor.h"

using namespace type;

TEST_CASE("Typename: deep clone works") {
	// Arrange
	auto type = std::make_unique<Typename>(u8"i32");

	// Act
	auto copy = clone(*type);

	// Assert
	CHECK(copy.get() != type.get());
	CHECK(copy->kind == TypeKind::Typename);
	CHECK(static_cast<const Typename &>(*copy).typename_ == type->typename_);
}

TEST_CASE("PointerType: deep clone works") {
	// Arrange
	auto type = std::make_unique<Typename>(u8"i32");
	auto ptrType = std::make_unique<PointerType>(std::move(type));

	// Act
	auto copy = clone(*ptrType);

	// Assert
	CHECK(copy.get() != ptrType.get());
	CHECK(copy->kind == TypeKind::Pointer);
	CHECK(*static_cast<const PointerType &>(*copy).pointeeType == *ptrType->pointeeType);
}

TEST_CASE("ArrayType: deep clone works") {
	// Arrange
	auto type = std::make_unique<Typename>(u8"i32");
	auto arrType = std::make_unique<ArrayType>(std::move(type), 42);

	// Act
	auto copy = clone(*arrType);

	// Assert
	CHECK(copy.get() != arrType.get());
	CHECK(copy->kind == TypeKind::Array);
	CHECK(*static_cast<const ArrayType &>(*copy).elementType == *arrType->elementType);
	CHECK(static_cast<const ArrayType &>(*copy).arraySize == 42);
}

TEST_CASE("FunctionType: deep clone works") {
	Vec<Box<const Type>> params;
	params.push_back(std::make_unique<Typename>(u8"i32"));
	params.push_back(std::make_unique<Typename>(u8"f32"));
	params.push_back(std::make_unique<Typename>(u8"char"));

	auto retType = std::make_unique<Typename>(u8"bool");
	auto funcType = std::make_unique<FunctionType>(std::move(params), std::move(retType));

	// Act
	auto copy = clone(*funcType);

	// Assert
	CHECK(copy.get() != funcType.get());
	CHECK(copy->kind == TypeKind::Function);

	auto &copyFunc = static_cast<const FunctionType &>(*copy);

	CHECK(copyFunc.paramTypes.size() == funcType->paramTypes.size());

	for (size_t i = 0; i < copyFunc.paramTypes.size(); ++i)
		CHECK(*copyFunc.paramTypes[i] == *funcType->paramTypes[i]);

	CHECK(*copyFunc.returnType == *funcType->returnType);
}

TEST_CASE("ErrorType: deep clone works") {
	// Arrange
	auto errType = std::make_unique<ErrorType>();

	// Act
	auto copy = clone(*errType);

	// Assert
	CHECK(copy.get() != errType.get());
	CHECK(copy->kind == TypeKind::Error);
}

TEST_CASE("UnitType: deep clone works") {
	// Arrange
	auto unitType = std::make_unique<UnitType>();

	// Act
	auto copy = clone(*unitType);

	// Assert
	CHECK(copy.get() != unitType.get());
	CHECK(copy->kind == TypeKind::Unit);
}