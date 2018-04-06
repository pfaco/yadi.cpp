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
#include <sstream>


namespace yadi
{

/**
 * This class holds the parameters of a hdlc object
 */
struct HdlcParams
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
    uint32_t server_addr = 0x0002FEFF;

    /**
     * Number of bytes in the server address
     */
    uint8_t server_addr_len = 0x04;

    /**
     * Enconded client address
     */
    uint8_t client_addr = 0x03;

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
    Hdlc();
    ~Hdlc();
    HdlcParams& parameters();
    void connect(PhyLayer& phy) override;
    void disconnect(PhyLayer& phy) override;
    void send(PhyLayer& phy, const std::vector<uint8_t>& buffer) override;
    void read(PhyLayer& phy, std::vector<uint8_t>& buffer) override;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

}

#endif /* HDLC_H_ */
