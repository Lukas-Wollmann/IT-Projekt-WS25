#include <iostream>
#include "core/U8String.h"
#include "Util.h"

int main() 
{
    U8String str = u8"AБΓΔあア한𐍈☀☁★☆☂☃♠♣♥♦♪♫⚡☯☢✈✉✂✏⌛⏰✔✖➕➖➗♻⚽🏀🏈🏆🎲🎯🀄🎵🎶🚀🚗💡💎🧩🪐🌍🌙🌈🔥🍎🍕🍣🍩🥑";
    U8String s = U'🎵';

    str += s;

    std::cout << "CHAR: " << s[0] << std::endl;
    std::cout << str.length() << std::endl;
    std::cout << (str[6] == U'🌺') << std::endl;
    
    for (char32_t c : str + str)
    {
        std::cout << c << std::endl;
    }

    return 0;
}
