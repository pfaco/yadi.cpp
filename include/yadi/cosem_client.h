#pragma once

#include <yadi/cosem_serializer.h>
#include <yadi/cosem_parser.h>
#include <yadi/cosem_settings.h>

namespace dlms
{

class CosemClient
{
public:
    CosemClient() = default;
    explicit CosemClient(const CosemSettings &settings) : settings_{settings} {}

private:
    CosemSettings settings_;
};

}
