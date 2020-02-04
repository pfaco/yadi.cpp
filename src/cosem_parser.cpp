#include <yadi/cosem_parser.h>
#include <yadi/cosem_data_tag.h>

namespace dlms
{

namespace
{
    static void check_tag(DataTag expected, uint8_t tag) {
        if (static_cast<uint8_t>(expected) != tag) {
            throw CosemParserError{};
        }
    }

    static size_t read_size(ByteInputStream &is) {
        size_t size = is.read_u8();
        if (size <= 0x80) {
            return size;
        }
        else if (size == 0x81) {
            return is.read_u8();
        }
        else if (size == 0x82) {
            return is.read_u16();
        }
        else if (size == 0x83) {
            return (static_cast<uint32_t>(is.read_u16()) << 8U) | is.read_u8();
        }
        else if (size == 0x84) {
            return is.read_u32();
        }
        else {
            throw CosemParserError{};
        }
    }
}

int8_t CosemParser::int8() {
	check_tag(DataTag::INT8, is.read_u8());
    return is.read_i8();
}

int16_t CosemParser::int16() {
	check_tag(DataTag::INT16, is.read_u8());
    return is.read_i16();
}

int32_t CosemParser::int32() {
	check_tag(DataTag::INT32, is.read_u8());
    return is.read_i32();
}

int64_t CosemParser::int64() {
	check_tag(DataTag::INT64, is.read_u8());
    return is.read_i64();
}

uint8_t CosemParser::uint8() {
	check_tag(DataTag::UINT8, is.read_u8());
    return is.read_u8();
}

uint16_t CosemParser::uint16() {
	check_tag(DataTag::UINT16, is.read_u8());
    return is.read_u16();
}

uint32_t CosemParser::uint32() {
	check_tag(DataTag::UINT32, is.read_u8());
    return is.read_u32();
}

uint64_t CosemParser::uint64() {
	check_tag(DataTag::UINT64, is.read_u8());
    return is.read_u64();
}

float CosemParser::float32() {
	check_tag(DataTag::FLOAT32, is.read_u8());
    uint32_t value = is.read_u32();
    float temp = 0;
    memcpy(&temp, &value, sizeof(float));
    return temp;
}

double CosemParser::float64() {
	check_tag(DataTag::FLOAT64, is.read_u8());
    uint64_t value = is.read_u64();
    double temp = 0;
    memcpy(&temp, &value, sizeof(double));
    return temp;
}

std::string CosemParser::visible_string() {
	check_tag(DataTag::VISIBLE_STRING, is.read_u8());
    size_t size = read_size(is);
    std::string str;
    is.read_buffer(std::back_inserter(str), size);
    return str;
}

std::vector<uint8_t> CosemParser::octet_string() {
	check_tag(DataTag::OCTET_STRING, is.read_u8());
    size_t size = read_size(is);
    std::vector<uint8_t> vec;
    is.read_buffer(std::back_inserter(vec), size);
    return vec;
}

size_t CosemParser::array_size() {
	check_tag(DataTag::ARRAY, is.read_u8());
    return read_size(is);
}

size_t CosemParser::struct_size() {
	check_tag(DataTag::STRUCTURE, is.read_u8());
    return read_size(is);
}

bool CosemParser::boolean() {
	check_tag(DataTag::BOOLEAN, is.read_u8());
    uint8_t value = is.read_u8();
    return value == 0 ? false : true;
}

uint8_t CosemParser::enumeration() {
	check_tag(DataTag::ENUM, is.read_u8());
    return is.read_u8();
}

void CosemParser::raw_data(std::vector<uint8_t> &buffer) {
    is.read_buffer(std::back_inserter(buffer), is.available());
}

}
