#include <yadi/cosem_serializer.h>
#include <yadi/cosem_data_tag.h>
#include <iostream>
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

void CosemSerializer::int8(int8_t value) {
    os.write_u8(static_cast<uint8_t>(DataTag::INT8));
	os.write_i8(value);
}

void CosemSerializer::int16(int16_t value) {
	os.write_u8(static_cast<uint8_t>(DataTag::INT16));
	os.write_i16(value);
}

void CosemSerializer::int32(int32_t value) {
	os.write_u8(static_cast<uint8_t>(DataTag::INT32));
	os.write_i32(value);
}

void CosemSerializer::int64(int64_t value) {
	os.write_u8(static_cast<uint8_t>(DataTag::INT64));
	os.write_i64(value);
}

void CosemSerializer::uint8(uint8_t value) {
	os.write_u8(static_cast<uint8_t>(DataTag::UINT8));
	os.write_u8(value);
}

void CosemSerializer::uint16(uint16_t value) {
	os.write_u8(static_cast<uint8_t>(DataTag::UINT16));
	os.write_u16(value);
}

void CosemSerializer::uint32(uint32_t value) {
	os.write_u8(static_cast<uint8_t>(DataTag::UINT32));
	os.write_u32(value);
}

void CosemSerializer::uint64(uint64_t value) {
	os.write_u8(static_cast<uint8_t>(DataTag::UINT64));
	os.write_u64(value);
}

void CosemSerializer::float32(float value) {
    uint32_t temp = 0;
    memcpy(&temp, &value, sizeof(float));
	os.write_u8(static_cast<uint8_t>(DataTag::FLOAT32));
	os.write_u32(temp);
}

void CosemSerializer::float64(double value) {
    uint64_t temp = 0;
    memcpy(&temp, &value, sizeof(float));
	os.write_u8(static_cast<uint8_t>(DataTag::FLOAT64));
	os.write_u64(temp);
}

void CosemSerializer::visible_string(const std::string &value) {
	os.write_u8(static_cast<uint8_t>(DataTag::VISIBLE_STRING));
    write_size(os, value.size());
	os.write_buffer(reinterpret_cast<const uint8_t*>(value.data()), value.size());
}

void CosemSerializer::octet_string(const std::vector<uint8_t> &value) {
	os.write_u8(static_cast<uint8_t>(DataTag::OCTET_STRING));
    write_size(os, value.size());
	os.write_buffer(reinterpret_cast<const uint8_t*>(value.data()), value.size());
}

void CosemSerializer::array_header(size_t size) {
	os.write_u8(static_cast<uint8_t>(DataTag::ARRAY));
	write_size(os, size);
}

void CosemSerializer::struct_header(size_t size) {
	os.write_u8(static_cast<uint8_t>(DataTag::STRUCTURE));
	write_size(os, size);
}

void CosemSerializer::boolean(bool value) {
	os.write_u8(static_cast<uint8_t>(DataTag::BOOLEAN));
	os.write_u8(value ? 0x01 : 0x00);
}

void CosemSerializer::enumeration(uint8_t value) {
	os.write_u8(static_cast<uint8_t>(DataTag::ENUM));
	os.write_u8(value);
}

void CosemSerializer::nulldata() {
	os.write_u8(0x00);
}

void CosemSerializer::optional() {
	os.write_u8(0x01);
}

void CosemSerializer::invoke_id_and_priority(InvokeId id, Priority priority) {
	os.write_u8(0xC8);
}

void CosemSerializer::attribute_descriptor(const CosemAttributeDescriptor &att) {
	os.write_u16(att.class_id);
	os.write_buffer(att.logical_name.data(), att.logical_name.size());
	os.write_i8(att.index);
}

void CosemSerializer::request(RequestTag request_tag, RequestType request_type) {
	os.write_u8(static_cast<uint8_t>(request_tag));
	os.write_u8(static_cast<uint8_t>(request_type));
}

}