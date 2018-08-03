
#include <yadi/parser.h>

namespace dlms
{
    void write_size(std::vector<uint8_t> &buffer, unsigned size)
    {
        if (size <= 0x80) {
            buffer.push_back(static_cast<uint8_t>(size));
        } else if (size <= 0xFF) {
            buffer.push_back(0x81);
            buffer.push_back(static_cast<uint8_t>(size));
        } else if (size <= 0xFFFF) {
            buffer.push_back(0x82);
            buffer.push_back(static_cast<uint8_t>(size >> 8));
            buffer.push_back(static_cast<uint8_t>(size));
        } else if (size <= 0xFFFFFF) {
            buffer.push_back(0x83);
            buffer.push_back(static_cast<uint8_t>(size >> 16));
            buffer.push_back(static_cast<uint8_t>(size >> 8));
            buffer.push_back(static_cast<uint8_t>(size));
        } else {
            buffer.push_back(0x84);
            buffer.push_back(static_cast<uint8_t>(size >> 24));
            buffer.push_back(static_cast<uint8_t>(size >> 16));
            buffer.push_back(static_cast<uint8_t>(size >> 8));
            buffer.push_back(static_cast<uint8_t>(size));
        }
    }

    template<typename T>
    static auto pack_sized_type(uint8_t tag, const T& value) -> std::vector<uint8_t>
    {
        std::vector<uint8_t> buffer;
        buffer.push_back(tag);
        write_size(buffer, value.size());
        buffer.insert(buffer.end(), value.begin(), value.end());
        return buffer;
    }

    auto from_string(std::string const& str, DataType tag) -> std::vector<uint8_t>
    {
        switch(tag) {
            case DataType::OCTET_STRING:
            case DataType::STRING:
                return pack_sized_type(static_cast<uint8_t>(tag), str);
        }
    }

    auto from_bytes(std::vector<uint8_t> const& data, DataType tag) -> std::vector<uint8_t>
    {
        switch(tag) {
            case DataType::OCTET_STRING:
            case DataType::STRING:
                return pack_sized_type(static_cast<uint8_t>(tag), data);
        }
    }

    auto to_string(std::vector<uint8_t> const& buffer) -> std::string
    {
        std::string retval;
        //TODO
        return retval;
    }

    auto to_bytes(std::vector<uint8_t> const& buffer) -> std::vector<uint8_t>
    {

    }

}
