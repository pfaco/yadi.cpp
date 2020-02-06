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
    GetResponse<ResponseBody> get_normal(Serial& serial, const CosemAttributeDescriptor &att, const AccessSelector& access_selector = {}) {
        reset();
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

    template<typename Body, typename Serial, typename AccessSelector = NullAccessSelection>
    SetResponse set_normal(Serial& serial, const CosemAttributeDescriptor &att, const Body &data, const AccessSelector& access_selector = {}) {
        reset();
        CosemSerializer serializer(buffer_tx);
        CosemParser parser(buffer_rx);
        SetRequest<Body,AccessSelector> req{{invoke_and_and_priority, att}, access_selector, data};
        SetResponse resp;
        ::dlms::serialize(serializer, req);
        serial.write(buffer_tx);
        serial.read(buffer_rx);
        ::dlms::parse(parser, resp);
        return std::move(resp);
    }

    template<typename Body, typename Serial, typename InvocationParameters = NullAccessSelection>
    ActionResponse<Body> action_normal(Serial& serial, const CosemAttributeDescriptor &att, const InvocationParameters& invocation_parameters = {}) {
        reset();
        CosemSerializer serializer(buffer_tx);
        CosemParser parser(buffer_rx);
        ActionRequest<InvocationParameters> req{{invoke_and_and_priority, att}, invocation_parameters};
        ActionResponse<Body> resp;
        ::dlms::serialize(serializer, req);
        serial.write(buffer_tx);
        serial.read(buffer_rx);
        ::dlms::parse(parser, resp);
        return std::move(resp);
    }

private:
    void reset() {
        buffer_tx.clear();
        buffer_rx.clear();
        invoke_and_and_priority = InvokeIdAndPriority{static_cast<uint8_t>(invoke_and_and_priority.invoke_id() + 1), invoke_and_and_priority.service_class(), invoke_and_and_priority.priority() };
    }

    CosemSettings settings_;
    InvokeIdAndPriority invoke_and_and_priority;
    std::vector<uint8_t> buffer_tx;
    std::vector<uint8_t> buffer_rx;
};

}
