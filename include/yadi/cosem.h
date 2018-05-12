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

namespace yadi
{

enum AuthenticationType
{
    PUBLIC = 0, LLS = 1, HLS_SHA1 = 2, HLS_SHA256 = 3, HLS_MD5 = 4, HLS_GCM = 5,
};

enum class SecurityType
{
    NONE, AUTHENTICATION, ENCRYPTION, AUTHENTICATION_ENCRYPTION
};

enum class ReferenceType
{
    LOGICAL_NAME, /* TODO SHORT_NAME */
};

enum CosemClasses {
    DATA = 1,
    REGISTER = 3,
    CLOCK = 8,
    ASSOCIATION_LN = 15,
};

struct cosem_params
{
    AuthenticationType authentication_type = AuthenticationType::PUBLIC;
    SecurityType security_type = SecurityType::NONE;
    ReferenceType reference_type = ReferenceType::LOGICAL_NAME;
    std::array<uint8_t,8> system_title{0};
    std::array<uint8_t,8> secret{0};
    std::array<uint8_t,16> ak{0};
    std::array<uint8_t,16> ek{0};
    unsigned challenger_size = 8;
};

class att_descriptor
{
public:
    att_descriptor(uint16_t class_id, uint8_t index, const std::initializer_list<uint8_t> &obis)
            : m_class_id{class_id}, m_index{index}, m_obis{0}
    {
        if (obis.size() != m_obis.size()) {
            throw std::invalid_argument("bad obis size");
        }
        std::copy_n(obis.begin(), m_obis.size(), m_obis.begin());
    }

    auto class_id() const -> uint16_t {
        return m_class_id;
    }

    auto obis() const -> std::array<uint8_t,6> {
        return m_obis;
    }

    auto index() const -> uint8_t {
        return m_index;
    }

    auto response_data() const -> std::vector<uint8_t> {
        return m_response_data;
    }

    void get_request_data(std::vector<uint8_t> &buffer) const {
        buffer.insert(buffer.end(), m_request_data.begin(), m_request_data.end());
    }

    void set_request_data(std::vector<uint8_t> data) {
        m_request_data = std::move(data);
    }

private:
    const uint16_t m_class_id;
    const uint8_t m_index;
    std::array<uint8_t,6> m_obis;
    std::vector<uint8_t> m_request_data;
    std::vector<uint8_t> m_response_data;
};

class cosem
{
public:
    cosem();
    ~cosem();
    auto parameters() -> cosem_params&;
    auto rx_buffer() -> std::vector<uint8_t>&;
    auto connection_request() -> const std::vector<uint8_t>& ;
    bool parse_connection_response();

private:
    class impl;
    std::unique_ptr<impl> m_pimpl;
};

class cosem_exception : public std::exception
{
public:
    explicit cosem_exception(const std::string &str) : m_what{str} {}
    cosem_exception (const cosem_exception& other) : m_what{other.m_what} {}
    virtual ~cosem_exception() throw() {}
    const cosem_exception& operator=(cosem_exception) = delete; //disable copy constructor
    virtual const char* what () const throw ()
    {
        return m_what.c_str();
    }

private:
    std::string m_what;
};

}

#endif /* COSEM_H_ */
