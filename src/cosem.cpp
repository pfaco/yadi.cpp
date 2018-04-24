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
#include <yadi/parser.h>

namespace yadi
{

/**
 * Connection states
 */
enum class ConnectionState {
    DISCONNECTED, CONNECTED, AUTHENTICATED
};

/**
 * BER encoded values
 */
enum BER
{
    CLASS_APPLICATION = 0x40,
    CLASS_CONTEXT = 0x80,
    CONSTRUCTED = 0x20,
    OBJECT_IDENTIFIER = 0x06,
    BER_BIT_STRING = 0x03,
    BER_OCTET_STRING = 0x04,
    BASE = CLASS_CONTEXT | CONSTRUCTED,
};

/**
 * Application-Association Request constants
 */
enum AARQ
{
    APPLICATION_CONTEXT_NAME = 1,
    SENDER_ACSE_REQUIREMENTS = 10,
    MECHANISM_NAME = 11,
    CALLING_AUTHENTICATION_VALUE = 12,
    USER_INFORMATION = 30,
};

/**
 * Application-Association Response constants
 */
enum AARE
{
    APPLICATION_1 = 97,
    APP_CONTEXT_NAME = 1,
};

/**
 * xDLMS constants
 */
enum XDLMS
{
    INITIATE_REQUEST = 1,
    VERSION = 6,
};

/**
 * Conformance block TAG and bits
 */
enum ConformanceBlock
{
    TAG = 95,
    READ = 1 << 3,
    WRITE = 1 << 4,
    UNCONFIRMED_WRITE = 1 << 5,
    ATTRIBUTE_0_SUPPORTED_WITH_SET = 1 << 8,
    PRIORITY_MGMT_SUPPORTED = 1 << 9,
    ATTRIBUTE_0_SUPPORTED_WITH_GET = 1 << 10,
    BLOCK_TRANSFER_WITH_GET_OR_READ = 1 << 11,
    BLOCK_TRANSFER_WITH_SET_OR_WRITE = 1 << 12,
    BLOCK_TRANSFER_WITH_ACTION = 1 << 13,
    MULTIPLE_REFERENCES = 1 << 14,
    INFORMATION_REPORT = 1 << 15,
    PARAMETERIZED_ACCESS = 1 << 18,
    GET = 1 << 19,
    SET = 1 << 20,
    SELECTIVE_ACCESS = 1 << 21,
    EVENT_NOTIFICATION = 1 << 22,
    ACTION = 1 << 23,
};

/**
 * Holds the connection state variables
 */
struct CosemConnection
{
    ConnectionState state = ConnectionState::DISCONNECTED;

    void reset() {
        state = ConnectionState::DISCONNECTED;
    }
};

/**
 * Implementation of COSEM application layer
 */
class Cosem::impl
{
public:
    CosemParams& parameters()
    {
        return m_params;
    }

    std::vector<uint8_t>& rx_buffer()
    {
        m_buffer_rx.clear();
        return m_buffer_rx;
    }

    const std::vector<uint8_t>& connection_request()
    {
        switch (m_connection.state) {
            case ConnectionState::DISCONNECTED:
                m_connection.reset();
                return aarq_frame();
            case ConnectionState::CONNECTED: {
                //AttributeDescriptor att(CosemClasses::ASSOCIATION_LN, 1, {0, 0, 40, 0, 0, 255});
                //TODO att.set_request_data(Parser::pack_octet_string(Security::processChallenger(m_params, m_connection)));
                //TODO return action_request(att);
                return aarq_frame();
            }
            default:
                throw CosemException("invalid state");
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
        }
        return true;
    }

private:
    CosemParams m_params;
    CosemConnection m_connection;
    std::vector<uint8_t> m_buffer_rx{128};
    std::vector<uint8_t> m_buffer_tx{128};

    /**
     * Generates an AARQ frame, based on the current parameters.
     * The bytes are added to the buffer_tx member variable.
     * @return vector with AARQ frame
     */
    std::vector<uint8_t>& aarq_frame()
    {
        m_buffer_tx.clear();
        m_buffer_tx.push_back(BER::CONSTRUCTED | BER::CLASS_APPLICATION);

        if (m_params.authentication_type == AuthenticationType::PUBLIC) {
            m_buffer_tx.insert(m_buffer_tx.end(), {0x80, 0x02, 0x07, 0x80});
        }

        // application context name
        update_buffer(7, BER::CLASS_CONTEXT | BER::CONSTRUCTED | AARQ::APPLICATION_CONTEXT_NAME, BER::OBJECT_IDENTIFIER);
        if (m_params.security_type == SecurityType::NONE) {
            m_buffer_tx.insert(m_buffer_tx.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01}); //LNAME_NO_CIPHERING
        } else {
            m_buffer_tx.insert(m_buffer_tx.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x03}); //LNAME_WITH_CIPHERING
        }

        // system title
        if (m_params.security_type != SecurityType::NONE || m_params.authentication_type == AuthenticationType::HLS_GCM) {
            update_buffer(m_params.system_title.size(), BER::BASE | BER::OBJECT_IDENTIFIER, BER::BER_OCTET_STRING);
            m_buffer_tx.insert(m_buffer_tx.end(), m_params.system_title.begin(), m_params.system_title.end());
        }

        // sender ACSE requirements
        if (m_params.authentication_type != AuthenticationType::PUBLIC) {
            m_buffer_tx.push_back(BER::CLASS_CONTEXT | AARQ::SENDER_ACSE_REQUIREMENTS);
            m_buffer_tx.push_back(2);
            m_buffer_tx.push_back(BER::BER_BIT_STRING | BER::BER_OCTET_STRING);
            m_buffer_tx.push_back(0x80);
            m_buffer_tx.push_back(BER::CLASS_CONTEXT | AARQ::MECHANISM_NAME);
            m_buffer_tx.push_back(7);
            m_buffer_tx.insert(m_buffer_tx.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x02});
            m_buffer_tx.push_back(m_params.authentication_type);
            if (m_params.authentication_type == AuthenticationType::LLS) {
                update_buffer(m_params.secret.size(), BER::BASE | AARQ::CALLING_AUTHENTICATION_VALUE, BER::CLASS_CONTEXT);
                m_buffer_tx.insert(m_buffer_tx.end(), m_params.secret.begin(), m_params.secret.end());
            } else {
                update_buffer(m_params.challenger_size, BER::BASE| AARQ::CALLING_AUTHENTICATION_VALUE, BER::CLASS_CONTEXT);
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
        update_buffer(11, BER::CONSTRUCTED | AARQ::USER_INFORMATION, BER::BER_OCTET_STRING);
        m_buffer_tx.push_back(XDLMS::INITIATE_REQUEST);
        m_buffer_tx.push_back(0); //Dedicated key
        m_buffer_tx.push_back(0); //Response-allowed
        m_buffer_tx.push_back(0); //Proposed quality of service
        m_buffer_tx.push_back(XDLMS::VERSION);
        m_buffer_tx.push_back(ConformanceBlock::TAG);
        m_buffer_tx.push_back(4); //conformance block size
        m_buffer_tx.push_back((uint8_t)conformance_block);
        m_buffer_tx.push_back((uint8_t)(conformance_block >> 8));
        m_buffer_tx.push_back((uint8_t)(conformance_block >> 16));
        m_buffer_tx.push_back((uint8_t)(conformance_block >> 24));

        // Add correct frame size
        m_buffer_tx[1] = m_buffer_tx.size() - 2;

        return m_buffer_tx;
    }

    void update_buffer(unsigned size, uint8_t tag1, uint8_t tag2) {
        m_buffer_tx.push_back(tag1);
        m_buffer_tx.push_back(size + 2);
        m_buffer_tx.push_back(tag2);
        m_buffer_tx.push_back(size);
    }

    void parse_aare_frame() {
        if (m_buffer_rx.size() < 4 || (m_buffer_rx[1] != (m_buffer_rx.size() - 2)) ) {
            throw new CosemException("Malformed AARE frame");
        }
        if (m_buffer_rx[0] != AARE::APPLICATION_1 || (m_buffer_rx[2] != (BER::CONSTRUCTED | AARE::APP_CONTEXT_NAME))) {
            throw new CosemException("Malformed AARE frame");
        }

    }
};

Cosem::Cosem() : m_pimpl{std::make_unique<impl>()} {}
Cosem::~Cosem() {}

CosemParams& Cosem::parameters()
{
    return m_pimpl->parameters();
}

std::vector<uint8_t>& Cosem::rx_buffer() {
    return m_pimpl->rx_buffer();
}

const std::vector<uint8_t>& Cosem::connection_request()
{
    return m_pimpl->connection_request();
}

bool Cosem::parse_connection_response()
{
    return m_pimpl->parse_connection_response();
}

}
