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

#ifndef LINK_LAYER_H_
#define LINK_LAYER_H_

#include <yadi/phy_layer.h>
#include <cstdint>
#include <vector>


namespace yadi {

class LinkLayer
{
public:
    virtual ~LinkLayer(){};
    virtual void connect(PhyLayer &phy) = 0;
    virtual void disconnect(PhyLayer &phy) = 0;
    virtual void send(PhyLayer &phy, const std::vector<uint8_t> &buffer) = 0;
    virtual void read(PhyLayer &phy, std::vector<uint8_t> &buffer) = 0;
};

class LinkLayerException : public std::exception
{
public:
    explicit LinkLayerException(const std::string &str) : m_what{str} {}
    LinkLayerException (const LinkLayerException& other) : m_what(other.m_what) {}
    virtual ~LinkLayerException() throw() {}
    const LinkLayerException& operator=(LinkLayerException) = delete; //disable copy constructors
    virtual const char* what () const throw ()
    {
        return m_what.c_str();
    }

private:
    std::string m_what;
};

}

#endif /* LINK_LAYER_H_ */
