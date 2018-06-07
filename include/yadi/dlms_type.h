///@file

#ifndef YADI_DLMS_PARSER_H
#define YADI_DLMS_PARSER_H

#include <cstdint>
#include <vector>
#include <string>

namespace yadi
{

enum class type_tags : uint8_t
{
    NULL_DATA = 0,
    ARRAY = 1,
    STRUCTURE = 2,
    BOOLEAN = 3,
    BIT_STRING = 4,
    INT32 = 5,
    UINT32 = 6,
    OCTET_STRING = 9,
    STRING = 10,
    UTF8_STRING = 12,
    BCD = 13,
    INT8 = 15,
    INT16 = 16,
    UINT8 = 17,
    UINT16 = 18,
    INT64 = 20,
    UINT64 = 21,
    FLOAT32 = 23,
    FLOAT64 = 24,
    DATE_TIME = 25,
    DATE = 26,
    TIME = 27,
};

class dlms_type
{
public:
    static void write_size(std::vector<uint8_t> &buffer, unsigned size);
    static auto from_string(std::string const& str, type_tags tag = type_tags::STRING) -> std::vector<uint8_t>;
    static auto from_bytes(std::vector<uint8_t> const& data, type_tags tag = type_tags::OCTET_STRING) -> std::vector<uint8_t>;
    static auto to_string(std::vector<uint8_t> const& buffer) -> std::string;
    static auto to_bytes(std::vector<uint8_t> const& buffer) -> std::vector<uint8_t>;
};

}

#endif //YADI_DLMS_PARSER_H
