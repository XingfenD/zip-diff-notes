#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstdint>
#include <memory>
#include <fstream>

// 读取指定大小的数据并处理小端字节序的通用函数
template<typename T>
inline T readLittleEndian(std::ifstream& file) {
    T value;
    file.read(reinterpret_cast<char*>(&value), sizeof(T));
    return value;
}

#endif /* UTILS_HPP */
