///@file

#ifndef HDLC_FRAME_H_
#define HDLC_FRAME_H_

#include <cstdint>
#include <vector>

namespace dlms {
namespace hdlc {

	/**
	 * The posible commands
	 */
	typedef enum HdlcCommand
	{
		COMMAND_I = 0x00, ///< Information response = transfer and acknowledge of information
		COMMAND_RR = 0x01, ///< Receive ready = not supported in this implementation
		COMMAND_RNR = 0x05, ///< Receive not ready = not supported in this implementation
		COMMAND_SNRM = 0x83, ///< Set Normal Response Mode = connects to the server
		COMMAND_DISC = 0x43, ///< Disconnect = disconnects from the server
		COMMAND_UI = 0x03, ///< Unnumbered information = Information without sequence number
		RESPONSE_I = 0x00, ///< Information response = transfer and acknowledge of information
		RESPONSE_RR = 0x01, ///< Receive ready = not supported in this implementation
		RESPONSE_RNR = 0x05, ///< Receive not ready = not supported in this implementation
		RESPONSE_UA = 0x63, ///< Unnumbered acknowledge = response to SNRM and DISC
		RESPONSE_DM = 0x0F, ///< Disconnected mode = response when client is disconnected
		RESPONSE_FRMR = 0x87, ///< Frame reject = response upon reception of invalid command, invalid sequence number, etc.
		RESPONSE_UI = 0x03, ///< Unnumbered information = Information response without sequence number
	}
	HdlcCommand;

	/**
	 *
	 */
	typedef enum HdlcClientAddress
	{
		CLIENT_ADDR_NO_STATION = 0x00,
		CLIENT_ADDR_MANAGEMENT_PROCESS = 0x01,
		CLIENT_ADDR_PUBLIC_CLIENT = 0x10,
	}
	HdlcClientAddress;

	/**
	 *
	 */
	typedef enum HdlcServerAddress
	{
		SERVER_ADDR_NO_STATION = 0x0000,
		SERVER_ADDR_MANAGEMENT_LOGICAL_DEVICE = 0x0001,
		SERVER_ADDR_BROADCAST = 0x3FFF,
	}
	HdlcServerAddress;

	/**
	 *
	 */
	typedef struct HdlcFrame
    {
		HdlcCommand cmd;
        uint8_t client_address;
		uint8_t rrr;
		uint8_t sss;
		uint16_t physical_address;
		uint16_t logical_address;
        std::vector<uint8_t> payload;
	}
	HdlcFrame;

	/**
	 * Parses the bytes of the buffer and, if it contains a valid HDLC frame,
	 * populates the hdlc_frame_t structure with the frame data.
	 * @param frame pointer to hdlc_frame_t structure to be populated
	 * @param buffer pointer to data
	 * @param buffer_size number of bytes in data
	 * @return true if the buffer contained a valid HDLC frame, false otherwise
	 */
    auto hdlc_frame_parse(const std::vector<uint8_t> &buffer) -> HdlcFrame;

	/**
	 *
	 * @param frame
	 * @param buffer
	 * @param buffer_size
	 * @return
	 */
    auto hdlc_frame_serialize(const HdlcFrame &frame) -> std::vector<uint8_t>;

    struct invalid_hdlc_frame : public std::exception {
        const char* what() const noexcept override {
            return "invalid hdlc frame";
        }
    };

} //namespace hdlc
} //namespace dlms

#endif
