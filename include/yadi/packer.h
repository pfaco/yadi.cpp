//
// Created by paulo on 4/22/2018.
//

#ifndef YADI_DLMS_PACKER_H
#define YADI_DLMS_PACKER_H

#include <cstdint>
#include <vector>
#include <yadi/parser.h>

namespace yadi
{

class StructPacker {
public:
    void add_element(std::vector<uint8_t> data);
    auto pack() -> std::vector<uint8_t> ;

private:
    std::vector<uint8_t> m_data;
    unsigned m_number_of_elements = 0;
};

}

#endif //YADI_DLMS_PACKER_H
