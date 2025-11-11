#include "utils.hpp"

template uint16_t readLittleEndian<uint16_t>(std::ifstream& file);
template uint32_t readLittleEndian<uint32_t>(std::ifstream& file);
