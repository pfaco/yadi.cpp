#pragma once

#include <array>
#include <cstdint>
#include <sstream>
#include <algorithm>

namespace dlms
{

class LogicalName
{
public:
    LogicalName(const std::string &str) {
        std::vector<uint8_t> tokens;
        std::string token;
        std::istringstream token_stream(str);
        while (std::getline(token_stream, token, '.')) {
            uint8_t val = static_cast<uint8_t>(std::atoi(token.c_str()));
            tokens.push_back(val);
        }
        if (tokens.size() != 6) {
            throw std::runtime_error{"logical name must be 6 bytes long"};
        }
        std::copy_n(tokens.begin(), 6, value_.begin());
    }

    LogicalName(std::initializer_list<uint8_t> const& initializer_list) {
        if (initializer_list.size() != 6) {
            throw std::runtime_error{"logical name must be 6 bytes long"};
        }
        std::copy_n(initializer_list.begin(), 6, value_.begin());
    }

    std::array<uint8_t,6> value() const {
        return value_;
    }

private:
    std::array<uint8_t,6> value_;
};

}
