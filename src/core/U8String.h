#pragma once
#include <iostream>

struct U8String
{
private:
    std::u8string m_Data;

public:
    U8String();
    U8String(const char32_t data);
    U8String(const char8_t *data);

    const char8_t *data() const { return m_Data.data(); }
};