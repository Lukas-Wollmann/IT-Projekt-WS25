#pragma once
#include "Typedef.h"
#include "utf8cpp/utf8.h"


struct U8String
{
public:
    U8String() = default;
    U8String(const char *str);
    U8String(const char8_t *str);
    U8String(const std::string &str);
    U8String(std::string &&str);

    U8String(const U8String &) = default;
    U8String(U8String &&) noexcept = default;
};