/*
 * This file is part of the yadi.cpp project.
 *
 * Copyright (C) 2017 Paulo Faco <paulofaco@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as
 * published by the Free Software Foundation, either version 3 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

///@file

#include <yadi/hdlc.h>
#include <algorithm>
#include <exception>
#include "hdlc_frame.h"

namespace dlms
{
namespace hdlc
{

static std::vector<uint8_t> get_frame(const HdlcCommand cmd, const HdlcParameters &params, const std::vector<uint8_t> &data)
{
    HdlcFrame frame;
    frame.cmd = cmd;
    frame.client_address = params.client_address;
    frame.logical_address = params.server_logical_address;
    frame.physical_address = params.server_physical_address;
    frame.rrr = 0;
    frame.sss = 1;
    frame.payload.insert(std::end(frame.payload), std::begin(data), std::end(data));
    return hdlc_frame_serialize(frame);
}

	/**
	 * Populates the buffer with the payload of a Set Normal Response Mode (SNRM) frame
	 * In this version it is not supported window size (rx or tx) different of 1, so those
	 * bytes are never generated. The bytes for settings the max_info_len parameters are only
	 * generated if those are different from the default (128).
	 */
    auto serialize_snrm(const HdlcParameters &params) -> std::vector<uint8_t>
	{
		auto temp_buffer = std::vector<uint8_t>{};

        if (params.max_information_field_length_tx < 128) {
			temp_buffer.push_back(0x05);
			temp_buffer.push_back(1);
            temp_buffer.push_back(static_cast<uint8_t>(params.max_information_field_length_tx));
		}
        else if (params.max_information_field_length_tx > 128) {
			temp_buffer.push_back(0x05);
			temp_buffer.push_back(2);
            temp_buffer.push_back(params.max_information_field_length_tx >> 8);
            temp_buffer.push_back(static_cast<uint8_t>(params.max_information_field_length_tx));
		}

        if (params.max_information_field_length_rx < 128) {
			temp_buffer.push_back(0x06);
			temp_buffer.push_back(1);
            temp_buffer.push_back(static_cast<uint8_t>(params.max_information_field_length_rx));
		}
        else if (params.max_information_field_length_rx > 128) {
			temp_buffer.push_back(0x06);
			temp_buffer.push_back(2);
            temp_buffer.push_back(params.max_information_field_length_rx >> 8);
            temp_buffer.push_back(static_cast<uint8_t>(params.max_information_field_length_rx));
		}

        return get_frame(COMMAND_SNRM, params, temp_buffer);
	}

    auto serialize_disc(const HdlcParameters &params) -> std::vector<uint8_t>
    {
        return get_frame(COMMAND_DISC, params, {});
    }

    auto serialize(const HdlcParameters &params, HdlcContext &ctx, const std::vector<uint8_t> &data) -> std::vector<uint8_t>
    {
        return get_frame(COMMAND_I, params, data);
    }

    auto parse(const HdlcParameters &params, HdlcContext &ctx, const std::vector<uint8_t> &buffer) -> std::vector<uint8_t>
	{
        auto frame = hdlc_frame_parse(buffer);
        return frame.payload;
    }

    bool parse_disc_response(const std::vector<uint8_t> &buffer)
    {
        return hdlc_frame_parse(buffer).cmd == RESPONSE_UA;
    }

    bool parse_snrm_response(const HdlcParameters &params, HdlcContext &ctx, const std::vector<uint8_t> &buffer)
	{
        auto frame = hdlc_frame_parse(buffer);

        if (frame.payload.size() == 0) {
            return false;
        }

        if ((frame.payload.size() < 3) || (frame.payload[0] != 0x80) || (frame.payload[1] != 0x81)) {
            return false;
        }

        if (frame.cmd != RESPONSE_UA) {
            return false;
        }

        ctx.max_information_field_length_rx = params.max_information_field_length_rx;
        ctx.max_information_field_length_tx = params.max_information_field_length_tx;

        auto offset = 0U;
        while ((offset + 1) < frame.payload.size()) {

            uint_fast8_t id = frame.payload[offset++];
            uint_fast8_t len = frame.payload[offset++];
            uint32_t value = 0;

            while (len < 4 && offset < frame.payload.size() && len-- != 0) {
                value <<= 8;
                value |= buffer[offset++];
            }

            switch (id) {
            case 5:
                ctx.max_information_field_length_tx = std::min(params.max_information_field_length_tx, static_cast<uint16_t>(value));
                break;
            case 6:
                ctx.max_information_field_length_rx = std::min(params.max_information_field_length_rx, static_cast<uint16_t>(value));
                break;
            }
        }

        return true;
    }

} //namespace hdlc
} //namespace dlms
