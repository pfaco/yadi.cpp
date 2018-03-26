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

#include "link_layer.h"
#include "phy_layer.h"
#include "cosem.h"

namespace yadi
{

class DlmsClient
{
public:
    explicit DlmsClient(LinkLayer &llayer);
    ~DlmsClient();
    CosemParams& parameters();
    void connect(PhyLayer &phy);
    void disconnect(PhyLayer &phy);
    void get(PhyLayer &phy, AttributeDescriptor &att);
    void set(PhyLayer &phy, AttributeDescriptor &att);
    void action(PhyLayer &phy, AttributeDescriptor &att);

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

}

#endif /* INCLUDE_DLMS_H_ */
