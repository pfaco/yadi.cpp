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

    auto emode_serialize(EmodeBaud baud) -> std::vector<uint8_t>;
    auto emode_parse(std::vector<uint8_t>) -> EmodeBaud ;
}

#endif /* EMODE_H_ */
