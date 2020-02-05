///@file

#include <iostream>
#include <iomanip>
#include <yadi/cosem_serializer.h>
#include <yadi/cosem_parser.h>

void print_buffer(const std::vector<uint8_t> &buffer) {
    std::cout << std::hex;
    for (auto b : buffer) {
        std::cout << std::setw(2) << std::setfill('0') << (int)b << ' ';
    }
    std::cout << '\n' << std::dec;
}

void write_buffer(std::vector<uint8_t> &buffer, const uint8_t *value, size_t size) {
    buffer.insert(std::end(buffer), value, value + size);
}

int main(int argc, char * argv[])
{
    try
    {
        std::vector<uint8_t> buffer;
        dlms::GetRequest<> reqget = {dlms::InvokeIdAndPriority{}, {1, {1,0,1,8,0,255}, 2}};
        dlms::SetRequest<std::vector<uint8_t>> reqset = {dlms::InvokeIdAndPriority{}, {3, "1.0.1.9.0.255", 2}, {}, {1,2,3,4,5}};
        serialize(dlms::CosemSerializer{buffer}, reqget);
        print_buffer(buffer);
        buffer.clear();
        serialize(dlms::CosemSerializer{buffer}, reqset);
        print_buffer(buffer);
        buffer.clear();

        std::vector<uint8_t> response = {0xC4, 0x01, 0xC1, 0x00, 0x0A, 0x06, 0x30, 0x2E, 0x31, 0x2E, 0x31, 0x00};
        std::vector<uint8_t> bad_resp = {0xC4, 0x01, 0xC1, 0x01, 0x04};
        dlms::GetResponse<std::string> resp;
        parse(dlms::CosemParser{response}, resp);
        std::cout << "resp.invoke_id_and_priority = "  << static_cast<unsigned>(resp.invoke_id_and_priority.value()) << '\n';
        std::cout << "resp.result = " << static_cast<unsigned>(resp.result) << '\n';
        std::cout << "resp = " << resp.body << '\n';

        dlms::GetResponse<std::string> resp2;
        parse(dlms::CosemParser{bad_resp}, resp2);
        std::cout << "resp2.invoke_id_and_priority = "  << static_cast<unsigned>(resp2.invoke_id_and_priority.value()) << '\n';
        std::cout << "resp2.result = " << static_cast<unsigned>(resp2.result) << '\n';
        std::cout << "resp2 = " << resp2.body << '\n';

    } catch (std::runtime_error const& e) {
        std::cout << e.what() << std::endl;
    } catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "unexpected exception" << std::endl;
    }

    return 0;
}
