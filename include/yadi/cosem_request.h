#pragma once

#include <yadi/cosem_attribute_descriptor.h>
#include <yadi/cosem_invoke_id_and_priority.h>

namespace dlms
{

enum class RequestTag : uint8_t
{
    GET_REQUEST = 192,
    SET_REQUEST = 193,
    ACTION_REQUEST = 195,
};

enum class RequestType : uint8_t
{
    NORMAL = 1,
};

struct NullAccessSelection {};
struct NullRequestBody {};

struct BasicRequest
{
    InvokeId invoke_id;
    Priority priority;
    CosemAttributeDescriptor descriptor;
};

template<typename AccessSelection = NullAccessSelection>
struct GetRequest
{
    BasicRequest basic;
    AccessSelection access_selection;
};

template<typename Body = NullRequestBody, typename AccessSelection = NullAccessSelection>
struct SetRequest
{
    BasicRequest basic;
    AccessSelection access_selection;
    Body body;
};

template<typename InvocationParameters = NullRequestBody>
struct ActionRequest
{
    BasicRequest basic;
    InvocationParameters invocation_parameters;
};

}
