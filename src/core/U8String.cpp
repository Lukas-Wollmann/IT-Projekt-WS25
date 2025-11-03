#include "U8String.h"


char8_t &U8String::operator[](size_t idx)
{
    (void)idx;
    return m_Data[0];
}

char8_t U8String::operator[](size_t idx) const
{
    (void)idx;
    return U'?';
}

size_t U8String::length() const
{
    size_t len = 0;

    for (size_t i = 0; i < m_Data.size(); ) 
    {
        char8_t c = m_Data[i];

        if      ( c          < 0x80) i += 1;  // ASCII
        else if ((c & 0xE0) == 0xC0) i += 2;  // 2-byte UTF-8
        else if ((c & 0xF0) == 0xE0) i += 3;  // 3-byte UTF-8
        else if ((c & 0xF8) == 0xF0) i += 4;  // 4-byte UTF-8
        else throw std::runtime_error("Encountered invalid UTF-8 sequence!");

        ++len;
    }

    return len;
}