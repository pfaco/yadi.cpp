///@file

#include <iostream>
#include <iomanip>
#include <memory>
#include <algorithm>
#include <chrono>
#include "yadi/hdlc.h"
#include "yadi/cosem.h"
#include "yadi/emode.h"
#include "serial.h"

class SerialListener : public ssp::serial_listener
{
public:
    void bytes_sent(std::vector<uint8_t> const& buffer) override {
        print("TX: ", buffer);
    }

    void bytes_read(std::vector<uint8_t> const& buffer) override {
        print("RX: ", buffer);
    }

private:
    void print(std::string const& message, std::vector<uint8_t> const& buffer) {
        std::cout << message;
        std::cout << std::hex;
        for (auto b : buffer) {
            std::cout << std::setw(2) << std::setfill('0') << static_cast<unsigned>(b) << ' ';
        }
        std::cout << std::dec;
        std::cout << std::endl;
    }
};

class Logger {
public:
    void error(std::string const& str) {
        std::cout << "ERROR: " << str << std::endl;
    }
    void warning(std::string const& str) {
        std::cout << "WARNING: " << str << std::endl;
    }
    void message(std::string const& str) {
        std::cout << "MESSAGE: " << str << std::endl;
    }
};

Logger logger;

auto main() -> int
{
    try
    {
        auto listener = std::make_shared<SerialListener>();
        ssp::serial serial("COM8");
        dlms::Hdlc hdlc{serial};
        dlms::Cosem cosem{hdlc};

        serial.add_listener(listener);
        serial.port().set_params(ssp::Baudrate::_300, ssp::Parity::EVEN, ssp::Databits::_7, ssp::Stopbits::_1, 3000);
        hdlc.parameters().client_addr = 0x01;
        hdlc.parameters().server_addr = 0x01;

        if (dlms::emode_connect(serial, 9600) != 9600) {
            logger.error("emode.fail");
            return -1;
        }

        serial.port().set_params(ssp::Baudrate::_9600, ssp::Parity::NONE, ssp::Databits::_8, ssp::Stopbits::_1, 3000);
        if (!hdlc.connect()) {
            logger.error("hdlc.connect.fail");
            return -1;
        }
        if (cosem.connect() != dlms::AssociationResult::ACCEPTED) {
            logger.error("cosem.connect.fail");
            return -1;
        }

        auto response = cosem.get_request({dlms::ClassID::CLOCK, {"0.0.1.0.0.255"}, 2});

        if (response.result != dlms::DataAccessResult::SUCCESS) {
            logger.error("dlms.get_request.error");
            return -1;
        }

        std::cout << "Data received: ";
        for (auto &b : response.data) {
            std::cout << static_cast<unsigned>(b) << ' ';
        }
        std::cout << std::endl;
        hdlc.disconnect();

    } catch (std::runtime_error const& e) {
        std::cout << "runtime error" << std::endl;
        std::cout << e.what() << std::endl;
    } catch (std::exception const& e) {
        std::cout << "exception" << std::endl;
        std::cout << e.what() << std::endl;
    } catch (...) {
        std::cout << "unexpected exception" << std::endl;
    }

    return 0;
}
