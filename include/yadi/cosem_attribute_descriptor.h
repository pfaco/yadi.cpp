#pragma once

#include <cstdint>
#include <array>

namespace dlms
{

struct CosemAttributeDescriptor
{
    uint16_t class_id;
    std::array<uint8_t, 6> logical_name;
    int8_t index;
};

}
