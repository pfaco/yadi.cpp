
#include <yadi/parser.h>
#include <exception>

namespace dlms
{
    void write_size(std::vector<uint8_t> &buffer, size_t size)
    {
		if (size > 0xFFFFFFFFLL) {
			throw std::runtime_error{ "size os too big" };
		}
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
		auto buffer = std::vector<uint8_t>{};
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
        return {};
    }

    auto from_bytes(std::vector<uint8_t> const& data, DataType tag) -> std::vector<uint8_t>
    {
        switch(tag) {
            case DataType::OCTET_STRING:
            case DataType::STRING:
                return pack_sized_type(static_cast<uint8_t>(tag), data);
        }
        return {};
    }

    auto to_string(const std::vector<uint8_t> &buffer) -> std::string
    {
        auto retval = std::string{};

        if (buffer.size() < 2) {
            return retval;
        }

        switch (buffer[0]) {
        case 0x06:
        {
            if (buffer.size() < 5) {
                throw std::underflow_error{"not enough bytes"};
            }
            uint32_t value = buffer[1];
            value <<= 8U;
            value |= buffer[2];
            value <<= 8U;
            value |= buffer[3];
            value <<= 8U;
            value |= buffer[4];
            return std::to_string(value);
        }

        case 0x0A:
            auto size = buffer[1];
            for (auto i = 0; i < size; ++i) {
                retval += buffer[i];
            }
            break;
        }

        return retval;
    }

    auto to_bytes(std::vector<uint8_t> const& buffer) -> std::vector<uint8_t>
    {
        return {};
    }

}
