#include <sstream>

#include "Doctest.h"
#include "core/U8String.h"

TEST_CASE("U8String: Test that operator<< works for char8_t *") {
	// Arrange
	const char8_t *str = u8"🌸🎀🍭";
	std::stringstream ss;

	// Act
	ss << str;
	std::string result = ss.str();

	// Assert
	CHECK(result == "\xF0\x9F\x8C\xB8\xF0\x9F\x8E\x80\xF0\x9F\x8D\xAD");
}

TEST_CASE("U8String: Construction via char32_t works") {
	// Arrange
	char32_t c = U'🦈';
	std::u8string expected = u8"🦈";

	// Act
	U8String result = c;

	// Assert
	CHECK(result.data() == expected);
}

TEST_CASE("U8String: Construction via char32_t throws if invalid codepoint") {
	// Arrange
	char32_t invalidChar = 0xFFFFFFFF;

	// Act & Assert
	CHECK_THROWS_AS(U8String result = invalidChar, utf8::invalid_code_point);
}

TEST_CASE("U8String: Construction via char8_t * works") {
	// Arrange
	const char8_t *str = u8"This is a 🦈 and he likes 🦋.";

	// Act
	U8String result = str;

	// Assert
	CHECK(result.data() == str);
}

TEST_CASE("U8String: Construction via char8_t * throws if nullptr") {
	// Arrange
	const char8_t *str = nullptr;

	// Act & Assert
	CHECK_THROWS_AS(U8String result = str, std::invalid_argument);
}

TEST_CASE("U8String: Construction via std::u8string & works") {
	// Arrange
	std::u8string str = u8"P.E.K.K.A. likes 🥞 and 🦋.";

	// Act
	U8String result = str;

	// Assert
	CHECK(result.data() == str);
}

TEST_CASE("U8String: Construction via std::u8string && works") {
	// Arrange
	std::u8string str = u8"🐸 Quaaackkk 🐸";
	std::u8string expected = str;

	// Act
	U8String result = std::move(str);

	// Assert
	CHECK(result.data() == expected);
}

TEST_CASE("U8String: Construction via char * works") {
	// Arrange
	const char *str = "❌ This does not guarantee utf-8! ❌";

	// Act
	U8String result(str);

	// Assert
	// Note: This reinterpret_cast is theoretically undefined behavior
	// as it violates the strict aliasing rule. Its okay for tests,
	// dont use something like that in normal code, its dangerous.
	CHECK(result.data() == reinterpret_cast<const char8_t *>(str));
}

TEST_CASE("U8String: Construction via char * throws if nullptr") {
	// Arrange
	const char *str = nullptr;

	// Act & Assert
	CHECK_THROWS_AS(U8String result(str), std::invalid_argument);
}

TEST_CASE("U8String: Construction via std::string & works") {
	// Arrange
	std::string str = "❌ This does also not guarantee utf-8! ❌";

	// Act
	U8String result(str);

	// Assert
	// Same here, look at the comment in the test above...
	CHECK(result.data() == reinterpret_cast<const char8_t *>(str.data()));
}

TEST_CASE("U8String: validateUTF8 throws if invalid utf8 sequence") {
	// Arrange
	std::u8string corruptedStr = u8"This is a corrupted string.";
	corruptedStr[4] = 0xFF;

	// Act & Assert
	CHECK_THROWS_AS(U8String result = corruptedStr, std::runtime_error);
}

TEST_CASE("U8String: ptr returns u8string internal buffer pointer") {
	// Arrange
    const char8_t *data = u8"The bunny likes 🌱.";
	U8String str = data;

	// Act
	const char8_t *ptr = str.ptr();

	// Assert
	CHECK(std::u8string_view(ptr) == u8"The bunny likes 🌱.");
}

TEST_CASE("U8String: data returns underlying u8string as reference") {
	// Arrange
	std::u8string buffer = u8"🌨️ and ❄️ makes a ☃️.";
	const char8_t *expected = buffer.data();
	U8String str = std::move(buffer);

	// Act
	const std::u8string &data = str.data();

	// Assert
	CHECK(data.data() == expected);
}

TEST_CASE("U8String: length returns the amount of utf-8 codepoints") {
	// Arrange
	U8String str = u8"🐝 “Bee happy!” 🍯";

	// Act
	size_t len = str.length();

	// Assert
	CHECK(len == 16);
}

TEST_CASE("U8String: begin and end both return a correct ConstIterator") {
	// Arrange
	U8String str = u8"The 🐳 likes water.";

	// Act
	size_t len = 0;
	for (char32_t _ : str)
		++len;

	// Assert
	CHECK(len == 18);
}

TEST_CASE("U8String: operator[] returns the correct codepoint") {
	// Arrange
	U8String str = u8"∑ ≔ 🔥✨🦊";

	// Act
	char32_t c = str[5];

	// Assert
	CHECK(c == U'✨');
}

TEST_CASE("U8String: operator[] throws if index out of range") {
	// Arrange
	U8String str = u8"🐸🌱🐸";

	// Act & Assert
	CHECK_THROWS_AS(str[20], std::out_of_range);
}

TEST_CASE("U8String: operator+= concatenates to the string") {
	// Arrange
	U8String str = u8"🦈 likes 🎨, ⭐️";
	U8String concat = u8" and 🎧.";

	// Act
	str += concat;

	// Assert
	CHECK(str.data() == u8"🦈 likes 🎨, ⭐️ and 🎧.");
}

TEST_CASE("U8String: operator+ concatenates into a new U8String object") {
	// Arrange
	U8String str = u8"🐸 <";
	U8String concat = u8"3 🐸";

	// Act
	U8String result = str + concat;

	// Assert
	CHECK(str.data() == u8"🐸 <");
	CHECK(concat.data() == u8"3 🐸");
	CHECK(result.data() == u8"🐸 <3 🐸");
}