#include <yadi/cosem_serializer.h>
#include <yadi/cosem_data_tag.h>
#include <iostream>
#include "byte_output_stream.h"

namespace dlms
{

namespace
{
    static void write_size(ByteOutputStream &os, size_t size) {
		if (size <= 0x80) {
			os.write_u8(static_cast<uint8_t>(size));
		}
		else if (size <= 0xFF) {
			os.write_u8(0x81);
			os.write_u8(static_cast<uint8_t>(size));
		}
		else if (size <= 0xFFFF) {
			os.write_u8(0x82);
			os.write_u16(static_cast<uint16_t>(size));
		}
		else {
			os.write_u8(0x84);
			os.write_u32(static_cast<uint32_t>(size));
		}
    }
}

struct CosemSerializer::impl
{
	explicit impl(std::vector<uint8_t> &buffer) : os{buffer} {}
	ByteOutputStream os;
};

CosemSerializer::CosemSerializer(std::vector<uint8_t> &buffer) : impl_{std::make_unique<impl>(buffer)} {}
CosemSerializer::~CosemSerializer() = default;

void CosemSerializer::int8(int8_t value) {
    impl_->os.write_u8(static_cast<uint8_t>(DataTag::INT8));
	impl_->os.write_i8(value);
}

void CosemSerializer::int16(int16_t value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::INT16));
	impl_->os.write_i16(value);
}

void CosemSerializer::int32(int32_t value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::INT32));
	impl_->os.write_i32(value);
}

void CosemSerializer::int64(int64_t value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::INT64));
	impl_->os.write_i64(value);
}

void CosemSerializer::uint8(uint8_t value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::UINT8));
	impl_->os.write_u8(value);
}

void CosemSerializer::uint16(uint16_t value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::UINT16));
	impl_->os.write_u16(value);
}

void CosemSerializer::uint32(uint32_t value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::UINT32));
	impl_->os.write_u32(value);
}

void CosemSerializer::uint64(uint64_t value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::UINT64));
	impl_->os.write_u64(value);
}

void CosemSerializer::float32(float value) {
    uint32_t temp = 0;
    memcpy(&temp, &value, sizeof(float));
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::FLOAT32));
	impl_->os.write_u32(temp);
}

void CosemSerializer::float64(double value) {
    uint64_t temp = 0;
    memcpy(&temp, &value, sizeof(float));
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::FLOAT64));
	impl_->os.write_u64(temp);
}

void CosemSerializer::visible_string(const std::string &value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::VISIBLE_STRING));
    write_size(impl_->os, value.size());
	impl_->os.write_buffer(reinterpret_cast<const uint8_t*>(value.data()), value.size());
}

void CosemSerializer::octet_string(const std::vector<uint8_t> &value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::OCTET_STRING));
    write_size(impl_->os, value.size());
	impl_->os.write_buffer(reinterpret_cast<const uint8_t*>(value.data()), value.size());
}

void CosemSerializer::array_header(size_t size) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::ARRAY));
	write_size(impl_->os, size);
}

void CosemSerializer::struct_header(size_t size) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::STRUCTURE));
	write_size(impl_->os, size);
}

void CosemSerializer::boolean(bool value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::BOOLEAN));
	impl_->os.write_u8(value ? 0x01 : 0x00);
}

void CosemSerializer::enumeration(uint8_t value) {
	impl_->os.write_u8(static_cast<uint8_t>(DataTag::ENUM));
	impl_->os.write_u8(value);
}

void CosemSerializer::nulldata() {
	impl_->os.write_u8(0x00);
}

void CosemSerializer::optional() {
	impl_->os.write_u8(0x01);
}

void CosemSerializer::invoke_id_and_priority(InvokeIdAndPriority &InvokeIdAndPriority) {
	impl_->os.write_u8(InvokeIdAndPriority.value());
}

void CosemSerializer::attribute_descriptor(const CosemAttributeDescriptor &att) {
	impl_->os.write_u16(att.class_id);
	impl_->os.write_buffer(att.logical_name.value().data(), att.logical_name.value().size());
	impl_->os.write_i8(att.index);
}

void CosemSerializer::request(RequestTag request_tag, RequestType request_type) {
	impl_->os.write_u8(static_cast<uint8_t>(request_tag));
	impl_->os.write_u8(static_cast<uint8_t>(request_type));
}

}
