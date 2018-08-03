#include "catch.hpp"
#include "yadi/data_type.h"
#include <iostream>

TEST_CASE( "Write size works correctly", "[write_size]") {
    std::vector<uint8_t> buffer;

    buffer.clear();
    dlms::write_size(buffer,0x00);
    REQUIRE (buffer == std::vector<uint8_t>{0x00});

    buffer.clear();
    dlms::write_size(buffer,0x01);
    REQUIRE (buffer == std::vector<uint8_t>{0x01});

    buffer.clear();
    dlms::write_size(buffer,0x80);
    REQUIRE (buffer == std::vector<uint8_t>{0x80});

    buffer.clear();
    dlms::write_size(buffer,0x81);
    REQUIRE (buffer == std::vector<uint8_t>{0x81, 0x81});

    buffer.clear();
    dlms::write_size(buffer,0xFF);
    REQUIRE (buffer == std::vector<uint8_t>{0x81, 0xFF});

    buffer.clear();
    dlms::write_size(buffer,0x100);
    REQUIRE (buffer == std::vector<uint8_t>{0x82, 0x01, 0x00});

    buffer.clear();
    dlms::write_size(buffer,0xFFFF);
    REQUIRE (buffer == std::vector<uint8_t>{0x82, 0xFF, 0xFF});

    buffer.clear();
    dlms::write_size(buffer,0x10000);
    REQUIRE (buffer == std::vector<uint8_t>{0x83, 0x01, 0x00, 0x00});

    buffer.clear();
    dlms::write_size(buffer,0xFFFFFF);
    REQUIRE (buffer == std::vector<uint8_t>{0x83, 0xFF, 0xFF, 0xFF});

    buffer.clear();
    dlms::write_size(buffer,0x1000000);
    REQUIRE (buffer == std::vector<uint8_t>{0x84, 0x01, 0x00, 0x00, 0x00});

    buffer.clear();
    dlms::write_size(buffer,0xFFFFFFFF);
    REQUIRE (buffer == std::vector<uint8_t>{0x84, 0xFF, 0xFF, 0xFF, 0xFF});
}

TEST_CASE( "Strings are correctly packed in dlms_type", "[from_string]" ) {
    std::vector<std::string> strs = {
            "0123456",
            "The Quick Brown Fox Jumped Over The Lazy Dog",
            "All Work and No Play Makes Jack a Dull Boy",
            "",
            "ZERO",
            "0987654321"
    };

    for (auto const& str : strs) {
        std::vector<uint8_t> expected;
        expected.push_back(0x0A);
        dlms::write_size(expected, str.size());
        expected.insert(expected.end(), str.begin(), str.end());
        REQUIRE( dlms::from_string(str) == expected );
    }
}

TEST_CASE ( "", "") {

}