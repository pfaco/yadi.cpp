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

#ifndef HDLC_H_
#define HDLC_H_

#include <yadi/link_layer.h>
#include <yadi/phy_layer.h>
#include <sstream>


namespace dlms
{

class client_addr
{
public:
    client_addr(uint8_t value) : m_value{static_cast<uint8_t>((value << 1u) | 0x01)} { };

    auto value() const -> uint8_t {
        return m_value;
    }

private:
    uint8_t m_value;
};

class server_addr
{
public:
    server_addr(uint8_t value) : m_value{static_cast<uint8_t>((value << 1u) | 0x01)}, m_size{1} {};

    auto value() -> uint32_t {
        return m_value;
    }

    auto size() -> unsigned {
        return m_size;
    }

private:
    uint32_t m_value;
    unsigned m_size;
};

/**
 * This class holds the parameters of a hdlc object
 */
struct HdlcParameters
{
    /**
     * The timeout for a response, in milliseconds
     */
    uint16_t timeout_millis = 2000;

    /**
     * Maximum length of transmitted frames
     */
    uint16_t max_information_field_length_tx = 128;

    /**
     * Maximum length of received frames
     */
    uint16_t max_information_field_length_rx = 128;

    /**
     * Enconded server address
     */
    server_addr server_addr = 0x01;//0x0002FEFF;

    /**
     * Enconded client address
     */
    client_addr client_addr = 0x01;

    /**
     * Maximum number of retries
     */
    uint8_t max_retries = 1;
};

/**
 * HDLC class
 */
class Hdlc : public LinkLayer
{
public:
    Hdlc(PhyLayer& phy);
    ~Hdlc();
    auto parameters() -> HdlcParameters& ;
    void set_phy_layer(PhyLayer &phy);
    bool connect();
    bool disconnect();
    void send(std::vector<uint8_t> const& buffer) override;
    auto read() -> std::vector<uint8_t> override;

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

}

#endif /* HDLC_H_ */
