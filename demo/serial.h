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

#include <yadi/interface.h>
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

class serial : public dlms::DataTransfer
{
private:
    SerialPort serial_;
    std::vector<std::weak_ptr<serial_listener>> listeners_;
public:
    serial(const std::string &port_name) : serial_{port_name} {}
    ~serial() = default;

    void send(const std::vector<uint8_t> &buffer) override {
        serial_.write(buffer);
        for (auto l : listeners_) {
            if (auto li = l.lock()) {
                li->bytes_sent(buffer);
            }
        }
    }

    auto read() -> std::vector<uint8_t> override {
        auto buffer = serial_.read();
        for (auto l : listeners_) {
            if (auto li = l.lock()) {
                li->bytes_read(buffer);
            }
        }
        return buffer;
    }

    void add_listener(std::weak_ptr<serial_listener> listener) {
        listeners_.push_back(std::move(listener));
    }

    auto port() -> SerialPort& {
        return serial_;
    }
};

}

#endif /* SERIAL_H_ */
