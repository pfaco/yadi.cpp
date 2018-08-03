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

#include <yadi/interface.h>
#include <memory>

namespace dlms
{

struct WrapperParameters
{
    uint16_t w_port_source = 0x01;
    uint16_t w_port_destination = 0x01;
    uint16_t timeout_millis = 2000;
};

class Wrapper : DataTransfer
{
public:
    Wrapper(DataTransfer &dtransfer);
    ~Wrapper();
    void send(std::vector<uint8_t> const& buffer) override;
    void read() -> std::vector<uint8_t> override;
    auto parameters() -> WrapperParameters&;
private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

}

#endif /* WRAPPER_H_ */
