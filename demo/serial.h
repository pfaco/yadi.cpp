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

#ifndef SERIAL_H_
#define SERIAL_H_

#include <yadi/phy_layer.h>
#include <ssp/serial.h>
#include <memory.h>

namespace ssp
{

class serial_listener
{
public:
    virtual ~serial_listener() {}
    virtual void bytes_sent(std::vector<uint8_t> const& buffer) = 0;
    virtual void bytes_read(std::vector<uint8_t> const& buffer) = 0;
};

class serial : public dlms::PhyLayer
{
public:
    explicit serial(const std::string &port_name);
    ~serial();
    void send(const std::vector<uint8_t> &buffer) override;
    auto read() -> std::vector<uint8_t> override;
    void add_listener(const std::weak_ptr<serial_listener> &listener);
    auto port() -> serial_port&;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

}

#endif /* SERIAL_H_ */
