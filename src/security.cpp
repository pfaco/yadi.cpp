#include <yadi/security.h>
#include <iterator>
#include <random>
#include <climits>
#include <algorithm>

std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char> rng;

namespace yadi {

    void Security::generate_challenger(unsigned size, std::vector<uint8_t> &buffer)
    {
        for (unsigned i = 0; i < size; ++i) {
            buffer.push_back(rng());
        }
    }

    std::vector<uint8_t> Security::process_challenger(const yadi::cosem_params &params)
    {
        switch (params.authentication_type) {
            case AuthenticationType::HLS_MD5:
                break;
            case AuthenticationType::HLS_SHA1:
                break;
            case AuthenticationType::HLS_SHA256:
                break;
            case AuthenticationType::HLS_GCM:
                break;
            default:
                throw std::invalid_argument("invalid authentication type");
        }
    }
}
