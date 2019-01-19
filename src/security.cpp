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

#include "security.h"
#include <yadi/cosem.h>
#include <iterator>
#include <random>
#include <climits>
#include <algorithm>

std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned int> rng;

namespace dlms {

    void Security::generate_challenger(unsigned size, std::vector<uint8_t> &buffer) {
        for (unsigned i = 0; i < size; ++i) {
            buffer.push_back(rng());
        }
    }

    std::vector<uint8_t> Security::process_challenger(CosemParameters const& params) {
        switch (params.authentication) {
            case AuthenticationMechanism::HLS_MD5:
                break;
            case AuthenticationMechanism::HLS_SHA1:
                break;
            case AuthenticationMechanism::HLS_SHA256:
                break;
            case AuthenticationMechanism::HLS_GMAC:
                break;
            default:
                throw std::invalid_argument("invalid authentication type");
        }
		return {};
    }
}
