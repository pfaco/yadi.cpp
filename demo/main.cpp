///@file

#include <iostream>
#include <iomanip>
#include <yadi/cosem_client.h>

template<typename T>
void print_buffer(const std::vector<T> &buffer) {
        std::cout << std::hex;
        for (auto b : buffer) {
            std::cout << std::setw(2) << std::setfill('0') << (int)b << ' ';
        }
        std::cout << '\n' << std::dec;
    }

struct SerialPort
{
    void write(const std::vector<uint8_t> &buffer) {
        std::cout << std::hex;
        for (auto b : buffer) {
            std::cout << std::setw(2) << std::setfill('0') << (int)b << ' ';
        }
        std::cout << '\n' << std::dec;
    }

    void read(std::vector<uint8_t> &buffer) {
        static int control = 0;
        static const uint8_t resp[] = {0xC4, 0x01, 0xC1, 0x00, 0x04, 0x0d, 0x67, 0x50};
        static const uint8_t resp_set[] = {0xC5, 0x01, 0xC1, 0x03};
        static const uint8_t resp_action[] = {0xC7, 0x01, 0xC1, 0x03, 0x00};
        if (control == 0) {
            buffer.insert(buffer.end(), resp, resp + sizeof(resp));
            control++;
        } else if (control == 1) {
            buffer.insert(buffer.end(), resp_set, resp_set + sizeof(resp_set));
            control++;
        } else {
            buffer.insert(buffer.end(), resp_action, resp_action + sizeof(resp_action));
            control = 0;
        }
    }
};

struct MyCrazyData
{
    uint8_t val1;
    uint16_t val2;
    uint32_t val3;
};

namespace dlms
{
    void serialize(CosemSerializer &serializer, const MyCrazyData &data) {
        serializer.struct_header(3);
        serializer.uint8(data.val1);
        serializer.uint16(data.val2);
        serializer.uint32(data.val3);
    }
}

int main(int argc, char * argv[])
{
    try
    {
        SerialPort serial;
        dlms::CosemClient cosem;
        MyCrazyData crazy_data{0x31, 0x2223, 0x12345678};
        std::vector<bool> mybitstring = {0,1,1,0,0,1,1,1,0,1,0,1,0};

        auto resp = cosem.get_normal<std::vector<bool>>(serial, {1, {1,0,1,8,0,255}, 2});
        std::cout << "invoke = " << (int)resp.invoke_id_and_priority.value() << '\n';
        std::cout << "result = " << (int)resp.result << '\n';
        std::cout << "value = "; print_buffer(resp.body);
        std::cout << '\n';

        auto setresp = cosem.set_normal<std::vector<bool>>(serial, {1, {1,0,1,8,0,255}, 2}, mybitstring);
        std::cout << "invoke = " << (int)setresp.invoke_id_and_priority.value() << '\n';
        std::cout << "result = " << (int)setresp.result << '\n';
        std::cout << '\n';

        auto actionresp = cosem.action_normal<dlms::RawResponseBody>(serial, {129, {1,0,1,8,0,255}, 2});
        std::cout << "invoke = " << (int)setresp.invoke_id_and_priority.value() << '\n';
        std::cout << "result = " << (int)setresp.result << '\n';
        std::cout << '\n';

    } catch (dlms::CosemParserError const& e) {
        std::cout << e.what() << std::endl;
    } catch (std::runtime_error const& e) {
        std::cout << e.what() << std::endl;
    } catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "unexpected exception" << std::endl;
    }

    return 0;
}
