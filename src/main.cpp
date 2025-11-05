#include "core/U8String.h"
#include <utf8cpp/utf8.h>

int main() {

    try 
    {
        U8String test1 = u8"🥝123", test2 = u8"🥝test";
        auto b = test1 + test2;
        
        for(auto it = b.begin(); it != b.end(); ++it)
        {
            std::cout << *it << "   ";
        }

    } 
    catch(utf8::exception &p) 
    {
        std::cout << p.what();
    }

    return 0;
}