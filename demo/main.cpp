///@file

#include <iostream>
#include <iomanip>
#include <yadi/cosem_client.h>

void print_buffer(const std::vector<uint8_t> &buffer) {
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
        static const uint8_t resp[] = {0xC4, 0x01, 0xC1, 0x00, 0x0A, 0x06, 0x30, 0x2E, 0x31, 0x2E, 0x31, 0x00};
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

int main(int argc, char * argv[])
{
    try
    {
        SerialPort serial;
        dlms::CosemClient cosem;

        auto resp = cosem.get_normal<dlms::RawResponseBody>(serial, {1, "1.0.1.8.0.255", 2});
        std::cout << "invoke = " << (int)resp.invoke_id_and_priority.value() << '\n';
        std::cout << "result = " << (int)static_cast<uint8_t>(resp.result) << '\n';
        std::cout << "value = "; print_buffer(resp.body.value);
        std::cout << '\n';

        auto setresp = cosem.set_normal<>(serial, {1, "1.0.1.8.0.255", 2}, uint32_t{0x1234});
        std::cout << "invoke = " << (int)setresp.invoke_id_and_priority.value() << '\n';
        std::cout << "result = " << (int)static_cast<uint8_t>(setresp.result) << '\n';
        std::cout << '\n';

        auto actionresp = cosem.action_normal<dlms::RawResponseBody>(serial, {129, "0.1.0.1.0.255", 2});
        std::cout << "invoke = " << (int)setresp.invoke_id_and_priority.value() << '\n';
        std::cout << "result = " << (int)static_cast<uint8_t>(setresp.result) << '\n';
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
