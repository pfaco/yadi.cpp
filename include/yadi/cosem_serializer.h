#pragma once

#include <vector>
#include <string>
#include <yadi/cosem_invoke_id_and_priority.h>
#include <yadi/cosem_attribute_descriptor.h>
#include <yadi/cosem_request.h>
#include <yadi/cosem_aarq.h>

namespace dlms
{
    
class CosemSerializer
{
public:
    explicit CosemSerializer(std::vector<uint8_t> &buffer);
    virtual ~CosemSerializer();

    void int8(int8_t value);
    void int16(int16_t value);
    void int32(int32_t value);
    void int64(int64_t value);

    void uint8(uint8_t value);
    void uint16(uint16_t value);
    void uint32(uint32_t value);
    void uint64(uint64_t value);

    void float32(float value);
    void float64(double value);

    void visible_string(const std::string &value);
    void octet_string(const std::vector<uint8_t> &value);
    void bit_string(const std::vector<bool> &value);

    void array_header(size_t size);
    void struct_header(size_t size);

    void boolean(bool value);

    void enumeration(uint8_t value);

    void nulldata();
    void optional();
    void invoke_id_and_priority(InvokeIdAndPriority &InvokeIdAndPriority);
    void attribute_descriptor(const CosemAttributeDescriptor &att);
    void request(RequestTag request_tag, RequestType request_type);

    void aarq(const ApplicationAssociationRequest &aarq);

private:
    struct impl;
    std::unique_ptr<impl> impl_;
};

static inline void serialize(CosemSerializer &serializer, int8_t value) {serializer.int8(value);}
static inline void serialize(CosemSerializer &serializer, int16_t value) {serializer.int16(value);}
static inline void serialize(CosemSerializer &serializer, int32_t value) {serializer.int32(value);}
static inline void serialize(CosemSerializer &serializer, int64_t value) {serializer.int64(value);}
static inline void serialize(CosemSerializer &serializer, uint8_t value) {serializer.uint8(value);}
static inline void serialize(CosemSerializer &serializer, uint16_t value) {serializer.uint16(value);}
static inline void serialize(CosemSerializer &serializer, uint32_t value) {serializer.uint32(value);}
static inline void serialize(CosemSerializer &serializer, uint64_t value) {serializer.uint64(value);}
static inline void serialize(CosemSerializer &serializer, float value) {serializer.float32(value);}
static inline void serialize(CosemSerializer &serializer, double value) {serializer.float64(value);}
static inline void serialize(CosemSerializer &serializer, bool value) {serializer.boolean(value);}
static inline void serialize(CosemSerializer &serializer, const std::string &value) {serializer.visible_string(value);}
static inline void serialize(CosemSerializer &serializer, const std::vector<uint8_t> &value) {serializer.octet_string(value);}
static inline void serialize(CosemSerializer &serializer, const std::vector<bool> &value) {serializer.bit_string(value);}

template<typename T>
static inline void serialize_optional(CosemSerializer &serializer, T &value) {
    serializer.optional();
    ::dlms::serialize(serializer, value);
}

template<>
static inline void serialize_optional(CosemSerializer &serializer, NullAccessSelection &access_selection) {
    (void)access_selection;
    serializer.nulldata();
}

template<>
static inline void serialize_optional(CosemSerializer &serializer, NullRequestBody &body) {
    (void)body;
    serializer.nulldata();
}

static inline void serialize(CosemSerializer &serializer, BasicRequest &req) {
    serializer.invoke_id_and_priority(req.invoke_id_and_priority);
    serializer.attribute_descriptor(req.descriptor);
}

template<typename AccessSelection = NullAccessSelection>
void serialize(CosemSerializer &serializer, GetRequest<AccessSelection> &req) {
    serializer.request(RequestTag::GET_REQUEST, RequestType::NORMAL);
    ::dlms::serialize(serializer, req.basic);
    ::dlms::serialize_optional(serializer, req.access_selection);
}

template<typename Body = NullRequestBody, typename AccessSelection = NullAccessSelection>
void serialize(CosemSerializer &serializer, SetRequest<Body,AccessSelection> &req) {
    serializer.request(RequestTag::SET_REQUEST, RequestType::NORMAL);
    ::dlms::serialize(serializer, req.basic);
    ::dlms::serialize_optional(serializer, req.access_selection);
    ::dlms::serialize(serializer, req.body);
}

template<typename InvocationParameters = NullRequestBody>
void serialize(CosemSerializer &serializer, ActionRequest<InvocationParameters> &req) {
    serializer.request(RequestTag::ACTION_REQUEST, RequestType::NORMAL);
    ::dlms::serialize(serializer, req.basic);
    ::dlms::serialize_optional(serializer, req.invocation_parameters);
}

}
