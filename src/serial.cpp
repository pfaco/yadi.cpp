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

#include "serial.h"
#include <serial/serial.h>
#include <chrono>

using namespace std::chrono;

namespace yadi
{

class Serial::impl
{
public:
    explicit impl(const std::string &port_name) : m_serial{port_name} {}

    void send(const std::vector<uint8_t> &buffer)
    {
        m_serial.write(buffer);
        m_serial.flush();
        for (auto l : listeners) {
            l->bytes_sent(buffer);
        }
    }

    void read(std::vector<uint8_t> &buffer, uint16_t timeout_millis, frame_complete_fptr *frame_complete)
    {
        auto time_limit = timeout_millis * milliseconds(1);
        auto start = steady_clock::now();

        do
        {
            size_t available = m_serial.available();
            (void)m_serial.read(buffer, available);

            if (frame_complete(buffer))
            {
                for (auto l : listeners) {
                    l->bytes_read(buffer);
                }

                return;
            }
        }
        while(duration_cast<milliseconds>(steady_clock::now() - start) < time_limit);

        throw PhyLayerException("timeout");
    }

    void add_listener(const std::shared_ptr<PhyLayerListener> &listener)
    {
        listeners.push_back(listener);
    }


private:
    serial::Serial m_serial;
    std::vector<std::shared_ptr<PhyLayerListener>> listeners;
};

Serial::Serial(const std::string &port_name) : m_pimpl{std::make_unique<impl>(port_name)}
{

}

Serial::~Serial() = default;

void Serial::send(const std::vector<uint8_t> &buffer)
{
    m_pimpl->send(buffer);
}

void Serial::read(std::vector<uint8_t> &buffer, uint16_t timeout_millis, frame_complete_fptr *frame_complete)
{
    m_pimpl->read(buffer, timeout_millis, frame_complete);
}

void Serial::add_listener(const std::shared_ptr<PhyLayerListener> &listener)
{
    m_pimpl->add_listener(listener);
}

std::vector<std::string> Serial::port_list()
{
    std::vector<std::string> retval;
    return retval;
}

}
