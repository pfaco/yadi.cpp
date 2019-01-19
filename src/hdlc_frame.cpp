///@file

#include "hdlc_frame.h"

namespace dlms {
namespace hdlc {

    static uint16_t checksequence_calc(const uint8_t *data, uint16_t size);
    static bool valid_frame(const uint8_t * buffer, size_t buffer_size);

    auto hdlc_frame_parse(const std::vector<uint8_t> &buffer) -> HdlcFrame
	{
        if (!valid_frame(buffer.data(), buffer.size())) {
            throw invalid_hdlc_frame{};
		}

        HdlcFrame frame;
        uint32_t address = 0;
        uint16_t offset = 3;

        frame.client_address = buffer[offset++] >> 1U;

		while ((address & 0x01) != 0x01) {
			address <<= 8;
			address |= buffer[offset++];
		}

		if (offset == 5) {
            frame.logical_address = ((address >> 9U) & 0x7F);
            frame.physical_address = ((address >> 1U) & 0x7F);
		}
		else {
            frame.logical_address = ((address >> 18U) & 0x3F80) | ((address >> 17U) & 0x7F);
            frame.physical_address = ((address >> 2U) & 0x3F80) | ((address >> 1U) & 0x7F);
		}

        frame.cmd = (HdlcCommand)(buffer[offset++] & 0xEF);
        frame.rrr = 0;
        frame.sss = 0;
        if ((frame.cmd & 0x01) == 0x00) {
            frame.rrr = (frame.cmd >> 5) & 0x07;
            frame.sss = (frame.cmd >> 1) & 0x07;
            frame.cmd = (HdlcCommand)0;
		}

        offset += 2;

        if (frame.cmd == RESPONSE_I && (buffer[offset++] != 0xE6 || buffer[offset++] != 0xE7 || buffer[offset++] != 0x00)) {
            throw invalid_hdlc_frame{};
        }

        auto begin = std::begin(buffer) + offset;
        auto end = std::begin(buffer) + buffer.size() - 3;

        if (end > begin) {
            frame.payload.insert(std::end(frame.payload), begin, end);
        }

        return frame;
	}

    auto hdlc_frame_serialize(const HdlcFrame &frame) -> std::vector<uint8_t>
	{
        std::vector<uint8_t> buffer;
		uint16_t fcs = 0;
		uint16_t size = 10;

        if (frame.payload.size() > 0U) {
            size += frame.payload.size();
			size += 2U;
            if (frame.cmd == COMMAND_I) {
                size += 3;
            }
		}

        buffer.push_back(0x7EU);
        buffer.push_back(0xA0U | ((size >> 8U) & 0x0FU));
        buffer.push_back((uint8_t)size);
        buffer.push_back((frame.logical_address >> 6U) & 0xFE);
        buffer.push_back((frame.logical_address << 1U) & 0xFE);
        buffer.push_back((frame.physical_address >> 6U) & 0xFE);
        buffer.push_back(((frame.physical_address << 1U) & 0xFE) | 0x01U);
        buffer.push_back((uint8_t)((frame.client_address << 1) | 0x01));
        if ((frame.cmd & 0x01) != 0x00) {
            buffer.push_back((uint8_t)(frame.cmd | 0x10));
		}
		else {
            buffer.push_back((uint8_t)(frame.cmd | 0x10 | (((frame.sss + 1) & 0x07) << 5) | ((frame.rrr & 0x07) << 1)));
		}


        fcs = checksequence_calc(&buffer[1], buffer.size() - 1);
        buffer.push_back((uint8_t)fcs);
        buffer.push_back((uint8_t)(fcs >> 8U));

        if (frame.cmd == COMMAND_I) {
            buffer.push_back(0xE6);
            buffer.push_back(0xE6);
            buffer.push_back(0x00);
        }

        buffer.insert(std::end(buffer), std::begin(frame.payload), std::end(frame.payload));

        if (frame.payload.size() > 0U) {
            fcs = checksequence_calc(&buffer[1], buffer.size() - 1);
            buffer.push_back((uint8_t)fcs);
            buffer.push_back((uint8_t)(fcs >> 8));
		}

        buffer.push_back(0x7EU);

        return buffer;
	}

    static bool valid_frame(const uint8_t * buffer, size_t size)
	{
		uint16_t fcs = 0;
        uint16_t addr_offset = 0;

		if (size < 10) {
			return false;
		}

		if (buffer[0] != 0x7E) {
			return false;
		}

		if (buffer[size - 1] != 0x7E) {
			return false;
		}

		if ((buffer[1] & 0xF0) != 0xA0) {
			return false;
		}

		if (((buffer[1] & 0x0F) << 8U | buffer[2]) != (size - 2)) {
			return false;
		}

		fcs = (uint16_t)((buffer[size - 2] << 8) | buffer[size - 3]);
		if (fcs != checksequence_calc(&buffer[1], size - 4)) {
			return false;
		}

        if ((buffer[3] & 0x01) != 0x01) {
            return false;
        }

        while ((addr_offset < 4) && ((buffer[addr_offset + 4] & 0x01) != 0x01)) {
            ++addr_offset;
		}

        if ((addr_offset != 1) && (addr_offset != 3)) {
			return false;
		}

        if ((addr_offset == 3) && (size < 12)) {
			return false;
		}

        if ((buffer[addr_offset + 5] & 0x10) != 0x10) {
			return false;
		}

        size -= addr_offset;
		size -= 9U;

		if ((size != 0U) && (size < 3U)) {
			return false;
		}

		if (size != 0U) {
            fcs = (uint16_t)((buffer[addr_offset + 5] << 8) | buffer[addr_offset + 6]);
            return fcs != checksequence_calc(&buffer[1], addr_offset + 5);
		}

		return true;
	}

	static uint16_t checksequence_calc(const uint8_t *data, uint16_t size)
	{
		static const uint16_t table[] = {
			0x0000, 0x1189, 0x2312, 0x329B, 0x4624, 0x57AD, 0x6536, 0x74BF,
			0x8C48, 0x9DC1, 0xAF5A, 0xBED3, 0xCA6C, 0xDBE5, 0xE97E, 0xF8F7,
			0x1081, 0x0108, 0x3393, 0x221A, 0x56A5, 0x472C, 0x75B7, 0x643E,
			0x9CC9, 0x8D40, 0xBFDB, 0xAE52, 0xDAED, 0xCB64, 0xF9FF, 0xE876,
			0x2102, 0x308B, 0x0210, 0x1399, 0x6726, 0x76AF, 0x4434, 0x55BD,
			0xAD4A, 0xBCC3, 0x8E58, 0x9FD1, 0xEB6E, 0xFAE7, 0xC87C, 0xD9F5,
			0x3183, 0x200A, 0x1291, 0x0318, 0x77A7, 0x662E, 0x54B5, 0x453C,
			0xBDCB, 0xAC42, 0x9ED9, 0x8F50, 0xFBEF, 0xEA66, 0xD8FD, 0xC974,
			0x4204, 0x538D, 0x6116, 0x709F, 0x0420, 0x15A9, 0x2732, 0x36BB,
			0xCE4C, 0xDFC5, 0xED5E, 0xFCD7, 0x8868, 0x99E1, 0xAB7A, 0xBAF3,
			0x5285, 0x430C, 0x7197, 0x601E, 0x14A1, 0x0528, 0x37B3, 0x263A,
			0xDECD, 0xCF44, 0xFDDF, 0xEC56, 0x98E9, 0x8960, 0xBBFB, 0xAA72,
			0x6306, 0x728F, 0x4014, 0x519D, 0x2522, 0x34AB, 0x0630, 0x17B9,
			0xEF4E, 0xFEC7, 0xCC5C, 0xDDD5, 0xA96A, 0xB8E3, 0x8A78, 0x9BF1,
			0x7387, 0x620E, 0x5095, 0x411C, 0x35A3, 0x242A, 0x16B1, 0x0738,
			0xFFCF, 0xEE46, 0xDCDD, 0xCD54, 0xB9EB, 0xA862, 0x9AF9, 0x8B70,
			0x8408, 0x9581, 0xA71A, 0xB693, 0xC22C, 0xD3A5, 0xE13E, 0xF0B7,
			0x0840, 0x19C9, 0x2B52, 0x3ADB, 0x4E64, 0x5FED, 0x6D76, 0x7CFF,
			0x9489, 0x8500, 0xB79B, 0xA612, 0xD2AD, 0xC324, 0xF1BF, 0xE036,
			0x18C1, 0x0948, 0x3BD3, 0x2A5A, 0x5EE5, 0x4F6C, 0x7DF7, 0x6C7E,
			0xA50A, 0xB483, 0x8618, 0x9791, 0xE32E, 0xF2A7, 0xC03C, 0xD1B5,
			0x2942, 0x38CB, 0x0A50, 0x1BD9, 0x6F66, 0x7EEF, 0x4C74, 0x5DFD,
			0xB58B, 0xA402, 0x9699, 0x8710, 0xF3AF, 0xE226, 0xD0BD, 0xC134,
			0x39C3, 0x284A, 0x1AD1, 0x0B58, 0x7FE7, 0x6E6E, 0x5CF5, 0x4D7C,
			0xC60C, 0xD785, 0xE51E, 0xF497, 0x8028, 0x91A1, 0xA33A, 0xB2B3,
			0x4A44, 0x5BCD, 0x6956, 0x78DF, 0x0C60, 0x1DE9, 0x2F72, 0x3EFB,
			0xD68D, 0xC704, 0xF59F, 0xE416, 0x90A9, 0x8120, 0xB3BB, 0xA232,
			0x5AC5, 0x4B4C, 0x79D7, 0x685E, 0x1CE1, 0x0D68, 0x3FF3, 0x2E7A,
			0xE70E, 0xF687, 0xC41C, 0xD595, 0xA12A, 0xB0A3, 0x8238, 0x93B1,
			0x6B46, 0x7ACF, 0x4854, 0x59DD, 0x2D62, 0x3CEB, 0x0E70, 0x1FF9,
			0xF78F, 0xE606, 0xD49D, 0xC514, 0xB1AB, 0xA022, 0x92B9, 0x8330,
			0x7BC7, 0x6A4E, 0x58D5, 0x495C, 0x3DE3, 0x2C6A, 0x1EF1, 0x0F78
		};

		uint16_t fcs = 0xffff;

		while (size--) {
			fcs = (fcs >> 8u) ^ table[(fcs ^ *data++) & 0xff];
		}

		return ~fcs;
	}

} //namespace hdlc
} //namespace dlms
