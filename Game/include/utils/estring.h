#pragma once

#include "ecrc32.h"

namespace EString
{
    using StringId = uint32_t;

    constexpr StringId crc32(const char* str, size_t size, size_t idx = 0, uint32_t prev_crc = 0xFFFFFFFF)
    {
        if (size == idx)
            return prev_crc ^ 0xFFFFFFFF; //-V112
        else
            return crc32(str, size, idx + 1, (prev_crc >> 8) ^ EHash::crcTable[(prev_crc ^ str[idx]) & 0xFF]);
    }

    // Compile time stringID 
    #define STRING_ID(x) std::integral_constant<EString::StringId, EString::crc32(x, sizeof(x) - 1)>::value
    
    template<typename StringClass>
    StringId CreateStringId(const StringClass& str)
    {
        return crc32(&str[0], str.size());
    }

    template<typename StringClass>
    StringId CreateStringId(const char* ptr, size_t size)
    {
        return crc32(ptr, size);
    }

    constexpr StringId operator ""_id(const char* s, size_t size)
    {
        return crc32(s, size);
    }

}

