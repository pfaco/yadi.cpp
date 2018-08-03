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

#ifndef YADI_DLMS_INTERFACE_H
#define YADI_DLMS_INTERFACE_H

#include <cstdint>
#include <vector>

namespace dlms
{

    class DataTransfer {
    public:
        virtual ~DataTransfer() {}
        virtual void send(std::vector<uint8_t> const& data) = 0;
        virtual auto read() -> std::vector<uint8_t> = 0;
    };

}

#endif //YADI_DLMS_INTERFACE_H
