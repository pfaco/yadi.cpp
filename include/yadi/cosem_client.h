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

    template<typename ResponseBody, typename Serial, typename AccessSelector = NullAccessSelection>
    GetResponse<ResponseBody> get(Serial& serial, const CosemAttributeDescriptor &att, const AccessSelector& access_selector = {}) {
        buffer_tx.clear();
        buffer_rx.clear();
        CosemSerializer serializer(buffer_tx);
        CosemParser parser(buffer_rx);
        GetRequest<AccessSelector> req{{invoke_and_and_priority, att}, access_selector};
        GetResponse<ResponseBody> resp;

        ::dlms::serialize(serializer, req);
        serial.write(buffer_tx);
        serial.read(buffer_rx);
        ::dlms::parse(parser, resp);
        return std::move(resp);
    }

private:
    CosemSettings settings_;
    InvokeIdAndPriority invoke_and_and_priority;
    std::vector<uint8_t> buffer_tx;
    std::vector<uint8_t> buffer_rx;
};

}
