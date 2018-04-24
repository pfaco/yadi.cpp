#include <yadi/packer.h>

namespace yadi {

StructPacker::StructPacker() {
    m_data.push_back(TypeTags::STRUCTURE);
    m_data.push_back(0); //number of elements
}

void StructPacker::add_element(std::vector<uint8_t> data) {
    m_number_of_elements++;
    m_data.insert(m_data.end(), data.begin(), data.end());
}

std::vector<uint8_t> StructPacker::pack() {
    insert_size(m_data, m_number_of_elements);
    return m_data;
}

}
