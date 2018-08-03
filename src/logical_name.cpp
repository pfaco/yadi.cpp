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

#include <yadi/cosem.h>
#include <algorithm>
#include <sstream>

namespace dlms
{
    class LogicalName::impl
    {
    public:
        std::array<uint8_t,6> value_;

        impl(std::string const& str) {
            std::vector<uint8_t> tokens;
            std::string token;
            std::istringstream tokenStream(str);
            while (std::getline(tokenStream, token, '.')) {
                uint8_t val = static_cast<uint8_t>(std::atoi(token.c_str()));
                tokens.push_back(val);
            }
            if (tokens.size() != 6) {
                throw std::runtime_error{"logical name must be 6 bytes long"};
            }
            std::copy_n(tokens.begin(), 6, value_.begin());
        }

        impl(std::initializer_list<uint8_t> const& initializer_list) {
            if (initializer_list.size() != 6) {
                throw std::runtime_error{"logical name must be 6 bytes long"};
            }
            std::copy_n(initializer_list.begin(), 6, value_.begin());
        }

        std::array<uint8_t,6>::const_iterator begin() const {
            return value_.begin();
        }

        std::array<uint8_t,6>::const_iterator end() const {
            return value_.end();
        }
    };

    LogicalName::LogicalName(std::string const& str) :
            pimpl_{std::make_unique<impl>(str)} {}

    LogicalName::LogicalName(std::initializer_list<uint8_t> const& initializer_list) :
            pimpl_{std::make_unique<impl>(initializer_list)} {}

    LogicalName::~LogicalName() = default;

    std::array<uint8_t,6>::const_iterator LogicalName::begin() const {
        return pimpl_->begin();
    }
    std::array<uint8_t,6>::const_iterator LogicalName::end() const {
        return pimpl_->end();
    }
}
