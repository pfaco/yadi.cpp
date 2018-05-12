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

#include <yadi/wrapper.h>

namespace yadi
{

static const uint8_t WRAPPER_VERSION_MSB = 0x00;
static const uint8_t WRAPPER_VERSION_LSB = 0x01;

static auto wrapper_frame_complete(const std::vector<uint8_t>& buffer) -> bool
{
    if (buffer.size() < 9) {
        return false;
    }
    uint16_t size = buffer[2] << 8 | buffer[3];
    size += 8;
    return size == buffer.size();
}

class wrapper::impl
{
public:
    auto parameters() -> wrapper_params&
    {
        return m_params;
    }

    void send(phy_layer& phy, const std::vector<uint8_t>& buffer)
    {
        m_buffer_tx.clear();
        m_buffer_tx.push_back(WRAPPER_VERSION_MSB);
        m_buffer_tx.push_back(WRAPPER_VERSION_LSB);
        m_buffer_tx.push_back(buffer.size() >> 8);
        m_buffer_tx.push_back(buffer.size());
        m_buffer_tx.push_back(m_params.w_port_destination >> 8);
        m_buffer_tx.push_back(m_params.w_port_destination);
        m_buffer_tx.push_back(m_params.w_port_source >> 8);
        m_buffer_tx.push_back(m_params.w_port_source);
        for (uint8_t b : buffer) {
            m_buffer_tx.push_back(b);
        }
        phy.send(m_buffer_tx);
    }

    void read(phy_layer& phy, std::vector<uint8_t>& buffer)
    {
        m_buffer_rx.clear();
        phy.read(m_buffer_rx, m_params.timeout_millis, wrapper_frame_complete);
    }

private:
    wrapper_params m_params;
    std::vector<uint8_t> m_buffer_rx{128};
    std::vector<uint8_t> m_buffer_tx{128};
};

wrapper::wrapper() : m_impl{std::make_unique<impl>()} {}
wrapper::~wrapper() = default;

void wrapper::connect(phy_layer& phy)
{
    //wrapper protocol doesn't have connection
}

void wrapper::disconnect(phy_layer& phy)
{
    //wrapper protocol doesn't have disconnection
}

void wrapper::send(phy_layer& phy, const std::vector<uint8_t>& buffer)
{
    m_impl->send(phy, buffer);
}

void wrapper::read(phy_layer& phy, std::vector<uint8_t>& buffer)
{
    m_impl->read(phy, buffer);
}

auto wrapper::parameters() -> wrapper_params&
{
    return m_impl->parameters();
}

}
