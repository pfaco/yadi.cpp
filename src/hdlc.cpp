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

namespace
{
    enum
    {
        HDLC_FLAG       = 0x7E,
        HDLC_FORMAT     = 0xA0,
        I_CONTROL       = 0x00,
        SNRM_CONTROL    = 0x83,
        DISC_CONTROL    = 0x43,
        UA_CONTROL      = 0x63,
        FRMR_CONTROL    = 0x87
    };
}

namespace dlms
{

/*
 * Private local-global functions
 */
static auto checksequence_calc(std::vector<uint8_t> &data, size_t offset, size_t size) -> uint16_t;
static bool is_frame_complete(const std::vector<uint8_t> &data);

struct HdlcConnection
{
    uint8_t rx_control = 0;
    uint8_t rx_sss = 0;
    uint8_t tx_sss = 0;
    uint8_t rx_rrr = 0;
    uint16_t rx_payload_offset = 0;
    uint16_t rx_payload_size = 0;
    uint16_t max_information_field_length_tx = 128;
    uint16_t max_information_field_length_rx = 128;
    void reset() {
        rx_control = 0;
        rx_sss = 0;
        tx_sss = 0;
        rx_rrr = 0;
        rx_payload_offset = 0;
        rx_payload_size = 0;
    }
};

class Hdlc::impl
{
    DataTransfer &dtransfer_;
    HdlcParameters params_;
    HdlcConnection connection_;

public:

    impl(DataTransfer &dtransfer) : dtransfer_{dtransfer} {}

    void set_phy_layer(DataTransfer &dtransfer) {
        dtransfer_ = dtransfer;
    }

    auto parameters() -> HdlcParameters& {
        return params_;
    }

    bool connect() {
        connection_.rx_sss = 0;
        connection_.tx_sss = 0;
        dtransfer_.send(construct_snrm());
        auto buffer = dtransfer_.read();
        verify_read_data(buffer);
        return parse_snrm_response(buffer);
    }

    bool disconnect() {
        std::vector<uint8_t> buffer_tx;
        frame_init(buffer_tx, DISC_CONTROL, 0);
        frame_close(buffer_tx);
        dtransfer_.send(buffer_tx);
        auto buffer = dtransfer_.read();
        verify_read_data(buffer);
        return connection_.rx_control == UA_CONTROL;
    }

    void send(const std::vector<uint8_t>& data) {
        uint8_t control = I_CONTROL;
        connection_.rx_sss &= 0x07;
        control |= connection_.rx_sss << 5;
        control |= connection_.tx_sss << 1;
        connection_.tx_sss += 1;
        connection_.tx_sss &= 0x07;
        std::vector<uint8_t> buffer;
        frame_init(buffer, control, data.size() + 3);
        connection_.rx_sss += 1;
        buffer.push_back(0xE6);
        buffer.push_back(0xE6);
        buffer.push_back(0);
        frame_update(buffer, data);
        frame_close(buffer);
        dtransfer_.send(buffer);
    }

    auto read() -> std::vector<uint8_t> {
        std::vector<uint8_t> retval;
        auto buffer_rx = dtransfer_.read();
        verify_read_data(buffer_rx);
        size_t offset = connection_.rx_payload_offset;
        if (buffer_rx.size() < (offset + 3)) {
            throw std::runtime_error("HDLC: received invalid LLC bytes");
        }
        if (buffer_rx[offset] != 0xE6 || buffer_rx[offset + 1] != 0xE7 || buffer_rx[offset + 2] != 0x00) {
            throw std::runtime_error("HDLC: received invalid LLC bytes");
        }
        for (size_t i = 3; i < connection_.rx_payload_size; ++i) {
            retval.push_back(buffer_rx[i]);
        }
        return retval;
    }

private:
    void verify_read_data(std::vector<uint8_t> &buffer) {
        uint16_t offset = 0;

        while (buffer[offset] != HDLC_FLAG) {
            offset++;
        }

        if ((buffer[offset+1] & 0xF0u) != HDLC_FORMAT) {
            throw std::runtime_error("HDLC: received invalid frame format");
        }

        uint_fast16_t frame_size = static_cast<uint_fast16_t>(((buffer[1+offset] & 0x0Fu) << 8u) | buffer[2+offset]);
        if (frame_size != buffer.size()-2-offset) {
            throw std::runtime_error("HDLC: received invalid frame format");
        }

        if (params_.client_addr.value() != buffer[offset+3]) {
            throw std::runtime_error("HDLC: received invalid address");
        }

        uint16_t addr_offset = 4;
        while (addr_offset < frame_size && (buffer[offset + addr_offset++] & 0x01u) != 0x01u);
        //if (!Arrays.equals(params.serverAddress, Arrays.copyOfRange(data, 4, offset))) {
        //  throw new link_layer_exception(LinkLayerExceptionReason.RECEIVED_INVALID_ADDRESS);
        //}

        connection_.rx_control = buffer[offset + addr_offset++];
        if (buffer.size() - offset - addr_offset > 3)
        {
            connection_.rx_payload_offset = static_cast<uint16_t>(offset + addr_offset + 2u);
            connection_.rx_payload_size = static_cast<uint16_t>(buffer.size() - offset - 3u);
        } else {
            connection_.rx_payload_offset = 0;
            connection_.rx_payload_size = 0;
        }

        uint_fast16_t fcs = buffer[offset + addr_offset +1];
        fcs <<= 8;
        fcs |= buffer[offset + addr_offset];
        if (fcs != checksequence_calc(buffer, offset+1, addr_offset-1u)) {
            throw std::runtime_error("HDLC: received invalid check sequence");
        }

        fcs = buffer[buffer.size()-2];
        fcs <<= 8;
        fcs |= buffer[buffer.size()-3];
        if (fcs != checksequence_calc(buffer, offset+1, buffer.size()-4-offset)) {
            throw std::runtime_error("HDLC: received invalid check sequence");
        }

        if ( (connection_.rx_control & 0x10u) != 0x10) {
            throw std::runtime_error("HDLC: segmentation not supported");
        }

        connection_.rx_control &= 0xEF; //remove p/f bit from control
        connection_.rx_rrr = 0;
        connection_.rx_sss = 0;

        if ((connection_.rx_control & 0x01u) == 0x00) {
            connection_.rx_rrr = static_cast<uint8_t>((connection_.rx_control >> 5u) & 0x07u);
            connection_.rx_sss = static_cast<uint8_t>((connection_.rx_control >> 1u) & 0x07u);
            connection_.rx_control &= 0x01; //remove sss and rrr bits from control
        } else if ((connection_.rx_control & 0x02u) == 0x00) {
            connection_.rx_rrr = static_cast<uint8_t>((connection_.rx_control >> 5u) & 0x07u);
            connection_.rx_control &= 0x0F; //remove rrr bits from control
        }

        if (connection_.rx_control == FRMR_CONTROL) {
            throw std::runtime_error("HDLC: frame rejected");
        }
    }

    void frame_init(std::vector<uint8_t> &buffer, uint8_t control_byte, size_t size) {
        control_byte |= 0x10u; //always final
        buffer.push_back(HDLC_FLAG);
        buffer.push_back(HDLC_FORMAT);
        buffer.push_back(0); //size
        for (size_t i = 0; i < params_.server_addr.size(); ++i) {
            buffer.push_back(params_.server_addr.value() >> ((params_.server_addr.size()-i-1)*8)); //TODO
        }
        buffer.push_back(params_.client_addr.value());
        buffer.push_back(control_byte);
        if (size > 0) {
            buffer.push_back(0); //header hcs
            buffer.push_back(0); //header hcs
        }
    }

    void frame_update(std::vector<uint8_t> &buffer, std::vector<uint8_t> const& data) {
        buffer.insert(buffer.end(), data.begin(), data.end());
    }

    void frame_close(std::vector<uint8_t> &buffer) {
        size_t address_offset = params_.server_addr.size() + 1;
        buffer.push_back(0); //fcs
        buffer.push_back(0); //fcs
        buffer.push_back(HDLC_FLAG);
        buffer[1] = HDLC_FORMAT | (uint8_t)((buffer.size() - 2) >> 8);
        buffer[2] = (uint8_t)(buffer.size() - 2);
        uint_fast16_t fcs = checksequence_calc(buffer, 1, address_offset + 3);
        buffer[4 + address_offset] = (uint8_t)fcs;
        buffer[5 + address_offset] = (uint8_t)(fcs >> 8);
        if (buffer.size() > (address_offset + 7)) {
            fcs = checksequence_calc(buffer, 1, buffer.size() - 4);
            buffer[buffer.size() - 3] = (uint8_t)fcs;
            buffer[buffer.size() - 2] = (uint8_t)(fcs >> 8);
        }
    }

    /**
     * Populates the buffer with the payload of a Set Normal Response Mode (SNRM) frame
     * In this version it is not supported window size (rx or tx) different of 1, so those
     * bytes are never generated. The bytes for settings the max_info_len parameters are only
     * generated if those are different from the default (128).
     */
    auto construct_snrm() -> std::vector<uint8_t> {
        std::vector<uint8_t> temp_buffer;
        if (params_.max_information_field_length_tx < 128) {
            temp_buffer.push_back(0x05);
            temp_buffer.push_back(1);
            temp_buffer.push_back(params_.max_information_field_length_tx);
        } else if (params_.max_information_field_length_tx > 128) {
            temp_buffer.push_back(0x05);
            temp_buffer.push_back(2);
            temp_buffer.push_back(params_.max_information_field_length_tx >> 8);
            temp_buffer.push_back(params_.max_information_field_length_tx);
        }
        if (params_.max_information_field_length_rx < 128) {
            temp_buffer.push_back(0x06);
            temp_buffer.push_back(1);
            temp_buffer.push_back(params_.max_information_field_length_rx);
        } else if (params_.max_information_field_length_rx > 128) {
            temp_buffer.push_back(0x06);
            temp_buffer.push_back(2);
            temp_buffer.push_back(params_.max_information_field_length_rx >> 8);
            temp_buffer.push_back(params_.max_information_field_length_rx);
        }
        std::vector<uint8_t> buffer;
        frame_init(buffer, SNRM_CONTROL, temp_buffer.size());
        frame_update(buffer, temp_buffer);
        frame_close(buffer);
        return buffer;
    }

    bool parse_snrm_response(std::vector<uint8_t> &buffer) {
        size_t offset = connection_.rx_payload_offset;
        size_t size = connection_.rx_payload_size;
        if (size == 0) {
            return false;
        }
        if (size < 5 || buffer[offset] != 0x81 || buffer[offset+1] != 0x80) {
            return false;
        }
        if (connection_.rx_control != UA_CONTROL) {
            return false;
        }
        offset += 3;
        while (offset < size) {
            uint_fast8_t id = buffer[offset++];
            uint_fast8_t len = buffer[offset++];
            uint32_t value = 0;
            while (len < 4 && offset < size && len-- != 0) {
                value <<= 8;
                value |= buffer[offset++];
            }
            switch (id) {
            case 5:
                connection_.max_information_field_length_tx = std::min(params_.max_information_field_length_tx, (uint16_t)value);
                break;
            case 6:
                connection_.max_information_field_length_rx = std::min(params_.max_information_field_length_rx, (uint16_t)value);
                break;
            }
        }
        return true;
    }

};

Hdlc::Hdlc(DataTransfer &dtransfer) : pimpl_{std::make_unique<impl>(dtransfer)} {}
Hdlc::~Hdlc() = default;

void Hdlc::set_phy_layer(DataTransfer &dtransfer) {
    pimpl_->set_phy_layer(dtransfer);
}

auto Hdlc::parameters() -> HdlcParameters&  {
    return pimpl_->parameters();
}

bool Hdlc::connect() {
    return pimpl_->connect();
}

bool Hdlc::disconnect() {
    return pimpl_->disconnect();
}

void Hdlc::send(const std::vector<uint8_t> &buffer) {
    pimpl_->send(buffer);
}

auto Hdlc::read() -> std::vector<uint8_t>
{
    return pimpl_->read();
}

static bool is_frame_complete(const std::vector<uint8_t> &data) {
    size_t offset = 0;
    size_t size = data.size();
    while (offset < size && data[offset] != HDLC_FLAG) {
        ++offset;
    }
    if(size < (offset + 9) || data[offset] != HDLC_FLAG || data[size - 1] != HDLC_FLAG) {
        return false;
    }
    if(size  < (((data[offset + 1] & 0x07) << 8u ) + data[offset + 2] + 2u)) {
        return false;
    }
    return true;
}

static auto checksequence_calc(std::vector<uint8_t> &data, size_t offset, size_t size) -> uint16_t  {
    static const uint16_t table[] =  {
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

    uint_fast16_t fcs = 0xffff;
    while (size--) {
        fcs = (fcs >> 8u) ^ table[(fcs ^ data[offset++]) & 0xff];
    }
    return ~fcs;
}

}
