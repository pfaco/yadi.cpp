//
// Created by Paulo on 9/15/2018.
//

#include "catch.hpp"
#include "yadi/hdlc.h"
#include <iostream>
#include <iomanip>

TEST_CASE( "SNRM is correctly serialized", "[serialize_snrm]") {
    static std::vector<uint8_t> expected_snrm = {0x7E, 0xA0, 0x0A, 0x00, 0x02, 0xFE, 0xFF, 0x03, 0x93, 0x5E, 0x92, 0x7E};

    dlms::hdlc::HdlcContext context;
    dlms::hdlc::HdlcParameters parameters;

    //REQUIRE (hdlc.serialize_snrm() == expected_snrm);
}
