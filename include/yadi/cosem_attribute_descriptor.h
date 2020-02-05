#pragma once

#include <cstdint>
#include <yadi/cosem_logical_name.h>

namespace dlms
{

struct CosemAttributeDescriptor
{
    uint16_t class_id;
    LogicalName logical_name;
    int8_t index;
};

}
