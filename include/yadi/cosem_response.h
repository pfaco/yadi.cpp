#pragma once

#include <yadi/cosem_invoke_id_and_priority.h>
#include <yadi/cosem_data_access_result.h>
#include <cstdint>
#include <vector>

namespace dlms
{

enum class ResponseTag : uint8_t
{
    GET_RESPONSE = 196,
    SET_RESPONSE = 197,
    ACTION_RESPONSE = 199,
};

enum class ResponseType : uint8_t
{
    NORMAL = 1,
};

struct RawResponseBody {
    std::vector<uint8_t> value;
};

template<typename Body = RawResponseBody>
struct GetResponse
{
    InvokeIdAndPriority invoke_id_and_priority;
    DataAccessResult result;
    Body body;
};

struct SetResponse
{
    InvokeIdAndPriority invoke_id_and_priority;
    DataAccessResult result;
};

template<typename Body = RawResponseBody>
struct ActionResponse
{
    InvokeIdAndPriority invoke_id_and_priority;
    ActionAccessResult result;
    DataAccessResult data_access_result;
    Body body;
};

}
