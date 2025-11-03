#pragma once
#include <iostream>
#include "Typedef.h"


struct U8String
{
private:
    std::u8string m_Data;

public:
    U8String() {}
    U8String(const char32_t data) : m_Data(1, data) {}
    U8String(const char8_t *data) : m_Data(data) {}

    char8_t &operator[](size_t idx);
    char8_t operator[](size_t idx) const;

    size_t length() const;

    const char8_t *data() const { return m_Data.data(); }
};