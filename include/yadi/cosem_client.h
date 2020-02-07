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
        ::dlms::serialize(serializer, GetRequest<AccessSelector>{{invoke_and_and_priority, att}, access_selector});
        serial.write(buffer_tx);
        serial.read(buffer_rx);
        return ::dlms::parse_get_normal<ResponseBody>(parser);
    }

    template<typename Body, typename Serial, typename AccessSelector = NullAccessSelection>
    SetResponse set_normal(Serial& serial, const CosemAttributeDescriptor &att, const Body &data, const AccessSelector& access_selector = {}) {
        reset();
        CosemSerializer serializer(buffer_tx);
        CosemParser parser(buffer_rx);
        ::dlms::serialize(serializer, SetRequest<Body,AccessSelector>{{invoke_and_and_priority, att}, access_selector, data});
        serial.write(buffer_tx);
        serial.read(buffer_rx);
        return ::dlms::parse_set_normal(parser);
    }

    template<typename Body, typename Serial, typename InvocationParameters = NullAccessSelection>
    ActionResponse<Body> action_normal(Serial& serial, const CosemAttributeDescriptor &att, const InvocationParameters& invocation_parameters = {}) {
        reset();
        CosemSerializer serializer(buffer_tx);
        CosemParser parser(buffer_rx);
        ::dlms::serialize(serializer, ActionRequest<InvocationParameters>{{invoke_and_and_priority, att}, invocation_parameters});
        serial.write(buffer_tx);
        serial.read(buffer_rx);
        return ::dlms::parse_action_normal<Body>(parser);
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
