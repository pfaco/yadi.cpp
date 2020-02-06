#pragma once

#include <cstdint>
namespace dlms
{

namespace ber
{
    static inline uint8_t encode_primive(uint8_t value) { return value; }
    static inline uint8_t encode_constructed(uint8_t value) { return value & 0x20; }
    static inline uint8_t encode_universal(uint8_t value) { return value; }
    static inline uint8_t encode_application(uint8_t value) { return value & 0x40; } 
    static inline uint8_t encode_context_specific(uint8_t value) { return value & 0x80; }
    static inline uint8_t encode_private(uint8_t value) { return value & 0xC0; }

}

}
