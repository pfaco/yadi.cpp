#pragma once

#include <vector>
#include <string>

namespace dlms
{

class ByteOutputStream
{
public:
    explicit ByteOutputStream(std::vector<uint8_t> &buffer) : buffer_{buffer} {}
    
    void write_u8(uint8_t value) {
        buffer_.push_back(value);
    }
    
    void write_u16(uint16_t value) {
        write_u8(static_cast<uint8_t>(value >> 8)); 
        write_u8(static_cast<uint8_t>(value));
    }

    void write_u32(uint32_t value) {
        write_u16(static_cast<uint16_t>(value >> 16)); 
        write_u16(static_cast<uint16_t>(value));
    }

    void write_u64(uint64_t value) {
        write_u32(static_cast<uint32_t>(value >> 32)); 
        write_u32(static_cast<uint32_t>(value));
    }

    void write_i8(int8_t value) {
        write_u8(static_cast<uint8_t>(value));
    }
    
    void write_i16(int16_t value) {
        write_u8(static_cast<uint16_t>(value));
    }

    void write_i32(int32_t value) {
        write_u8(static_cast<uint32_t>(value));
    }

    void write_i64(int64_t value) {
        write_u8(static_cast<uint64_t>(value));
    }

    void write_buffer(const uint8_t *value, size_t size) {
        buffer_.insert(std::end(buffer_), value, value + size);
    }

private:
    std::vector<uint8_t> &buffer_;
};

class CosemSerializer
{
public:
    explicit CosemSerializer(std::vector<uint8_t> &buffer) : os{ByteOutputStream{buffer}} {}

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

    void array_header(size_t size);
    void struct_header(size_t size);

    void boolean(bool value);

    void enumeration(uint8_t value);

private:
    ByteOutputStream os;
};

void serialize(CosemSerializer serializer, int8_t value) {serializer.int8(value);}
void serialize(CosemSerializer serializer, int16_t value) {serializer.int16(value);}
void serialize(CosemSerializer serializer, int32_t value) {serializer.int32(value);}
void serialize(CosemSerializer serializer, int64_t value) {serializer.int64(value);}
void serialize(CosemSerializer serializer, uint8_t value) {serializer.uint8(value);}
void serialize(CosemSerializer serializer, uint16_t value) {serializer.uint16(value);}
void serialize(CosemSerializer serializer, uint32_t value) {serializer.uint32(value);}
void serialize(CosemSerializer serializer, uint64_t value) {serializer.uint64(value);}
void serialize(CosemSerializer serializer, float value) {serializer.float32(value);}
void serialize(CosemSerializer serializer, double value) {serializer.float64(value);}
void serialize(CosemSerializer serializer, bool value) {serializer.boolean(value);}
void serialize(CosemSerializer serializer, const std::string &value) {serializer.visible_string(value);}
void serialize(CosemSerializer serializer, const std::vector<uint8_t> &value) {serializer.octet_string(value);}

}
