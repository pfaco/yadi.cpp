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

namespace yadi
{

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
        return aarq_request();
    }

    bool parse_connection_response()
    {
        return true;
    }

private:
    CosemParams m_params;
    std::vector<uint8_t> m_buffer_rx{128};
    std::vector<uint8_t> m_buffer_tx{128};

    enum BER
    {
        CLASS_APPLICATION = 0x40,
        CLASS_CONTEXT = 0x80,
        CONSTRUCTED = 0x20,
        OBJECT_IDENTIFIER = 0x06,
        BIT_STRING = 0x03,
        OCTET_STRING = 0x04,
    };

    enum AARQ
    {
        APPLICATION_CONTEXT_NAME = 1,
        SENDER_ACSE_REQUIREMENTS = 10,
        MECHANISM_NAME = 11,
        CALLING_AUTHENTICATION_VALUE = 12,
        USER_INFORMATION = 30,
    };

    std::vector<uint8_t>& aarq_request()
    {
        m_buffer_tx.clear();
        m_buffer_tx.push_back(BER::CLASS_APPLICATION | BER::CONSTRUCTED);

        // TODO
        if (m_params.authentication_type == AuthenticationType::PUBLIC) {
            m_buffer_tx.push_back(0x80);
            m_buffer_tx.push_back(0x02);
            m_buffer_tx.push_back(0x07);
            m_buffer_tx.push_back(0x80);
        }

        // application context name
        update_buffer(get_application_context_name(),
                      BER::CLASS_CONTEXT | BER::CONSTRUCTED | AARQ::APPLICATION_CONTEXT_NAME,
                      BER::OBJECT_IDENTIFIER);

        // system title
        if (m_params.security_type != SecurityType::NONE || m_params.authentication_type == AuthenticationType::HLS_GCM) {
            update_buffer(std::move(m_params.system_title),
                          BER::CLASS_CONTEXT | BER::CONSTRUCTED | BER::OBJECT_IDENTIFIER,
                          BER::OCTET_STRING);
        }

        // sender ACSE requirements
        if (m_params.authentication_type != AuthenticationType::PUBLIC) {
            m_buffer_tx.push_back(BER::CLASS_CONTEXT | AARQ::SENDER_ACSE_REQUIREMENTS);
            m_buffer_tx.push_back(2);
            m_buffer_tx.push_back(BER::BIT_STRING | BER::OCTET_STRING);
            m_buffer_tx.push_back(0x80);
            m_buffer_tx.push_back(BER::CLASS_CONTEXT | AARQ::MECHANISM_NAME);
            m_buffer_tx.push_back(7);
            m_buffer_tx.insert(m_buffer_tx.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x02});
            m_buffer_tx.push_back(m_params.authentication_type);
            update_buffer(Security::generate_challanger(m_params),
                          BER::CLASS_CONTEXT | BER::CONSTRUCTED | AARQ::CALLING_AUTHENTICATION_VALUE,
                          BER::CLASS_CONTEXT);
        }

        // Initiate request
        update_buffer(get_initiate_request(), BER::CONSTRUCTED | AARQ::USER_INFORMATION, BER::OCTET_STRING);

        // Add correct frame size
        m_buffer_tx[1] = m_buffer_tx.size();

        return m_buffer_tx;
    }

    template<typename T>
    void update_buffer(const T &&buffer, uint8_t tag1, uint8_t tag2) {
        m_buffer_tx.push_back(tag1);
        m_buffer_tx.push_back(buffer.size() + 2);
        m_buffer_tx.push_back(tag2);
        m_buffer_tx.push_back(buffer.size());
        for (auto const &b : buffer) {
            m_buffer_tx.push_back(b);
        }
    }

    std::vector<uint8_t> get_application_context_name()
    {
        std::vector<uint8_t> buffer;
        return buffer;
    }

    std::vector<uint8_t> get_initiate_request()
    {
        std::vector<uint8_t> buffer;
        return buffer;
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
