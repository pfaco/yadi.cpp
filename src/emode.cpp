///@file

#include <yadi/emode.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include <string>

namespace dlms
{
    static auto ask_baud_frame = std::vector<uint8_t>{0x2f, 0x3f, 0x21, 0x0d, 0x0a}; //"/?!\r\n"
    static auto accept_baud_frame = std::vector<uint8_t>{0x06, 0x32, 0x00, 0x32, 0x0D, 0x0A};

    bool emode_parse_baudrate_response(const std::vector<uint8_t> &data)
    {
        return data.size() >= 15 && data[data.size()-2] == 0x0D && data[data.size()-1] == 0x0A;
    }

    auto emode_serialize_ask_baudrate(EmodeBaud baud) -> std::vector<uint8_t>
    {
        return ask_baud_frame;
    }

    auto emode_serialize_accept_baudrate() -> std::vector<uint8_t>
    {
        return accept_baud_frame;
    }
}
