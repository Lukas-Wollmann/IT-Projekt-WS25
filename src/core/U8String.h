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
    
    // Note: const char * literals do not guarantee utf-8 encoding
    // you should always use c++20 u8 string literals to do so.
    explicit U8String(const char *str);
    explicit U8String(const std::string &str);

    const char8_t *ptr() const;
    const std::u8string &data() const;
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