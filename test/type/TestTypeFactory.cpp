#include "Doctest.h"
#include "type/TypeFactory.h"

TEST_CASE("Primitive interning") {
	TypeFactory::reset();

	Type t1 = TypeFactory::getI32();
	Type t2 = TypeFactory::getI32();
	Type t3 = TypeFactory::getChar();

	CHECK(t1 == t2); // Must be same pointer
	CHECK(t1 != t3); // Different primitives must be different pointers
	CHECK(t1->str() == u8"i32");
}

TEST_CASE("Pointer interning") {
	TypeFactory::reset();

	Type i32_1 = TypeFactory::getI32();
	Type i32_2 = TypeFactory::getI32();

	Type ptr1 = TypeFactory::getPointer(i32_1);
	Type ptr2 = TypeFactory::getPointer(i32_2);
	Type ptrToChar = TypeFactory::getPointer(TypeFactory::getChar());

	CHECK(ptr1 == ptr2);	  // Pointer to same type should be interned
	CHECK(ptr1 != ptrToChar); // Pointer to different types should be different
	CHECK(ptr1->str() == u8"*i32");
}

TEST_CASE("Function interning") {
	TypeFactory::reset();

	Type i32 = TypeFactory::getI32();
	Type boolean = TypeFactory::getBool();

	// Create (i32, bool) -> i32
	Type fn1 = TypeFactory::getFunction({i32, boolean}, i32);
	Type fn2 = TypeFactory::getFunction({i32, boolean}, i32);

	// Create (bool) -> i32
	Type fn3 = TypeFactory::getFunction({boolean}, i32);

	CHECK(fn1 == fn2);
	CHECK(fn1 != fn3);
}

TEST_CASE("Struct interning (Nominal)") {
	TypeFactory::reset();

	// Structs are currently interned by name
	Type s1 = TypeFactory::getStruct(u8"Player");
	Type s2 = TypeFactory::getStruct(u8"Player");
	Type s3 = TypeFactory::getStruct(u8"Enemy");

	CHECK(s1 == s2);
	CHECK(s1 != s3);
}

TEST_CASE("Registry integrity") {
	TypeFactory::reset();

	// Add a specific type and ensure it is the only thing there initially
	TypeFactory::getI32();

	auto all = TypeFactory::allTypes();
	CHECK(all.size() == 1);

	bool foundI32 = false;
	for (auto t : all) {
		if (t->str() == u8"i32")
			foundI32 = true;
	}
	CHECK(foundI32);
}

TEST_CASE("Reset invalidates pointers") {
	TypeFactory::reset();
	TypeFactory::getI32();
	TypeFactory::getChar();

	CHECK(TypeFactory::allTypes().size() == 2);

	TypeFactory::reset(); // Clear memory

	CHECK(TypeFactory::allTypes().size() == 0);

	Type newI32 = TypeFactory::getI32();
	CHECK(TypeFactory::allTypes().size() == 1);
}