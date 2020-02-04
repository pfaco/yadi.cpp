#pragma once

#include <cstdint>

namespace dlms
{

enum class DataTag : uint8_t
{
    ARRAY = 1,
    STRUCTURE = 2,
    BOOLEAN = 3,
    BITSTRING = 4,
    INT32 = 5,
    UINT32 = 6,
    OCTET_STRING = 9,
    VISIBLE_STRING = 10,
    UTF8_STRING = 12,
    BCD = 13,
    INT8 = 15,
    INT16 = 16,
    UINT8 = 17,
    UINT16 = 18,
    INT64 = 20,
    UINT64 = 21,
    ENUM = 22,
    FLOAT32 = 23,
    FLOAT64 = 24,
    DATE_TIME = 25,
    DATE = 26,
    TIME = 27,
};

}
