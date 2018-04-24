#include <yadi/security.h>
#include <iterator>
#include <random>
#include <climits>
#include <algorithm>

std::independent_bits_engine<std::default_random_engine, CHAR_BIT, unsigned char> rng;

namespace yadi {

    void Security::generate_challenger(unsigned size, std::vector<uint8_t> &buffer) {
        for (unsigned i = 0; i < size; ++i) {
            buffer.push_back(rng());
        }
    }

}
