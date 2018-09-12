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
#include <stdexcept>

namespace dlms
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

void Wrapper::serialize(std::vector<uint8_t> const& data) -> std::vector<uint8_t> {
    std::vector<uint8_t> buffer;
    buffer.clear();
    buffer.push_back(WRAPPER_VERSION_MSB);
    buffer.push_back(WRAPPER_VERSION_LSB);
    buffer.push_back(static_cast<uint8_t>(data.size() >> 8));
    buffer.push_back(static_cast<uint8_t>(data.size()));
    buffer.push_back(static_cast<uint8_t>(params_.w_port_destination >> 8));
    buffer.push_back(static_cast<uint8_t>(params_.w_port_destination));
    buffer.push_back(static_cast<uint8_t>(params_.w_port_source >> 8));
    buffer.push_back(static_cast<uint8_t>(params_.w_port_source));
    buffer.insert(buffer.end(), data.begin(), data.end());
    return buffer;
}

auto read(std::vector<uint8_t> const& data) -> std::vector<uint8_t>
{
    if (!wrapper_frame_complete(data)) {
        throw std::runtime_error("wrapper: received invalid data");
    }
    //TODO unpack wrapper frame
    return data;
}

}
