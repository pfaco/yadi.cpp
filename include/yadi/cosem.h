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

#ifndef COSEM_H_
#define COSEM_H_

#include <vector>
#include <cstdint>
#include <memory>
#include <algorithm>
#include <utility>
#include <yadi/link_layer.h>

namespace dlms
{

enum class AuthenticationMechanism : uint8_t  {
    LOWEST = 0,
    LLS = 1,
    HLS = 2,
    HLS_MD5 = 3,
    HLS_SHA1 = 4,
    HLS_GMAC = 5,
    HLS_SHA256 = 6,
    HLS_ECDSA = 7
};

enum class SecurityContext {
    NONE,
    AUTHENTICATION,
    ENCRYPTION,
    AUTHENTICATION_ENCRYPTION
};

enum class AssociationResult : uint8_t {
    ACCEPTED = 0,
    REJECTED_PERMANENT = 1,
    REJECTED_TRANSIENT = 2
};

enum class DataAccessResult : uint8_t {
    SUCCESS = 0,
    HARDWARE_FAULT = 1,
    TEMPORARY_FAILURE = 2,
    READ_WRITE_DENIED = 3,
    OBJECT_UNDEFINED = 4,
    OBJECT_CLASS_INCONSISTENT = 9,
    OBJECT_UNAVAILABLE = 11,
    TYPE_UNMATCHED = 12,
    SCOPE_OF_ACCESS_VIOLATED = 13,
    DATA_BLOCK_UNAVAILABLE = 14,
    LONG_GET_ABORTED = 15,
    NO_LONG_GET_IN_PROGRESS = 16,
    LONG_SET_ABORTED = 17,
    NO_LONG_SET_IN_PROGRESS = 18,
    DATA_BLOCK_NUMBER_INVALID = 19,
    OTHER_REASON = 250
};

enum class ClassID : uint16_t {
    DATA = 1,
    REGISTER = 3,
    CLOCK = 8,
    ASSOCIATION_LN = 15,
};

struct CosemParameters {
    AuthenticationMechanism authentication = AuthenticationMechanism::LOWEST;
    SecurityContext security = SecurityContext::NONE;
    std::array<uint8_t,8> system_title{0};
    std::array<uint8_t,8> secret{0};
    std::array<uint8_t,16> ak{0};
    std::array<uint8_t,16> ek{0};
    unsigned challenger_size = 8;
};

class LogicalName {
public:
    LogicalName(std::string const& str);
    LogicalName(std::initializer_list<uint8_t> const& initializer_list);
    std::iterator begin();
    std::iterator end();
private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

class Cosem {
public:
    Cosem(LinkLayer &link);
    ~Cosem();
    void set_link_layer(LinkLayer &link);
    auto parameters() -> CosemParameters&;
    auto connect() -> AssociationResult;
    void disconnect();
    auto get_request(ClassID class_id, LogicalName logical_name, uint8_t index, std::vector<uint8_t> data = {}) -> std::pair<DataAccessResult,std::vector<uint8_t>>;
    auto set_request(ClassID class_id, LogicalName logical_name, uint8_t index, std::vector<uint8_t> data = {}) -> std::pair<DataAccessResult,std::vector<uint8_t>>;
    auto act_request(ClassID class_id, LogicalName logical_name, uint8_t index, std::vector<uint8_t> data = {}) -> std::pair<DataAccessResult,std::vector<uint8_t>>;

private:
    class impl;
    std::unique_ptr<impl> pimpl_;
};

}

#endif /* COSEM_H_ */
