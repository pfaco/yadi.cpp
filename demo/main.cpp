///@file

#include <iostream>
#include <iomanip>
#include <yadi/cosem_client.h>

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
        static const uint8_t resp[] = {0xC4, 0x01, 0xC1, 0x00, 0x0A, 0x06, 0x30, 0x2E, 0x31, 0x2E, 0x31, 0x00};
        buffer.insert(buffer.end(), resp, resp + sizeof(resp));
    }
};

int main(int argc, char * argv[])
{
    try
    {
        SerialPort serial;
        dlms::CosemClient cosem;

        auto resp = cosem.get<std::string>(serial, {1, "1.0.1.8.0.255", 2});
        std::cout << "resp invoke = " << (int)resp.invoke_id_and_priority.value() << '\n';
        std::cout << "resp result = " << (int)static_cast<uint8_t>(resp.result) << '\n';
        std::cout << "resp value = " << resp.body << '\n';

    } catch (std::runtime_error const& e) {
        std::cout << e.what() << std::endl;
    } catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "unexpected exception" << std::endl;
    }

    return 0;
}
