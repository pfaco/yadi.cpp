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

class DlmsClient::impl
{
public:
    explicit impl(LinkLayer &llayer) : m_llayer{llayer} {}
    CosemParams& parameters() { return m_cosem.parameters(); }

    void connect(PhyLayer &phy)
    {
        m_llayer.connect(phy);

        do
        {
            m_llayer.send(phy, m_cosem.connection_request());
            m_llayer.read(phy, m_cosem.rx_buffer());
        }
        while (!m_cosem.parse_connection_response());
    }

    void disconnect(PhyLayer &phy)
    {
        m_llayer.disconnect(phy);
    }

    void get(PhyLayer &phy, AttributeDescriptor &att)
    {

    }

    void set(PhyLayer &phy, AttributeDescriptor &att)
    {

    }

    void action(PhyLayer &phy, AttributeDescriptor &att)
    {

    }

private:
    LinkLayer &m_llayer;
    Cosem m_cosem;
};

DlmsClient::DlmsClient(LinkLayer &llayer) : m_pimpl{std::make_unique<impl>(llayer)} {}

DlmsClient::~DlmsClient() {}

CosemParams& DlmsClient::parameters()
{
    return m_pimpl->parameters();
}

void DlmsClient::connect(PhyLayer &phy)
{
    m_pimpl->connect(phy);
}

void DlmsClient::disconnect(PhyLayer &phy)
{
    m_pimpl->disconnect(phy);
}

void DlmsClient::get(PhyLayer &phy, AttributeDescriptor &att)
{
    m_pimpl->get(phy, att);
}

void DlmsClient::set(PhyLayer &phy, AttributeDescriptor &att)
{
    m_pimpl->get(phy, att);
}

void DlmsClient::action(PhyLayer &phy, AttributeDescriptor &att)
{
    m_pimpl->get(phy, att);
}

}
