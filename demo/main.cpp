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
        dlms::CosemSerializer serializer(buffer);
        std::string str = "It's me a Mario!";
        dlms::GetRequest<std::string> req = {dlms::InvokeId{}, dlms::Priority{}, {1, {1,0,1,8,0,255}, 2}, {}};
        serialize(serializer, req);
        print_buffer(buffer);

        std::vector<uint8_t> response = {0xC4, 0x01, 0xC1, 0x00, 0x0A, 0x06, 0x30, 0x2E, 0x31, 0x2E, 0x31, 0x00};
        dlms::GetResponse<std::string> resp;
        dlms::CosemParser parser(response);
        parse(parser, resp);

        std::cout << "resp = " << resp.body << '\n';

    } catch (std::runtime_error const& e) {
        std::cout << e.what() << std::endl;
    } catch (std::exception const& e) {
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "unexpected exception" << std::endl;
    }

    return 0;
}
