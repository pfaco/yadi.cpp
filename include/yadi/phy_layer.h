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

#ifndef PHY_LAYER_H_
#define PHY_LAYER_H_

#include <cstdint>
#include <vector>
#include <memory>
#include <exception>
#include <sstream>
#include <string>

namespace yadi
{

class phy_layer_listener
{
public:
    virtual ~phy_layer_listener() {}
    virtual void bytes_sent(std::vector<uint8_t> const& buffer) = 0;
    virtual void bytes_read(std::vector<uint8_t> const& buffer) = 0;
};

typedef bool frame_complete_fptr(std::vector<uint8_t> const& data);

class phy_layer
{
public:
    virtual ~phy_layer() = default;
    virtual void send(std::vector<uint8_t> const& buffer) = 0;
    virtual void read(std::vector<uint8_t> &buffer, uint16_t timeout_millis, frame_complete_fptr *frame_complete) = 0;
    virtual void add_listener(const std::shared_ptr<phy_layer_listener>& listener) = 0;
};

class phy_layer_exception : public std::exception
{
public:
    explicit phy_layer_exception(std::string const& str) : m_what{str} {}
    phy_layer_exception (const phy_layer_exception& other) : m_what{other.m_what} {}
    virtual ~phy_layer_exception() throw() {}
    phy_layer_exception const& operator=(phy_layer_exception) = delete; //disable copy constructor
    virtual const char* what () const throw ()
    {
        return m_what.c_str();
    }

private:
    std::string m_what;
};

}

#endif /* PHY_LAYER_H_ */
