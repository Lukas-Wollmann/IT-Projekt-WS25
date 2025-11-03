#include <iostream>
#include <core/U8String.h>

int main() 
{
    U8String str = u8"Hallo 🌺 123ÖÄ?";
    
    std::cout << str.length() << std::endl;
    
    return 0;
}
