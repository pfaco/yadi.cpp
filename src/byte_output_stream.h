#pragma once

#include <vector>
#include <cstdint>

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
        write_u16(static_cast<uint16_t>(value));
    }

    void write_i32(int32_t value) {
        write_u32(static_cast<uint32_t>(value));
    }

    void write_i64(int64_t value) {
        write_u64(static_cast<uint64_t>(value));
    }

    void write_buffer(const uint8_t *value, size_t size) {
        buffer_.insert(std::end(buffer_), value, value + size);
    }

private:
    std::vector<uint8_t> &buffer_;
};

}
