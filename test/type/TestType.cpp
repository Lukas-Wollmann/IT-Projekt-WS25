#include "Doctest.h"
#include "type/Type.h"

TEST_CASE("PrimitiveType logic") {
	PrimitiveType t1(PrimitiveKind::I32);
	PrimitiveType t2(PrimitiveKind::I32);
	PrimitiveType t3(PrimitiveKind::Bool);

	CHECK(t1.str() == u8"i32");
	CHECK(t1.equals(&t2));
	CHECK(!t1.equals(&t3));

	auto cloned = t1.clone();
	CHECK(cloned->equals(&t1));
}

TEST_CASE("PointerType logic") {
	PrimitiveType i32(PrimitiveKind::I32);
	PrimitiveType boolean(PrimitiveKind::Bool);

	PointerType p1(&i32);
	PointerType p2(&i32);
	PointerType p3(&boolean);

	CHECK(p1.str() == u8"*i32");
	CHECK(p1.equals(&p2));
	CHECK(!p1.equals(&p3));
}

TEST_CASE("FunctionType logic") {
	PrimitiveType i32(PrimitiveKind::I32);
	PrimitiveType b(PrimitiveKind::Bool);

	// (i32, bool) -> i32
	FunctionType f1({&i32, &b}, &i32);
	FunctionType f2({&i32, &b}, &i32);

	// (i32) -> i32
	FunctionType f3({&i32}, &i32);

	// (i32, bool) -> bool
	FunctionType f4({&i32, &b}, &b);

	CHECK(f1.equals(&f2));
	CHECK(!f1.equals(&f3)); // Parameter count mismatch
	CHECK(!f1.equals(&f4)); // Return type mismatch
}

TEST_CASE("StructType logic") {
	StructType s1(u8"Node", {});
	StructType s2(u8"Node", {});
	StructType s3(u8"List", {});

	// Nominal equality (based on name)
	CHECK(s1.equals(&s2));
	CHECK(!s1.equals(&s3));
	CHECK(s1.str() == u8"Node");
}

TEST_CASE("Unit and Error types") {
	UnitType u1;
	UnitType u2;
	ErrorType e1;

	CHECK(u1.equals(&u2));
	CHECK(!u1.equals(&e1));
	CHECK(u1.str() == u8"()");
	CHECK(e1.str() == u8"<error-type>");
}

TEST_CASE("Cross-type equality") {
	PrimitiveType i32(PrimitiveKind::I32);
	PointerType ptr(&i32);

	CHECK(!i32.equals(&ptr));
	CHECK(!ptr.equals(&i32));
}