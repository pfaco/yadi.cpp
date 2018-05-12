
#include <yadi/parser.h>

namespace yadi
{
    void parser::insert_size(std::vector<uint8_t> &buffer, unsigned size)
    {
        if (size <= 0x80) {
            buffer.push_back(static_cast<uint8_t>(size));
        } else if (size <= 0xFF) {
            buffer[1] = 0x81;
            buffer.push_back(static_cast<uint8_t>(size));
        } else if (size <= 0xFFFF) {
            buffer[1] = 0x82;
            buffer.push_back(static_cast<uint8_t>(size >> 8));
            buffer.push_back(static_cast<uint8_t>(size));
        } else if (size <= 0xFFFFFF) {
            buffer[1] = 0x83;
            buffer.push_back(static_cast<uint8_t>(size >> 16));
            buffer.push_back(static_cast<uint8_t>(size >> 8));
            buffer.push_back(static_cast<uint8_t>(size));
        } else {
            buffer[1] = 0x84;
            buffer.push_back(static_cast<uint8_t>(size >> 24));
            buffer.push_back(static_cast<uint8_t>(size >> 16));
            buffer.push_back(static_cast<uint8_t>(size >> 8));
            buffer.push_back(static_cast<uint8_t>(size));
        }
    }

    template<typename T>
    static void pack_sized_type(unsigned tag, std::vector<uint8_t> &buffer, const T& value)
    {
        buffer.push_back(tag);
        parser::insert_size(buffer, value.size());
        buffer.insert(buffer.end(), value.begin(), value.end());
    }

    auto parser::pack_octet_string(const std::vector<uint8_t> &value) -> std::vector<uint8_t>
    {
        std::vector<uint8_t> retval;
        parser::pack_octet_string(retval, value);
        return retval;
    }

    void parser::pack_octet_string(std::vector<uint8_t> &buffer, const std::vector<uint8_t> &value)
    {
        pack_sized_type(TypeTags::OCTET_STRING, buffer, value);
    }

    void parser::pack_string(std::vector<uint8_t> &buffer, const std::string &str)
    {
        pack_sized_type(TypeTags::STRING, buffer, str);
    }

    auto parser::to_string(std::vector<uint8_t> &buffer) -> std::string
    {
        std::string retval;
        //TODO
        return retval;
    }

}
