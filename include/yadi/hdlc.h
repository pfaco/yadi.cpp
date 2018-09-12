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

#include <vector>
#include <cstdint>

namespace dlms
{

/**
 * This class holds the parameters of a hdlc object
 */
struct HdlcParameters
{
    uint16_t max_information_field_length_tx = 128;
    uint16_t max_information_field_length_rx = 128;
    uint8_t server_address_len = 4;
    uint16_t server_logical_address = 1;
    uint16_t server_physical_address = 0x3FFF;
    uint8_t client_address = 1;
};

/**
 * HDLC class
 */
class Hdlc
{
    HdlcParameters params_;
public:
    explicit Hdlc() = default;
    explicit Hdlc(HdlcParameters &params) : params_{params} {}
    auto parameters() -> HdlcParameters& {return params_; }
    auto serialize_snrm() -> std::vector<uint8_t>;
    auto serialize_disc() -> std::vector<uint8_t>;
    auto serialize(std::vector<uint8_t> const& data) -> std::vector<uint8_t>;
    auto parse(std::vector<uint8_t> const& data) -> std::vector<uint8_t>;
};

}

#endif /* HDLC_H_ */
