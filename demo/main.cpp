///@file

#include <iostream>
#include "yadi/dlms.h"
#include "yadi/emode.h"
#include "yadi/parser.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <sstream>
#include <ssp/serial.h>

auto logger = spdlog::stdout_logger_mt("log");

static void print_bytes(const std::string &msg, const std::vector<uint8_t> &buffer)
{
    std::stringstream stream;
    stream << msg;
    for (auto b : buffer) {
        stream << fmt::format("{:02X} ", b);
    }
    logger->debug(stream.str());
}

int main(int argc, char * argv[])
{
    try
    {
        auto dlms_client = dlms::CosemHdlcClient<ssp::SerialPort>{};
        auto serial = ssp::SerialPort("COM6", ssp::Baudrate::_9600, ssp::Parity::NONE, ssp::Databits::_8, ssp::Stopbits::_1, 2000);
        serial.install_rx_listener([](auto buffer) {print_bytes("RX: ", buffer);});
        serial.install_tx_listener([](auto buffer) {print_bytes("TX: ", buffer);});

        logger->set_level(spdlog::level::trace);
        logger->info("initiating transaction..");

        if (!dlms_client.connect(serial)) {
            logger->error("fail to connect");
        } else {
            
            auto response_serial_num = dlms_client.get_request(serial, {dlms::ClassID::DATA, {"0.0.96.1.0.255"}, 2, {}});
            if (response_serial_num.result != dlms::DataAccessResult::SUCCESS) {
                logger->error("failure to read serial number: {}", static_cast<uint8_t>(response_serial_num.result));
            } else {
                auto serial_num = dlms::to_string(response_serial_num.data);
                logger->info("serial number = {}", serial_num);
            }

            auto response_active_energy = dlms_client.get_request(serial, {dlms::ClassID::REGISTER, {"1.0.1.8.0.255"}, 2, {}});
            if (response_active_energy.result != dlms::DataAccessResult::SUCCESS) {
                logger->error("failure to read active energy: {}", static_cast<uint8_t>(response_serial_num.result));
            } else {
                auto active_fwd_energy = dlms::to_string(response_active_energy.data);
                logger->info("active energy = {}", active_fwd_energy);
            }

            if (!dlms_client.disconnect(serial)) {
                logger->error("fail to disconnect");
            }

        }

        logger->info("transaction finished");

    } catch (const std::runtime_error &e) {
		logger->error("runtime error: {}", e.what());
    } catch (const std::exception &e) {
		logger->error("exception: {}", e.what());
    } catch (...) {
		logger->error("unknown exception");
    }

    return 0;
}
