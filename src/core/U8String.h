#pragma once
#include <iostream>
#include <array>
#include <sstream>
#include "Typedef.h"


struct U8StringIterator;


struct U8String
{
private:
    std::u8string m_Data;

public:
    U8String() {}
    U8String(char32_t data);
    U8String(const char8_t *data);
    U8String(const std::u8string &data);

    explicit U8String(const std::string &data);

    char32_t operator[](size_t idx) const;
    U8String &operator+=(const U8String &str);

    size_t length() const;
    const char8_t *data() const;
    
    U8StringIterator begin() const;
    U8StringIterator end() const;
    
    friend U8String operator+(const U8String &left, const U8String &right);
    
    static size_t getCodepointSize(char8_t str);
    static char32_t decodeCodepoint(const char8_t *str);
    static std::array<char8_t, 4> encodeCodepoint(char32_t c);
};


struct U8StringIterator 
{
private:
    const char8_t *m_Ptr, *m_End;

public:
    using iterator_category = std::forward_iterator_tag;
    using value_type        = char32_t;
    using difference_type   = std::ptrdiff_t;
    using pointer           = void;
    using reference         = char32_t;

    U8StringIterator(const char8_t *ptr, const char8_t *end);

    char32_t operator*() const;

    U8StringIterator &operator++();
    U8StringIterator operator++(int);

    bool operator==(const U8StringIterator &other) const;
    bool operator!=(const U8StringIterator &other) const;
};