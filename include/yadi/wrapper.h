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

#ifndef WRAPPER_H_
#define WRAPPER_H_

#include <vector>
#include <cstdint>

namespace dlms
{
namespace wrapper
{

struct WrapperParameters
{
    uint16_t w_port_source = 0x01;
    uint16_t w_port_destination = 0x01;
};

auto serialize(const WrapperParameters &params, const std::vector<uint8_t> &data) -> std::vector<uint8_t>;
auto parse(const WrapperParameters &params, const std::vector<uint8_t> &data) -> std::vector<uint8_t>;

} //namespace wrapper
} //namespace dlms

#endif /* WRAPPER_H_ */
