//
// Created by Paulo on 9/15/2018.
//

#include "catch.hpp"
#include "yadi/cosem.h"
#include <iostream>
#include <iomanip>

TEST_CASE( "AARQ is correctly serialized for LLS and no Security", "[serialize_aarq]") {
    static std::vector<uint8_t> expected_aarq = {0x60, 0x39, 0x80, 0x02, 0x07, 0x80, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85,
                                                 0x74, 0x05, 0x08, 0x01, 0x01, 0x8A, 0x02, 0x07, 0x80, 0x8B, 0x07, 0x60,
                                                 0x85, 0x74, 0x05, 0x08, 0x02, 0x01, 0xAC, 0x0A, 0x80, 0x08, 0x31, 0x32,
                                                 0x33, 0x34, 0x35, 0x36, 0x37, 0x38, 0xBE, 0x0F, 0x04, 0x0D, 0x01, 0x00,
                                                 0x00, 0x00, 0x06, 0x5F, 0x04, 0x00, 0xB8, 0x38, 0x00, 0xFF, 0xFF};

    dlms::CosemContext context;
    dlms::CosemParameters params;
    dlms::Cosem cosem{};

    cosem.parameters.authentication = dlms::AuthenticationMechanism::LLS;
    cosem.parameters.security = dlms::SecurityContext::NONE;
    cosem.parameters.secret = {0x31, 0x32, 0x33, 0x34, 0x35, 0x36, 0x37, 0x38};

    REQUIRE (dlms::serialize_aarq(cosem) == expected_aarq);
}

TEST_CASE( "AARQ is correctly serialized for Public and no Security", "[serialize_aarq]") {
    static std::vector<uint8_t> expected_aarq = {0x60, 0x20, 0x80, 0x02, 0x07, 0x80, 0xA1, 0x09, 0x06, 0x07, 0x60, 0x85,
                                                 0x74, 0x05, 0x08, 0x01, 0x01, 0xBE, 0x0F, 0x04, 0x0D, 0x01, 0x00, 0x00,
                                                 0x00, 0x06, 0x5F, 0x04, 0x00, 0xB8, 0x38, 0x00, 0xFF, 0xFF};

    dlms::CosemContext context;
    dlms::CosemParameters params;
    dlms::Cosem cosem{};

    cosem.parameters.authentication = dlms::AuthenticationMechanism::LOWEST;
    cosem.parameters.security = dlms::SecurityContext::NONE;

    REQUIRE (dlms::serialize_aarq(cosem) == expected_aarq);
}

TEST_CASE( "Get-Request is correctly serialized", "serialize_get") {
    static std::vector<uint8_t> expected_get = {0xC0, 0x01, 0xC1, 0x00, 0x01, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x02,
                                                0x00};

    dlms::CosemContext context;
    dlms::CosemParameters params;
    dlms::Cosem cosem{};
    cosem.parameters.security = dlms::SecurityContext::NONE;
    dlms::Request request = {dlms::ClassID::DATA, {"1.0.1.8.0.255"}, 2, {}};

    REQUIRE (dlms::serialize_get_request(cosem, request) == expected_get);
}

TEST_CASE( "Set-Request is correctly serialized", "serialize_set") {
    static std::vector<uint8_t> expected_set = {0xC1, 0x01, 0xC1, 0x00, 0x01, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF, 0x02,
                                                0x00, 0x06, 0x0A, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88,
                                                0x99};

    dlms::CosemContext context;
    dlms::CosemParameters params;
    dlms::Cosem cosem{};
    cosem.parameters.security = dlms::SecurityContext::NONE;
    dlms::Request request = {dlms::ClassID::DATA, {"1.0.1.8.0.255"}, 2, {0x06, 0x0A, 0x00, 0x11, 0x22, 0x33, 0x44, 0x55,
                                                                         0x66, 0x77, 0x88, 0x99}};

    REQUIRE (dlms::serialize_set_request(cosem, request) == expected_set);
}

TEST_CASE( "Action-Request is correctly serialized", "serialize_act") {
    static std::vector<uint8_t> expected_act = {0xC3, 0x01, 0xC1, 0x00, 0x01, 0x01, 0x00, 0x01, 0x08, 0x00, 0xFF,
                                                0x02, 0x00 };

    dlms::CosemContext context;
    dlms::CosemParameters params;
    dlms::Cosem cosem{};
    cosem.parameters.security = dlms::SecurityContext::NONE;
    dlms::Request request = {dlms::ClassID::DATA, {"1.0.1.8.0.255"}, 2, {}};

    REQUIRE (dlms::serialize_action_request(cosem, request) == expected_act);
}
