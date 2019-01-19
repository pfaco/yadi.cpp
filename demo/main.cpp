///@file

#include <iostream>
#include "yadi/dlms.h"
#include "yadi/emode.h"
#include "yadi/parser.h"
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_sinks.h"
#include <sstream>
#include <ssp/serial.h>

auto console = spdlog::stdout_logger_mt("console");

static void print_bytes(const std::string &msg, const std::vector<uint8_t> &buffer)
{
    std::stringstream stream;
        stream << msg;
        for (auto b : buffer) {
            stream << fmt::format("{:02X} ", b);
        }
        console->info(stream.str());
}

int main(int argc, char * argv[])
{
    try
    {
        auto dlms_client = dlms::CosemHdlcClient<ssp::SerialPort>{};
        auto serial = ssp::SerialPort("COM6");
        serial.install_rx_listener([](auto buffer) {print_bytes("RX: ", buffer);});
        serial.install_tx_listener([](auto buffer) {print_bytes("TX: ", buffer);});
        serial.set_params(ssp::Baudrate::_9600, ssp::Parity::NONE, ssp::Databits::_8, ssp::Stopbits::_1, 2000);

        console->info("initiating transaction..");

        if (!dlms_client.connect(serial)) {
            console->error("fail to connect");
        }

        auto response_serial_num = dlms_client.get_request(serial, {dlms::ClassID::DATA, {"0.0.96.1.0.254"}, 2, {}});
        if (response_serial_num.result != dlms::DataAccessResult::SUCCESS) {
            console->error("failure to read serial number: {}", static_cast<uint8_t>(response_serial_num.result));
        } else {
            auto serial_num = dlms::to_string(response_serial_num.data);
            console->info("serial number = {}", serial_num);
        }

        auto response_active_energy = dlms_client.get_request(serial, {dlms::ClassID::REGISTER, {"1.0.1.8.0.255"}, 2, {}});
        if (response_active_energy.result != dlms::DataAccessResult::SUCCESS) {
            console->error("failure to read active energy: {}", static_cast<uint8_t>(response_serial_num.result));
        } else {
            auto active_fwd_energy = dlms::to_string(response_active_energy.data);
            console->info("active energy = {}", active_fwd_energy);
        }

        if (!dlms_client.disconnect(serial)) {
            console->error("fail to disconnect");
        }

        console->info("transaction finished");

    } catch (std::runtime_error const& e) {
		console->error("runtime error: {}", e.what());
    } catch (std::exception const& e) {
		console->error("exception: {}", e.what());
    } catch (...) {
		console->error("unknown exception");
    }

    return 0;
}
