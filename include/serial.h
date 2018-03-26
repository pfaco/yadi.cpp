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

#include "phy_layer.h"
#include <memory.h>

namespace yadi
{

class Serial : public PhyLayer
{
public:
    static std::vector<std::string> port_list();

    explicit Serial(const std::string &port_name);
    ~Serial();
    void send(const std::vector<uint8_t> &buffer) override;
    void read(std::vector<uint8_t> &buffer, uint16_t timeout_millis, frame_complete_fptr *frame_complete) override;
    void add_listener(const std::shared_ptr<PhyLayerListener> &listener) override;

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

}

#endif /* SERIAL_H_ */
