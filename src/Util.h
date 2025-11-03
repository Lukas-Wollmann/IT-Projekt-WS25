#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <format>
#include "core/U8String.h"


template<typename T>
concept Printable = requires(std::ostream &os, const T &t)
{
    { os << t } -> std::same_as<std::ostream&>;
};

template<Printable T>
std::ostream &operator<<(std::ostream &os, const std::vector<T> &vec) 
{
    os << "{ ";

    for (size_t i = 0; i < vec.size(); ++i)
    {
        if (i > 0) os << ", ";
        
        os << vec[i];
    }

    return os << " }";    
}

std::ostream &operator<<(std::ostream &os, const char8_t *str) 
{
    while(*str)
    {
        os.put(static_cast<char>(*str));
        
        ++str;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, char32_t c) 
{
    auto bytes = U8String::encodeCodepoint(c);

    for (size_t i = 0; i < 4; ++i)
    {
        if (!bytes[i]) return os;

        os.put(bytes[i]);
    }

    return os;
}