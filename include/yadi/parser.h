///@file

#ifndef YADI_DLMS_PARSER_H
#define YADI_DLMS_PARSER_H

#include <cstdint>
#include <vector>
#include <string>

namespace dlms
{

enum class DataType : uint8_t
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

void write_size(std::vector<uint8_t> &buffer, size_t size);
auto from_string(std::string const& str, DataType tag = DataType::STRING) -> std::vector<uint8_t>;
auto from_bytes(std::vector<uint8_t> const& data, DataType tag = DataType::OCTET_STRING) -> std::vector<uint8_t>;
auto to_string(std::vector<uint8_t> const& buffer) -> std::string;
auto to_bytes(std::vector<uint8_t> const& buffer) -> std::vector<uint8_t>;

}

#endif //YADI_DLMS_PARSER_H
