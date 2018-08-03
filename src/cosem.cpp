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
#include <yadi/parser.h>
#include "security.h"

namespace dlms
{

/**
 * Connection states
 */
enum class ConnectionState
{
    DISCONNECTED,
    CONNECTED,
    AUTHENTICATED
};

/**
 * COSEM Constants
 */
enum CosemConstants : unsigned int
{
    BER_CLASS_APPLICATION = 0x40,
    BER_CLASS_CONTEXT = 0x80,
    BER_CONSTRUCTED = 0x20,
    BER_OBJECT_IDENTIFIER = 0x06,
    BER_BIT_STRING = 0x03,
    BER_OCTET_STRING = 0x04,
    BER_BASE = BER_CLASS_CONTEXT | BER_CONSTRUCTED,

    AARQ_APPLICATION_CONTEXT_NAME = 1,
    AARQ_SENDER_ACSE_REQUIREMENTS = 10,
    AARQ_MECHANISM_NAME = 11,
    AARQ_CALLING_AUTHENTICATION_VALUE = 12,
    AARQ_USER_INFORMATION = 30,

    AARE_APPLICATION_1 = 97,
    AARE_APP_CONTEXT_NAME = 1,

    XDLMS_VERSION = 6,
    XDLMS_NO_CIPHERING_INITIATE_REQUEST = 1,
    XDLMS_NO_CIPHERING_INITIATE_RESPONSE = 8,
    XDLMS_NO_CIPHERING_GET_REQUEST = 192,
    XDLMS_NO_CIPHERING_SET_REQUEST = 193,
    XDLMS_NO_CIPHERING_ACTION_REQUEST = 195,
    XDLMS_NO_CIPHERING_GET_RESPONSE = 196,
    XDLMS_NO_CIPHERING_SET_RESPONSE = 197,
    XDLMS_NO_CIPHERING_ACTION_RESPONSE = 199,

    XDLMS_GLOBAL_CIPHERING_INITIATE_REQUEST = 33,
    XDLMS_GLOBAL_CIPHERING_INITIATE_RESPONSE = 40,
    XDLMS_GLOBAL_CIPHERING_GET_REQUEST = 200,
    XDLMS_GLOBAL_CIPHERING_SET_REQUEST = 201,
    XDLMS_GLOBAL_CIPHERING_ACTION_REQUEST = 203,
    XDLMS_GLOBAL_CIPHERING_GET_RESPONSE = 204,
    XDLMS_GLOBAL_CIPHERING_SET_RESPONSE = 205,
    XDLMS_GLOBAL_CIPHERING_ACTION_RESPONSE = 207,

    XDLMS_HIGH_PRIORITY = 128,
    XDLMS_SERVICE_CONFIRMED = 64,
    XDLMS_INVOKE_ID = 1
};

/**
 * Conformance block TAG and bits
 */
enum ConformanceBlock : unsigned int
{
    TAG = 95,
    //READ = 1u << 3u,
    //WRITE = 1u << 4u,
    //UNCONFIRMED_WRITE = 1u << 5u,
    //ATTRIBUTE_0_SUPPORTED_WITH_SET = 1u << 8u,
    //PRIORITY_MGMT_SUPPORTED = 1u << 9u,
    //ATTRIBUTE_0_SUPPORTED_WITH_GET = 1u << 10u,
    BLOCK_TRANSFER_WITH_GET_OR_READ = 1u << 11u,
    BLOCK_TRANSFER_WITH_SET_OR_WRITE = 1u << 12u,
    BLOCK_TRANSFER_WITH_ACTION = 1u << 13u,
    //MULTIPLE_REFERENCES = 1u << 14u,
    //INFORMATION_REPORT = 1u << 15u,
    //PARAMETERIZED_ACCESS = 1u << 18u,
    GET = 1u << 19u,
    SET = 1u << 20u,
    SELECTIVE_ACCESS = 1u << 21u,
    //EVENT_NOTIFICATION = 1u << 22u,
    ACTION = 1u << 23u,
};

/**
 * Holds the connection state variables
 */
struct CosemConnection
{
    ConnectionState state = ConnectionState::DISCONNECTED;
    void reset() {
        state = ConnectionState::DISCONNECTED;
    }
};

/**
 * Implementation of COSEM application layer
 */
class Cosem::impl
{
    DataTransfer &dtransfer_;
    CosemParameters params_;
    CosemConnection connection_;

public:
    explicit impl(DataTransfer &dtransfer) : dtransfer_{dtransfer} {}

    auto parameters() -> CosemParameters& {
        return params_;
    }

    auto connect() -> AssociationResult {
        connection_.reset();
        dtransfer_.send(aarq_frame());
        parse_aare_frame(dtransfer_.read());
        if (params_.authentication <= AuthenticationMechanism::LLS) {
            return AssociationResult::ACCEPTED;
        }
        auto response = act_request({ClassID::ASSOCIATION_LN, {"0.0.40.0.0.255"}, 1, from_bytes(Security::process_challenger(params_))});
        if (response.result != DataAccessResult::SUCCESS) {
            return AssociationResult::REJECTED_PERMANENT;
        }
        return AssociationResult::ACCEPTED;
    }

    auto get_request(Request const& request) -> Response {
        return {DataAccessResult::SUCCESS,{}};
    }

    auto set_request(Request const& reques) -> Response {
        return {DataAccessResult::SUCCESS,{}};
    }

    auto act_request(Request const& request) -> Response {
        std::vector<uint8_t> buffer;
        if (params_.security != SecurityContext::NONE) {
            unsigned size = 0;
            buffer.push_back(XDLMS_GLOBAL_CIPHERING_ACTION_REQUEST);
            write_size(buffer, size);
        }
        buffer.push_back(XDLMS_NO_CIPHERING_ACTION_REQUEST);
        buffer.push_back(1);
        buffer.push_back((uint8_t)(XDLMS_HIGH_PRIORITY | XDLMS_SERVICE_CONFIRMED | XDLMS_INVOKE_ID));
        buffer.push_back(static_cast<uint16_t>(request.class_id) >> 8u);
        buffer.push_back((uint8_t) request.class_id);
        buffer.insert(buffer.end(), request.logical_name.begin(), request.logical_name.end());
        buffer.push_back(request.index);
        buffer.push_back(request.data.empty() ? 0u : 1u);
        buffer.insert(buffer.end(), request.data.begin(), request.data.end());
        return {DataAccessResult::SUCCESS,{}};
    }

    auto pack_frame(uint8_t cmd_tag, std::vector<uint8_t> const& payload) -> std::vector<uint8_t> {
        std::vector<uint8_t> buffer;
        if (params_.security != SecurityContext::NONE) {
            unsigned size = 0;
            buffer.push_back(XDLMS_GLOBAL_CIPHERING_ACTION_REQUEST);
            write_size(buffer, size);
        }
    }

private:
    /**
     * Generates an AARQ frame, based on the current parameters.
     * The bytes are added to the buffer_tx member variable.
     * @return vector with AARQ frame
     */
    auto aarq_frame() -> std::vector<uint8_t>
    {
        std::vector<uint8_t> buffer;
        buffer.push_back(BER_CONSTRUCTED | BER_CLASS_APPLICATION);

        if (params_.authentication == AuthenticationMechanism::LOWEST) {
            buffer.insert(buffer.end(), {0x80, 0x02, 0x07, 0x80});
        }

        // application context name
        update_buffer(buffer, 7, BER_CLASS_CONTEXT | BER_CONSTRUCTED | AARQ_APPLICATION_CONTEXT_NAME, BER_OBJECT_IDENTIFIER);
        if (params_.security == SecurityContext::NONE) {
            buffer.insert(buffer.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x01}); //LNAME_NO_CIPHERING
        } else {
            buffer.insert(buffer.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x01, 0x03}); //LNAME_WITH_CIPHERING
        }

        // system title
        if (params_.security != SecurityContext::NONE || params_.authentication == AuthenticationMechanism::HLS_GMAC) {
            update_buffer(buffer, params_.system_title.size(), BER_BASE | BER_OBJECT_IDENTIFIER, BER_OCTET_STRING);
            buffer.insert(buffer.end(), params_.system_title.begin(), params_.system_title.end());
        }

        // sender ACSE requirements
        if (params_.authentication != AuthenticationMechanism::LOWEST) {
            buffer.push_back(BER_CLASS_CONTEXT | AARQ_SENDER_ACSE_REQUIREMENTS);
            buffer.push_back(2);
            buffer.push_back(BER_BIT_STRING | BER_OCTET_STRING);
            buffer.push_back(0x80);
            buffer.push_back(BER_CLASS_CONTEXT | AARQ_MECHANISM_NAME);
            buffer.push_back(7);
            buffer.insert(buffer.end(), {0x60, 0x85, 0x74, 0x05, 0x08, 0x02});
            buffer.push_back(static_cast<uint8_t>(params_.authentication));
            if (params_.authentication == AuthenticationMechanism::LLS) {
                update_buffer(buffer, params_.secret.size(), BER_BASE | AARQ_CALLING_AUTHENTICATION_VALUE, BER_CLASS_CONTEXT);
                buffer.insert(buffer.end(), params_.secret.begin(), params_.secret.end());
            } else {
                update_buffer(buffer, params_.challenger_size, BER_BASE| AARQ_CALLING_AUTHENTICATION_VALUE, BER_CLASS_CONTEXT);
                Security::generate_challenger(params_.challenger_size, buffer);
            }
        }

        // Conformance block
        uint32_t conformance_block = 0;
        conformance_block |= ConformanceBlock::GET;
        conformance_block |= ConformanceBlock::SET;
        conformance_block |= ConformanceBlock::ACTION;
        conformance_block |= ConformanceBlock::SELECTIVE_ACCESS;
        conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_GET_OR_READ;
        conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_SET_OR_WRITE;
        conformance_block |= ConformanceBlock::BLOCK_TRANSFER_WITH_ACTION;

        // Initiate request
        update_buffer(buffer, 11, BER_CONSTRUCTED | AARQ_USER_INFORMATION, BER_OCTET_STRING);
        buffer.push_back(XDLMS_NO_CIPHERING_INITIATE_REQUEST);
        buffer.push_back(0); //Dedicated key
        buffer.push_back(0); //Response-allowed
        buffer.push_back(0); //Proposed quality of service
        buffer.push_back(XDLMS_VERSION);
        buffer.push_back(ConformanceBlock::TAG);
        buffer.push_back(4); //conformance block size
        buffer.push_back((uint8_t)conformance_block);
        buffer.push_back((uint8_t)(conformance_block >> 8u));
        buffer.push_back((uint8_t)(conformance_block >> 16u));
        buffer.push_back((uint8_t)(conformance_block >> 24u));

        // Add correct frame size
        buffer[1] = (uint8_t)(buffer.size() - 2);

        return buffer;
    }

    void update_buffer(std::vector<uint8_t> &buffer, unsigned size, uint8_t tag1, uint8_t tag2) {
        buffer.push_back(tag1);
        buffer.push_back((uint8_t)(size + 2));
        buffer.push_back(tag2);
        buffer.push_back((uint8_t)size);
    }

    /**
     * Verifies the AARE frame received as response from the AARQ sent previously
     * The AARE content must be
     * @return NONE
     */
    void parse_aare_frame(std::vector<uint8_t> const& buffer) {
        if (buffer.size() < 4 || (buffer[1] != (buffer.size() - 2)) ) {
            throw std::runtime_error("Malformed AARE frame");
        }
        if (buffer[0] != AARE_APPLICATION_1 || (buffer[2] != (BER_CONSTRUCTED | AARE_APP_CONTEXT_NAME))) {
            throw std::runtime_error("Malformed AARE frame");
        }
    }
};

Cosem::Cosem(DataTransfer &dtransfer) : pimpl_{std::make_unique<impl>(dtransfer)} {};
Cosem::~Cosem() = default;

auto Cosem::parameters() -> CosemParameters& {
    return pimpl_->parameters();
}

auto Cosem::connect() -> AssociationResult {
   return pimpl_->connect();
}

auto Cosem::get_request(Request const& request) -> Response {
    return pimpl_->act_request(request);
}

auto Cosem::set_request(Request const& request) -> Response {
    return pimpl_->act_request(request);
}

auto Cosem::act_request(Request const& request) -> Response {
    return pimpl_->act_request(request);
}

}
