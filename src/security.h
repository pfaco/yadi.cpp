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

#ifndef YADI_DLMS_SECURITY_H
#define YADI_DLMS_SECURITY_H

#include <yadi/cosem.h>

namespace dlms
{

class Security
{
public:
    static void generate_challenger(unsigned size, std::vector <uint8_t> &buffer);

    static auto process_challenger(CosemParameters const &params) -> std::vector<uint8_t>;
};

}

#endif //YADI_DLMS_SECURITY_H
