#include "utils.hpp"

// 为常用类型提供显式实例化，确保在编译时就能生成代码
template uint16_t readLittleEndian<uint16_t>(std::ifstream& file);
template uint32_t readLittleEndian<uint32_t>(std::ifstream& file);
