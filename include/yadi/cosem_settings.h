#pragma once

#include <array>
#include <cstdint>

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

struct CosemSettings
{
    AuthenticationMechanism authentication = AuthenticationMechanism::LOWEST;
    SecurityContext security = SecurityContext::NONE;
    std::array<uint8_t,8> system_title{0};
    std::array<uint8_t,8> secret{0};
    std::array<uint8_t,16> ak{0};
    std::array<uint8_t,16> guek{0};
};

}
