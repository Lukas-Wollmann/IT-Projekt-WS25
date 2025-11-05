#pragma once
#include <iostream>
#include <string>
#include <utf8cpp/utf8.h>
#include "Typedef.h"

std::ostream &operator<<(std::ostream &os, char32_t c);
std::ostream &operator<<(std::ostream &os, const char8_t *str);


struct U8String
{
    using ConstIterator = utf8::iterator<std::u8string::const_iterator>;

public:
    std::u8string m_Data;

public:
    U8String() = default;
    U8String(char32_t c);
    U8String(const char8_t *str);
    U8String(const std::u8string &str);
    U8String(std::u8string &&str);

    U8String(const U8String &) = default;
    U8String(U8String &&) noexcept = default;

    // Normal string literals do not guarantee UTF8 encoding,
    // therefore enforce the usage of u8 string literals.
    U8String(const std::string &) = delete;
    U8String(const char *) = delete;

    size_t length() const;
    ConstIterator begin() const;
    ConstIterator end() const;

    char32_t operator[](size_t idx) const;
    U8String &operator+=(const U8String &str);

    friend U8String operator+(const U8String &left, const U8String &right);
    friend std::ostream &operator<<(std::ostream &os, const U8String &str);

private:
    void validateUTF8();
};