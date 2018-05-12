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

#include <yadi/dlms.h>

namespace yadi
{

class dlms_client::impl
{
public:
    explicit impl(link_layer &llayer) : m_llayer{llayer} {};

    auto parameters() -> cosem_params&
    {
        return m_cosem.parameters();
    }

    void connect(phy_layer &phy)
    {
        m_llayer.connect(phy);
        do
        {
            m_llayer.send(phy, m_cosem.connection_request());
            m_llayer.read(phy, m_cosem.rx_buffer());
        }
        while (!m_cosem.parse_connection_response());
    }

    void disconnect(phy_layer &phy)
    {
        m_llayer.disconnect(phy);
    }

    void get(phy_layer &phy, att_descriptor &att)
    {

    }

    void set(phy_layer &phy, att_descriptor &att)
    {

    }

    void action(phy_layer &phy, att_descriptor &att)
    {

    }

private:
    link_layer &m_llayer;
    cosem m_cosem;
};

dlms_client::dlms_client(link_layer &llayer) : m_pimpl{std::make_unique<impl>(llayer)} {}

dlms_client::~dlms_client() = default;

auto dlms_client::parameters() -> cosem_params&
{
    return m_pimpl->parameters();
}

void dlms_client::connect(phy_layer &phy)
{
    m_pimpl->connect(phy);
}

void dlms_client::disconnect(phy_layer &phy)
{
    m_pimpl->disconnect(phy);
}

void dlms_client::get(phy_layer &phy, att_descriptor &att)
{
    m_pimpl->get(phy, att);
}

void dlms_client::set(phy_layer &phy, att_descriptor &att)
{
    m_pimpl->get(phy, att);
}

void dlms_client::action(phy_layer &phy, att_descriptor &att)
{
    m_pimpl->get(phy, att);
}

}
