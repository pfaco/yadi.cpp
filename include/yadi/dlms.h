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

#ifndef INCLUDE_DLMS_H_
#define INCLUDE_DLMS_H_

#include <yadi/link_layer.h>
#include <yadi/phy_layer.h>
#include <yadi/cosem.h>

namespace yadi
{

class dlms_client
{
public:
    explicit dlms_client(link_layer &llayer);
    ~dlms_client();
    auto parameters() -> cosem_params&;
    void connect(phy_layer &phy);
    void disconnect(phy_layer &phy);
    void get(phy_layer &phy, att_descriptor &att);
    void set(phy_layer &phy, att_descriptor &att);
    void action(phy_layer &phy, att_descriptor &att);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

}

#endif /* INCLUDE_DLMS_H_ */
