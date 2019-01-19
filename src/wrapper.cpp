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
namespace wrapper
{

	static const auto WRAPPER_VERSION_MSB = uint8_t{ 0x00 };
	static const auto WRAPPER_VERSION_LSB = uint8_t{ 0x01 };

	static auto wrapper_frame_complete(const std::vector<uint8_t>& buffer) -> bool
	{
		if (buffer.size() < 9) {
			return false;
		}
		auto size = static_cast<uint16_t>((buffer[2] << 8 | buffer[3]) + 8);
		return size == buffer.size();
	}

	auto serialize(const WrapperParameters &params, const std::vector<uint8_t> &data) -> std::vector<uint8_t>
	{
		auto buffer = std::vector<uint8_t>{};
		buffer.clear();
		buffer.push_back(WRAPPER_VERSION_MSB);
		buffer.push_back(WRAPPER_VERSION_LSB);
		buffer.push_back(static_cast<uint8_t>(data.size() >> 8));
		buffer.push_back(static_cast<uint8_t>(data.size()));
		buffer.push_back(static_cast<uint8_t>(params.w_port_destination >> 8));
		buffer.push_back(static_cast<uint8_t>(params.w_port_destination));
		buffer.push_back(static_cast<uint8_t>(params.w_port_source >> 8));
		buffer.push_back(static_cast<uint8_t>(params.w_port_source));
		buffer.insert(buffer.end(), data.begin(), data.end());
		return buffer;
	}

	auto parse(const WrapperParameters &params, const std::vector<uint8_t> &data) -> std::vector<uint8_t>
	{
		if (!wrapper_frame_complete(data)) {
			throw std::runtime_error("wrapper: received invalid data");
		}
		//TODO unpack wrapper frame
		return data;
	}

} //namespace wrapper
} //namespace dlms
