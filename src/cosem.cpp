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

#include <yadi/cosem.h>
#include <yadi/security.h>
#include <yadi/dlms_type.h>
#include <yadi/packer.h>

namespace yadi
{

/**
 * Connection states
 */
enum class ConnectionState
{
    DISCONNECTED, CONNECTED, AUTHENTICATED
};

/**
 * COSEM Constants
 */
enum CosemConstants : unsigned int
{
    BER_CLASS_APPLICATION = 0x40,
    BER_CLASS_CONTEXT = 0x80,
    BER_CONSTRUCTED = 0x20,
    BER_OBJECT_IDENTIFIER = 0x06,
    BER_BIT_STRING = 0x03,
    BER_OCTET_STRING = 0x04,
    BER_BASE = BER_CLASS_CONTEXT | BER_CONSTRUCTED,

    AARQ_APPLICATION_CONTEXT_NAME = 1,
    AARQ_SENDER_ACSE_REQUIREMENTS = 10,
    AARQ_MECHANISM_NAME = 11,
    AARQ_CALLING_AUTHENTICATION_VALUE = 12,
    AARQ_USER_INFORMATION = 30,

    AARE_APPLICATION_1 = 97,
    AARE_APP_CONTEXT_NAME = 1,

    XDLMS_VERSION = 6,
    XDLMS_NO_CIPHERING_INITIATE_REQUEST = 1,
    XDLMS_NO_CIPHERING_INITIATE_RESPONSE = 8,
    XDLMS_NO_CIPHERING_GET_REQUEST = 192,
    XDLMS_NO_CIPHERING_SET_REQUEST = 193,
    XDLMS_NO_CIPHERING_ACTION_REQUEST = 195,
    XDLMS_NO_CIPHERING_GET_RESPONSE = 196,
    XDLMS_NO_CIPHERING_SET_RESPONSE = 197,
    XDLMS_NO_CIPHERING_ACTION_RESPONSE = 199,

    XDLMS_GLOBAL_CIPHERING_INITIATE_REQUEST = 33,
    XDLMS_GLOBAL_CIPHERING_INITIATE_RESPONSE = 40,
    XDLMS_GLOBAL_CIPHERING_GET_REQUEST = 200,
    XDLMS_GLOBAL_CIPHERING_SET_REQUEST = 201,
    XDLMS_GLOBAL_CIPHERING_ACTION_REQUEST = 203,
    XDLMS_GLOBAL_CIPHERING_GET_RESPONSE = 204,
    XDLMS_GLOBAL_CIPHERING_SET_RESPONSE = 205,
    XDLMS_GLOBAL_CIPHERING_ACTION_RESPONSE = 207,

    XDLMS_HIGH_PRIORITY = 128,
    XDLMS_SERVICE_CONFIRMED = 64,
    XDLMS_INVOKE_ID = 1
};

/**
 * Conformance block TAG and bits
 */
enum ConformanceBlock : unsigned int
{
    TAG = 95,
    //READ = 1u << 3u,
    //WRITE = 1u << 4u,
    //UNCONFIRMED_WRITE = 1u << 5u,
    //ATTRIBUTE_0_SUPPORTED_WITH_SET = 1u << 8u,
    //PRIORITY_MGMT_SUPPORTED = 1u << 9u,
    //ATTRIBUTE_0_SUPPORTED_WITH_GET = 1u << 10u,
    BLOCK_TRANSFER_WITH_GET_OR_READ = 1u << 11u,
    BLOCK_TRANSFER_WITH_SET_OR_WRITE = 1u << 12u,
    BLOCK_TRANSFER_WITH_ACTION = 1u << 13u,
    //MULTIPLE_REFERENCES = 1u << 14u,
    //INFORMATION_REPORT = 1u << 15u,
    //PARAMETERIZED_ACCESS = 1u << 18u,
    GET = 1u << 19u,
    SET = 1u << 20u,
    SELECTIVE_ACCESS = 1u << 21u,
    //EVENT_NOTIFICATION = 1u << 22u,
    ACTION = 1u << 23u,
};

/**
 * Holds the connection state variables
 */
struct CosemConnection
{
    ConnectionState state = ConnectionState::DISCONNECTED;

    void reset()
    {
        state = ConnectionState::DISCONNECTED;
    }
};

static const std::vector<uint8_t> empty_vector{0};

/**
 * Implementation of COSEM application layer
 */
class cosem::impl
{
public:
    auto parameters() -> cosem_params&
    {
        return m_params;
    }

    auto rx_buffer() -> std::vector<uint8_t>&
    {
        m_buffer_rx.clear();
        return m_buffer_rx;
    }

    auto connection_request() -> const std::vector<uint8_t>&
    {
        switch (m_connection.state) {
            case ConnectionState::DISCONNECTED:
                m_connection.reset();
                return aarq_frame();
            case ConnectionState::CONNECTED: {
                att_descriptor att(CosemClasses::ASSOCIATION_LN, 1, {0, 0, 40, 0, 0, 255});
                return action_request(att, dlms_type::from_bytes(Security::process_challenger(m_params)));
            }
            default:
                throw cosem_exception("invalid state");
        }
    }

    bool parse_connection_response()
    {
        switch (m_connection.state) {
            case ConnectionState::DISCONNECTED:
                parse_aare_frame();
                m_connection.state = ConnectionState::CONNECTED;
                return m_params.authentication_type < AuthenticationType ::HLS_SHA1;
            case ConnectionState::CONNECTED:
                break;
            case ConnectionState::AUTHENTICATED:
                throw std::logic_error("invalid state");
        }
        return true;
    }

    auto set_request(att_descriptor const& att, std::vector<uint8_t> const& data = empty_vector) -> std::vector<uint8_t>&
    {
        return m_buffer_tx;
    }

    auto get_request(att_descriptor const& att, std::vector<uint8_t> const& data = empty_vector) -> std::vector<uint8_t>&
    {
        return m_buffer_tx;
    }

    auto action_request(att_descriptor const& att, std::vector<uint8_t> const& data = empty_vector) -> std::vector<uint8_t>&
    {
        if (m_params.security_type != SecurityType::NONE) {
            unsigned size = 0;
            m_buffer_tx.push_back(XDLMS_GLOBAL_CIPHERING_ACTION_REQUEST);
            dlms_type::write_size(m_buffer_tx, size);
        }
        m_buffer_tx.push_back(XDLMS_NO_CIPHERING_ACTION_REQUEST);
        m_buffer_tx.push_back(1);
        m_buffer_tx.push_back((uint8_t)(XDLMS_HIGH_PRIORITY | XDLMS_SERVICE_CONFIRMED | XDLMS_INVOKE_ID));
        m_buffer_tx.push_back((uint8_t)(att.class_id() >> 8u));
        m_buffer_tx.push_back((uint8_t) att.class_id());
        m_buffer_tx.insert(m_buffer_tx.end(), att.obis().begin(), att.obis().end());
        m_buffer_tx.push_back(att.index());
        m_buffer_tx.push_back(data.empty() ? 0 : 1);
        m_buffer_tx.insert(m_buffer_tx.end(), data.begin(), data.end());
        return m_buffer_tx;
    }

    auto pack_frame(uint8_t cmd_tag, std::vector<uint8_t> const& payload) -> std::vector<uint8_t>& {
        if (m_params.security_type != SecurityType::NONE) {
            unsigned size = 0;
            m_buffer_tx.push_back(XDLMS_GLOBAL_CIPHERING_ACTION_REQUEST);
            dlms_type::write_size(m_buffer_tx, size);
        }
    }

private:
    cosem_params m_params;
    CosemConnection m_connection;
    std::vector<uint8_t> m_buffer_rx{128};
    std::vector<uint8_t> m_buffer_tx{128};

    /**
     * Generates an AARQ frame, based on the current parameters.
     * The bytes are added to the buffer_tx member variable.
     * @return vector with AARQ frame
     */
    auto aarq_frame() -> std::vector<uint8_t>&
    {
        m_buffer_tx.clear();
        m_buffer_tx.push_back(BER_CONSTRUCTED | BER_CLASS_APPLICATION);

        if (m_params.authentication_type == AuthenticationType::PUBLIC) {
            m_buffer_tx.insert(m_buffer_tx.end(), {0x80, 0x02, 0x07, 0x80});
        }

        // application context name
        update_buffer(7, BER_CLASS_CONTEXT | BER_CONSTRUCTED | AARQ_APPLICATION_CONTEXT_NAME, BER_OBJECT_IDENTIFIER);
        if (m_params.security_type == SecurityType::NONE) {
            m_buffer_tx.insert(m_buffer_tx.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01}); //LNAME_NO_CIPHERING
        } else {
            m_buffer_tx.insert(m_buffer_tx.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x03}); //LNAME_WITH_CIPHERING
        }

        // system title
        if (m_params.security_type != SecurityType::NONE || m_params.authentication_type == AuthenticationType::HLS_GCM) {
            update_buffer(m_params.system_title.size(), BER_BASE | BER_OBJECT_IDENTIFIER, BER_OCTET_STRING);
            m_buffer_tx.insert(m_buffer_tx.end(), m_params.system_title.begin(), m_params.system_title.end());
        }

        // sender ACSE requirements
        if (m_params.authentication_type != AuthenticationType::PUBLIC) {
            m_buffer_tx.push_back(BER_CLASS_CONTEXT | AARQ_SENDER_ACSE_REQUIREMENTS);
            m_buffer_tx.push_back(2);
            m_buffer_tx.push_back(BER_BIT_STRING | BER_OCTET_STRING);
            m_buffer_tx.push_back(0x80);
            m_buffer_tx.push_back(BER_CLASS_CONTEXT | AARQ_MECHANISM_NAME);
            m_buffer_tx.push_back(7);
            m_buffer_tx.insert(m_buffer_tx.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x02});
            m_buffer_tx.push_back(m_params.authentication_type);
            if (m_params.authentication_type == AuthenticationType::LLS) {
                update_buffer(m_params.secret.size(), BER_BASE | AARQ_CALLING_AUTHENTICATION_VALUE, BER_CLASS_CONTEXT);
                m_buffer_tx.insert(m_buffer_tx.end(), m_params.secret.begin(), m_params.secret.end());
            } else {
                update_buffer(m_params.challenger_size, BER_BASE| AARQ_CALLING_AUTHENTICATION_VALUE, BER_CLASS_CONTEXT);
                Security::generate_challenger(m_params.challenger_size, m_buffer_tx);
            }
        }

        // Conformance block
        uint32_t conformance_block = 0;
        conformance_block |= ConformanceBlock::GET;
        conformance_block |= ConformanceBlock::SET;
        conformance_block |= ConformanceBlock::ACTION;
        conformance_block |= ConformanceBlock::SELECTIVE_ACCESS;
        conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_GET_OR_READ;
        conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_SET_OR_WRITE;
        conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_ACTION;

        // Initiate request
        update_buffer(11, BER_CONSTRUCTED | AARQ_USER_INFORMATION, BER_OCTET_STRING);
        m_buffer_tx.push_back(XDLMS_NO_CIPHERING_INITIATE_REQUEST);
        m_buffer_tx.push_back(0); //Dedicated key
        m_buffer_tx.push_back(0); //Response-allowed
        m_buffer_tx.push_back(0); //Proposed quality of service
        m_buffer_tx.push_back(XDLMS_VERSION);
        m_buffer_tx.push_back(ConformanceBlock::TAG);
        m_buffer_tx.push_back(4); //conformance block size
        m_buffer_tx.push_back((uint8_t)conformance_block);
        m_buffer_tx.push_back((uint8_t)(conformance_block >> 8u));
        m_buffer_tx.push_back((uint8_t)(conformance_block >> 16u));
        m_buffer_tx.push_back((uint8_t)(conformance_block >> 24u));

        // Add correct frame size
        m_buffer_tx[1] = (uint8_t)(m_buffer_tx.size() - 2);

        return m_buffer_tx;
    }

    void update_buffer(unsigned size, uint8_t tag1, uint8_t tag2) {
        m_buffer_tx.push_back(tag1);
        m_buffer_tx.push_back((uint8_t)(size + 2));
        m_buffer_tx.push_back(tag2);
        m_buffer_tx.push_back((uint8_t)size);
    }

    /**
     * Verifies the AARE frame received as response from the AARQ sent previously
     * The AARE content must be
     * @return NONE
     */
    void parse_aare_frame() {
        if (m_buffer_rx.size() < 4 || (m_buffer_rx[1] != (m_buffer_rx.size() - 2)) ) {
            throw cosem_exception("Malformed AARE frame");
        }
        if (m_buffer_rx[0] != AARE_APPLICATION_1 || (m_buffer_rx[2] != (BER_CONSTRUCTED | AARE_APP_CONTEXT_NAME))) {
            throw cosem_exception("Malformed AARE frame");
        }
    }
};

cosem::cosem() : m_pimpl{std::make_unique<impl>()} {};
cosem::~cosem() = default;

auto cosem::parameters() -> cosem_params&
{
    return m_pimpl->parameters();
}

auto cosem::rx_buffer() -> std::vector<uint8_t>&{
    return m_pimpl->rx_buffer();
}

auto cosem::connection_request() -> const std::vector<uint8_t>&
{
    return m_pimpl->connection_request();
}

auto cosem::parse_connection_response() -> bool
{
    return m_pimpl->parse_connection_response();
}

}
