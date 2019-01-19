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

#ifndef EMODE_H_
#define EMODE_H_

#include <vector>
#include <cstdint>
#include <exception>

namespace dlms
{

    enum class EmodeBaud : char {
          _300 = '0',
          _600 = '1',
         _1200 = '2',
         _2400 = '3',
         _4800 = '4',
         _9600 = '5',
        _19200 = '6'
    };

    struct EmodeError : public std::exception
    {
        const char* what() const noexcept override {
            return "emode error";
        }
    };

    template<typename T>
    EmodeBaud emode_process(T& serial, EmodeBaud desired_baudrate)
    {
        auto ask_baud_frame = std::vector<uint8_t>{0x2f, 0x3f, 0x21, 0x0d, 0x0a}; //"/?!\r\n"
        auto accept_baud_frame = std::vector<uint8_t>{0x06, 0x32, 0x00, 0x32, 0x0D, 0x0A};

        serial.write(ask_baud_frame);
        auto data = serial.read();
        if (data.size() < 15 || data[data.size()-2] != 0x0D || data[data.size()-1] != 0x0A) {
            throw EmodeError{};
        }
        serial.write(accept_baud_frame);
        (void)serial.read();
    }

}

#endif /* EMODE_H_ */
