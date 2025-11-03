#include "U8String.h"

U8String::U8String(char32_t codepoint) 
    : m_Data(encodeCodepoint(codepoint).data(), 4) 
{}

U8String::U8String(const char8_t *data) 
    : m_Data(data) 
{}

U8String::U8String(const std::u8string &data) 
    : m_Data(data)
{}

U8String::U8String(const std::string& data) 
    : m_Data(data.begin(), data.end()) 
{}

char32_t U8String::operator[](size_t idx) const
{
    size_t len = 0, dataLen = m_Data.size();

    for (size_t i = 0; i < dataLen; ) 
    {
        if (len == idx)
        {
            if (i < dataLen) return decodeCodepoint(&m_Data[i]);
            else throw std::out_of_range("Not enough bytes available for UTF-8 sequence");
        }

        i += getCodepointSize(m_Data[i]);
        ++len;
    }

    throw std::out_of_range("Index out of string!");
}


U8String &U8String::operator+=(const U8String &str)
{
    m_Data += str.m_Data;

    return *this;
}

size_t U8String::length() const
{
    size_t len = 0;

    for (size_t i = 0; i < m_Data.size(); ) 
    {
        i += getCodepointSize(m_Data[i]);
        ++len;
    }

    return len;
}

const char8_t *U8String::data() const
{ 
    return m_Data.data(); 
}

U8StringIterator U8String::begin() const 
{
    return U8StringIterator(m_Data.data(), m_Data.data() + m_Data.size());
}

U8StringIterator U8String::end() const 
{
    return U8StringIterator(m_Data.data() + m_Data.size(), m_Data.data() + m_Data.size());
}

U8String operator+(const U8String &left, const U8String &right)
{
    return left.m_Data + right.m_Data;
}

size_t U8String::getCodepointSize(char8_t c)
{
    if      ( c          < 0x80) return 1;  // ASCII
    else if ((c & 0xE0) == 0xC0) return 2;  // 2-byte UTF-8
    else if ((c & 0xF0) == 0xE0) return 3;  // 3-byte UTF-8
    else if ((c & 0xF8) == 0xF0) return 4;  // 4-byte UTF-8
    else throw std::runtime_error("Encountered invalid UTF-8 sequence!");
}

char32_t U8String::decodeCodepoint(const char8_t *str)
{
    char8_t c = str[0];

    // 1-byte ASCII
    if (c < 0x80) return c;
        
    // 2-byte: 110xxxxx 10xxxxxx
    if ((c & 0xE0) == 0xC0)
    {                          
        char32_t codepoint = c & 0x1F;
        codepoint = (codepoint << 6) | (str[1] & 0x3F);
        
        return codepoint;
    }

    // 3-byte: 1110xxxx 10xxxxxx 10xxxxxx
    if ((c & 0xF0) == 0xE0)
    {                          
        char32_t codepoint = c & 0x0F;
        codepoint = (codepoint << 6) | (str[1] & 0x3F);
        codepoint = (codepoint << 6) | (str[2] & 0x3F);

        return codepoint;
    }

    // 4-byte: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
    if ((c & 0xF8) == 0xF0)
    {                          
        char32_t codepoint = (c & 0x07);
        codepoint = (codepoint << 6) | (str[1] & 0x3F);
        codepoint = (codepoint << 6) | (str[2] & 0x3F);
        codepoint = (codepoint << 6) | (str[3] & 0x3F);

        return codepoint;
    }

    throw std::runtime_error("Encountered Invalid UTF-8 leading byte while decoding!");
}

std::array<char8_t, 4> U8String::encodeCodepoint(char32_t c)
{
    // 1-byte ASCII
    if (c <= 0x7F) return { static_cast<char8_t>(c), 0, 0, 0 };
    
    // 2-byte
    if (c <= 0x7FF) return 
    {
        static_cast<char8_t>(0xC0 | ((c >> 6) & 0x1F)), 
        static_cast<char8_t>(0x80 | ( c       & 0x3F)), 
        0, 0 
    };

    // 3-byte
    if (c <= 0xFFFF) return 
    {
        static_cast<char8_t>(0xE0 | ((c >> 12) & 0x0F)),
        static_cast<char8_t>(0x80 | ((c >>  6) & 0x3F)),
        static_cast<char8_t>(0x80 | ( c        & 0x3F)),
        0
    };

    // 4-byte
    if (c <= 0x10FFFF) return
    {
        static_cast<char8_t>(0xF0 | ((c >> 18) & 0x07)),
        static_cast<char8_t>(0x80 | ((c >> 12) & 0x3F)),
        static_cast<char8_t>(0x80 | ((c >>  6) & 0x3F)),
        static_cast<char8_t>(0x80 | ( c        & 0x3F))
    };
    
    throw std::runtime_error("Encountered invalid unicode codepoint while decoding!");
}

U8StringIterator::U8StringIterator(const char8_t *ptr, const char8_t *end) 
    : m_Ptr(ptr), m_End(end) 
{}

char32_t U8StringIterator::operator*() const 
{
    if (m_Ptr >= m_End)
        throw std::out_of_range("Dereferencing end iterator");
    
    return U8String::decodeCodepoint(m_Ptr); 
}

U8StringIterator &U8StringIterator::operator++() 
{
    if (m_Ptr < m_End)      
        m_Ptr += U8String::getCodepointSize(*m_Ptr);
    
    return *this;
}

U8StringIterator U8StringIterator::operator++(int) 
{
    U8StringIterator tmp = *this;
    ++(*this);

    return tmp;
}

bool U8StringIterator::operator==(const U8StringIterator &other) const 
{
    return m_Ptr == other.m_Ptr;
}

bool U8StringIterator::operator!=(const U8StringIterator &other) const 
{
    return m_Ptr != other.m_Ptr;
}