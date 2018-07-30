#include <yadi/security.h>
#include <iterator>
#include <random>
#include <climits>
#include <algorithm>

std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char> rng;

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
            case AuthenticationMechanism::HLS_GCM:
                break;
            default:
                throw std::invalid_argument("invalid authentication type");
        }
    }
}
