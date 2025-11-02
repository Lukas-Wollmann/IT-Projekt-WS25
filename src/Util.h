#pragma once
#include <iostream>
#include <sstream>
#include <vector>
#include <format>


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