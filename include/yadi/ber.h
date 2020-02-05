#pragma once

#include <cstdint>
namespace dlms
{

namespace ber
{

    static inline uint8_t application(uint8_t value) { return value & 0x60; } 
    static inline uint8_t context_specific(uint8_t value) { return value & 0xA0; }
    
}

}
