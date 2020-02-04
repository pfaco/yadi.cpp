#pragma once

#include <yadi/data_access_result.h>
#include <cstdint>
#include <vector>

namespace dlms
{

struct RawResponseBody {
    std::vector<uint8_t> value;
};

template<typename Body = RawResponseBody>
class CosemResponse
{
    DataAccessResult result;
    Body body;
};

}
