#pragma once

#include <vector>
#include <string>
#include <yadi/cosem_response.h>

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

class CosemParser
{
public:
    explicit CosemParser(const std::vector<uint8_t> &buffer) : is{ByteInputStream{buffer}} {}

    int8_t int8();
    int16_t int16();
    int32_t int32();
    int64_t int64();

    uint8_t uint8();
    uint16_t uint16();
    uint32_t uint32();
    uint64_t uint64();

    float float32();
    double float64();

    std::string visible_string();
    std::vector<uint8_t> octet_string();

    size_t array_size();
    size_t struct_size();

    bool boolean();

    uint8_t enumeration();

    void raw_data(std::vector<uint8_t> &buffer);

private:
    ByteInputStream is;
};

class CosemParserError : std::exception {
    const char * what() const noexcept override {
        return "COSEM parser error";
    }
};

static inline void parse(CosemParser &parser, int8_t &value) { value = parser.int8(); }
static inline void parse(CosemParser &parser, int16_t &value) { value = parser.int16(); }
static inline void parse(CosemParser &parser, int32_t &value) { value = parser.int32(); }
static inline void parse(CosemParser &parser, int64_t &value) { value = parser.int64(); }
static inline void parse(CosemParser &parser, uint8_t &value) { value = parser.uint8(); }
static inline void parse(CosemParser &parser, uint16_t &value) { value = parser.uint16(); }
static inline void parse(CosemParser &parser, uint32_t &value) { value = parser.uint32(); }
static inline void parse(CosemParser &parser, uint64_t &value) { value = parser.uint64(); }
static inline void parse(CosemParser &parser, float &value) { value = parser.float32(); }
static inline void parse(CosemParser &parser, double &value) { value = parser.float64(); }
static inline void parse(CosemParser &parser, bool &value) { value = parser.boolean(); }
static inline void parse(CosemParser &parser, std::string &value) { value = parser.visible_string(); }
static inline void parse(CosemParser &parser, std::vector<uint8_t> &value) { value = parser.octet_string(); }

static inline void parse(CosemParser &parser, RawResponseBody &value) {
    parser.raw_data(value.value);
}

template<typename Body = RawResponseBody>
void parse(CosemParser &parser, GetResponse<Body> &resp) {
    uint8_t invoke_id_and_priority = parser.uint8();
}

}
