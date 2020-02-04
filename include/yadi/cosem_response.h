#pragma once

#include <yadi/cosem_invoke_id_and_priority.h>
#include <cstdint>
#include <vector>

namespace dlms
{

enum class DataAccessResult : uint8_t
{

};

struct RawResponseBody {
    std::vector<uint8_t> value;
};

template<typename Body = RawResponseBody>
struct GetResponse
{
    InvokeId invoke_id;
    Priority priority;
    DataAccessResult result;
    Body body;
};

}
