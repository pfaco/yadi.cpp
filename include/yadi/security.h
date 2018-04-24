//
// Created by paulo.faco on 2018-04-20.
//

#ifndef YADI_DLMS_SECURITY_H
#define YADI_DLMS_SECURITY_H

#include <yadi/cosem.h>

namespace yadi {

    class Security {
    public:
        static void generate_challenger(unsigned size, std::vector<uint8_t> &buffer);
    };

}

#endif //YADI_DLMS_SECURITY_H
