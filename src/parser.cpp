
#include <yadi/parser.h>

namespace yadi
{
    static void add_size(std::vector<uint8_t> &buffer, unsigned size, const std::vector<uint8_t>::iterator &it) {
        if (size <= 0x80) {
            buffer[1] = size;
        } else if (size <= 0xFF) {
            buffer[1] = 0x81;
            buffer.insert(it, size);
        } else if (size <= 0xFFFF) {
            buffer[1] = 0x82;
            buffer.insert(it, {static_cast<uint8_t>(size >> 8),
                               static_cast<uint8_t>(size)});
        } else if (size <= 0xFFFFFF) {
            buffer[1] = 0x83;
            buffer.insert(it, {static_cast<uint8_t>(size >> 16),
                               static_cast<uint8_t>(size >> 8),
                               static_cast<uint8_t>(size)});
        } else {
            buffer[1] = 0x84;
            buffer.insert(it, {static_cast<uint8_t>(size >> 24),
                               static_cast<uint8_t>(size >> 16),
                               static_cast<uint8_t>(size >> 8),
                               static_cast<uint8_t>(size)});
        }
    }

    static void push_back_size(std::vector<uint8_t> &buffer, unsigned size) {
        add_size(buffer, size, buffer.end());
    }

    static void insert_size(std::vector<uint8_t> &buffer, unsigned size) {
        add_size(buffer, size, buffer.begin()+2);
    }

    template<typename T>
    static void pack_sized_type(unsigned tag, std::vector<uint8_t> &buffer, const T& value) {
        buffer.push_back(tag);
        push_back_size(buffer, value.size());
        buffer.insert(buffer.end(), value.begin(), value.end());
    }

    void Parser::pack_octet_string(std::vector<uint8_t> &buffer, const std::vector<uint8_t> &value) {
        pack_sized_type(TypeTags::OCTET_STRING, buffer, value);
    }

    void Parser::pack_string(std::vector<uint8_t> &buffer, const std::string &str) {
        pack_sized_type(TypeTags::STRING, buffer, str);
    }

    std::string Parser::to_string(std::vector<uint8_t> &buffer) {
        std::string retval;
        //TODO
        return retval;
    }

}
