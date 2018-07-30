///@file

#include <yadi/emode.h>
#include <cstdint>
#include <thread>
#include <chrono>
#include <string>

namespace dlms
{
    static std::vector<uint8_t> ask_baud_frame = {0x2f, 0x3f, 0x21, 0x0d, 0x0a}; //"/?!\r\n"
    static std::vector<uint8_t> accept_baud_frame = {0x06, 0x32, 0x00, 0x32, 0x0D, 0x0A};

    static bool is_frame_complete(const std::vector<uint8_t> &data)
    {
        return data.size() >= 15 && data[data.size()-2] == 0x0D && data[data.size()-1] == 0x0A;
    }

    bool emode::connect(PhyLayer &phy) {
        std::vector<uint8_t> buffer_rx;
        phy.send(ask_baud_frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(550));
        buffer_rx phy.read();
        accept_baud_frame[2] = buffer_rx[4] > 0x35 ? 0x35 : buffer_rx[4];
        phy.send(accept_baud_frame);
        std::this_thread::sleep_for(std::chrono::milliseconds(550));
        return true;
    }
}
