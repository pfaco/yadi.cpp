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

#ifndef COSEM_H_
#define COSEM_H_

#include <vector>
#include <cstdint>
#include <memory>

namespace yadi
{

struct CosemParams
{

};

struct AttributeDescriptor
{
    uint16_t class_id;
    uint16_t index;
    uint8_t obis[6];
};

class Cosem
{
public:
    Cosem();
    ~Cosem();
    CosemParams& parameters();
    std::vector<uint8_t>& connection_request();
    bool parse_connection_response();

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

}

#endif /* COSEM_H_ */
