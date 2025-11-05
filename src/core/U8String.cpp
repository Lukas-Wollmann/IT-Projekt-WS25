#include "U8String.h"


std::ostream &operator<<(std::ostream &os, char32_t c)
{
    std::string bytes;
    utf8::append(c, std::back_inserter(bytes));

    os << bytes;

    return os;
}

std::ostream &operator<<(std::ostream &os, const char8_t *str)
{
    while (*str != '\0')
    {
        os.put(static_cast<char>(*str));
        ++str;
    }

    return os;
}

U8String::U8String(char32_t c) 
{
    utf8::utf32to8(&c, &c + 1, std::back_inserter(m_Data));
}

U8String::U8String(const char8_t *str) 
    : m_Data(str) 
{
    validateUTF8();
}

U8String::U8String(const std::u8string &str) 
    : m_Data(str) 
{
    validateUTF8();
}

U8String::U8String(std::u8string &&str) : m_Data(std::move(str)) 
{
    validateUTF8();
}

size_t U8String::length() const
{
    return utf8::distance(m_Data.begin(), m_Data.end());
}

U8String::ConstIterator U8String::begin() const
{
    return U8String::ConstIterator(m_Data.begin(), m_Data.begin(), m_Data.end());
}

U8String::ConstIterator U8String::end() const
{
    return U8String::ConstIterator(m_Data.end(), m_Data.begin(), m_Data.end());
}

U8String operator+(const U8String &left, const U8String &right)
{
    U8String result;
    result.m_Data = left.m_Data + right.m_Data;

    return result;
}

U8String &U8String::operator+=(const U8String &str)
{
    m_Data.append(str.m_Data);

    return *this;
}

char32_t U8String::operator[](size_t idx) const
{
    auto it = m_Data.begin();
    auto end = m_Data.end();
    size_t i = 0;

    while (it != m_Data.end()) 
    {
        char32_t codepoint = utf8::next(it, end);

        if (i == idx) return codepoint;
        
        ++i;
    }

    throw std::out_of_range("Index out of range");
}

std::ostream &operator<<(std::ostream &os, const U8String &str)
{
    return os << str.m_Data.data();
}

void U8String::validateUTF8()
{
    auto it = utf8::find_invalid(m_Data.begin(), m_Data.end());

    if (it != m_Data.end()) 
        throw std::runtime_error("Invalid UTF-8 sequence");
}