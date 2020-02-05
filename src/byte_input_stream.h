#pragma once

#include <vector>
#include <cstdint>

namespace dlms
{

class ByteInputStream
{
public:
    explicit ByteInputStream(const std::vector<uint8_t> &buffer) : buffer_{buffer}, index_{0} {}
    
    uint8_t read_u8() {
        return buffer_[index_++];
    }
    
    uint16_t read_u16() {
        return static_cast<uint16_t>((static_cast<uint16_t>(read_u8()) << 8U) | read_u8());
    }

    uint32_t read_u32() {
        return static_cast<uint32_t>((static_cast<uint32_t>(read_u16()) << 16U) | read_u16());
    }

    uint64_t read_u64() {
        return static_cast<uint64_t>((static_cast<uint64_t>(read_u32()) << 32U) | read_u32());
    }

    int8_t read_i8() {
        return static_cast<int8_t>(read_u8());
    }
    
    int16_t read_i16() {
        return static_cast<int16_t>(read_u16());
    }

    int32_t read_i32() {
        return static_cast<int32_t>(read_u32());
    }

    int64_t read_i64() {
        return static_cast<int64_t>(read_u64());
    }

    size_t available() {
        return index_ < buffer_.size() ? buffer_.size() - index_ : 0;
    }

    template<typename Iterator>
    void read_buffer(Iterator it, size_t size) {
        while (size--) {
            *it++ = read_u8();
        }
    }

private:
    const std::vector<uint8_t> &buffer_;
    size_t index_;
};

}
