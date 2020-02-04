#pragma once

#include <yadi/cosem_attribute_descriptor.h>

namespace dlms
{

struct NullAccessSelection {};
struct NullRequestBody {};

template<typename AccessSelection = NullAccessSelection>
class CosemGetRequest
{
    CosemAttributeDescriptor descriptor;
    AccessSelection access_selection;
};

template<typename Body = NullRequestBody, typename AccessSelection = NullAccessSelection>
class CosemSetRequest
{
    CosemAttributeDescriptor descriptor;
    AccessSelection access_selection;
    Body body;
};

template<typename InvocationParameters = NullRequestBody>
class CosemActionRequest
{
    CosemAttributeDescriptor descriptor;
    InvocationParameters invocation_parameters;
};

}
