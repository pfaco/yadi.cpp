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

namespace yadi
{

class Cosem::impl
{
public:
    CosemParams& parameters()
    {
        return m_params;
    }

    std::vector<uint8_t>& rx_buffer() {
        m_buffer_rx.clear();
        return m_buffer_rx;
    }

    const std::vector<uint8_t>& connection_request()
    {
        return m_buffer_tx;
    }

    bool parse_connection_response()
    {
        return false;
    }

private:
    CosemParams m_params;
    std::vector<uint8_t> m_buffer_rx{128};
    std::vector<uint8_t> m_buffer_tx{128};
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
