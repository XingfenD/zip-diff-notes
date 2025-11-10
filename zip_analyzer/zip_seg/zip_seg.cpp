#include "zip_seg.hpp"
#include "utils.hpp"
#include "defs.hpp"
#include <iostream>
#include <stdexcept>

// 实现LocalFileHeader::print方法
void LocalFileHeader::print() const {
    std::cout << "Local File Header Information:" << std::endl;
    std::cout << "  Signature: 0x" << std::hex << signature << std::dec << std::endl;
    std::cout << "  Version Needed: " << version_needed << std::endl;
    std::cout << "  General Bit Flag: 0x" << std::hex << general_bit_flag << std::dec << std::endl;
    std::cout << "  Compression Method: " << compression_method << std::endl;
    std::cout << "  Last Mod Time: 0x" << std::hex << last_mod_time << std::dec << std::endl;
    std::cout << "  Last Mod Date: 0x" << std::hex << last_mod_date << std::dec << std::endl;
    std::cout << "  CRC32: 0x" << std::hex << crc32 << std::dec << std::endl;
    std::cout << "  Compressed Size: " << compressed_size << " bytes" << std::endl;
    std::cout << "  Uncompressed Size: " << uncompressed_size << " bytes" << std::endl;
    std::cout << "  Filename Length: " << filename_length << " bytes" << std::endl;
    std::cout << "  Extra Field Length: " << extra_field_length << " bytes" << std::endl;

    if (filename_length > 0 && filename) {
        std::cout << "  Filename: " << filename.get() << std::endl;
    }

    // 注意：通常不直接打印扩展区数据，因为它是二进制数据
}

// 实现readFromFile方法
bool LocalFileHeader::readFromFile(std::ifstream& file) {
    if (!file.is_open() || !file.good()) {
        return false;
    }

    // 读取固定大小的字段
    signature = readLittleEndian<uint32_t>(file);

    // 检查签名是否正确
    if (signature != LOCAL_FILE_HEADER_SIG) {
        return false;
    }

    version_needed = readLittleEndian<uint16_t>(file);
    general_bit_flag = readLittleEndian<uint16_t>(file);
    compression_method = readLittleEndian<uint16_t>(file);
    last_mod_time = readLittleEndian<uint16_t>(file);
    last_mod_date = readLittleEndian<uint16_t>(file);
    crc32 = readLittleEndian<uint32_t>(file);
    compressed_size = readLittleEndian<uint32_t>(file);
    uncompressed_size = readLittleEndian<uint32_t>(file);
    filename_length = readLittleEndian<uint16_t>(file);
    extra_field_length = readLittleEndian<uint16_t>(file);

    // 读取文件名
    if (filename_length > 0) {
        filename = std::make_unique<char[]>(filename_length + 1);  // +1 for null terminator
        file.read(filename.get(), filename_length);
        filename[filename_length] = '\0';  // 添加null终止符
    }

    // 读取扩展区数据
    if (extra_field_length > 0) {
        extra_field = std::make_unique<uint8_t[]>(extra_field_length);
        file.read(reinterpret_cast<char*>(extra_field.get()), extra_field_length);
    }

    return !file.fail();
}

void CentralDirectoryHeader::print() const {
    std::cout << "Central Directory Header Information:" << std::endl;
    std::cout << "  Signature: 0x" << std::hex << signature << std::dec << std::endl;
    std::cout << "  Version Made By: " << version_made_by << std::endl;
    std::cout << "  Version Needed: " << version_needed << std::endl;
    std::cout << "  General Bit Flag: 0x" << std::hex << general_bit_flag << std::dec << std::endl;
    std::cout << "  Compression Method: " << compression_method << std::endl;
    std::cout << "  Last Mod Time: 0x" << std::hex << last_mod_time << std::dec << std::endl;
    std::cout << "  Last Mod Date: 0x" << std::hex << last_mod_date << std::dec << std::endl;
    std::cout << "  CRC32: 0x" << std::hex << crc32 << std::dec << std::endl;
    std::cout << "  Compressed Size: " << compressed_size << " bytes" << std::endl;
    std::cout << "  Uncompressed Size: " << uncompressed_size << " bytes" << std::endl;
    std::cout << "  Filename Length: " << filename_length << " bytes" << std::endl;
    std::cout << "  Extra Field Length: " << extra_field_length << " bytes" << std::endl;
    std::cout << "  File Comment Length: " << file_comment_length << " bytes" << std::endl;
    std::cout << "  Disk Number Start: " << disk_number_start << std::endl;
    std::cout << "  Internal Attr: 0x" << std::hex << internal_attr << std::dec << std::endl;
    std::cout << "  External Attr: 0x" << std::hex << external_attr << std::dec << std::endl;
    std::cout << "  Relative Offset: 0x" << std::hex << relative_offset << std::dec << std::endl;

    if (filename_length > 0 && filename) {
        std::cout << "  Filename: " << filename.get() << std::endl;
    }

    // 注意：通常不直接打印扩展区数据，因为它是二进制数据
}

bool CentralDirectoryHeader::readFromFile(std::ifstream& file) {
    if (!file.is_open() || !file.good()) {
        return false;
    }

    // 读取固定大小的字段
    signature = readLittleEndian<uint32_t>(file);

    // 检查签名是否正确
    if (signature != CENTRAL_DIRECTORY_HEADER_SIG) {
        return false;
    }

    version_made_by = readLittleEndian<uint16_t>(file);
    version_needed = readLittleEndian<uint16_t>(file);
    general_bit_flag = readLittleEndian<uint16_t>(file);
    compression_method = readLittleEndian<uint16_t>(file);
    last_mod_time = readLittleEndian<uint16_t>(file);
    last_mod_date = readLittleEndian<uint16_t>(file);
    crc32 = readLittleEndian<uint32_t>(file);
    compressed_size = readLittleEndian<uint32_t>(file);
    uncompressed_size = readLittleEndian<uint32_t>(file);
    filename_length = readLittleEndian<uint16_t>(file);
    extra_field_length = readLittleEndian<uint16_t>(file);
    file_comment_length = readLittleEndian<uint16_t>(file);
    disk_number_start = readLittleEndian<uint16_t>(file);
    internal_attr = readLittleEndian<uint16_t>(file);
    external_attr = readLittleEndian<uint32_t>(file);
    relative_offset = readLittleEndian<uint32_t>(file);
    // 读取文件名
    if (filename_length > 0) {
        filename = std::make_unique<char[]>(filename_length + 1);  // +1 for null terminator
        file.read(filename.get(), filename_length);
        filename[filename_length] = '\0';  // 添加null终止符
    }

    // 读取扩展区数据
    if (extra_field_length > 0) {
        extra_field = std::make_unique<uint8_t[]>(extra_field_length);
        file.read(reinterpret_cast<char*>(extra_field.get()), extra_field_length);
    }

    return !file.fail();
}

void EndOfCentralDirectoryRecord::print() const {
    std::cout << "End of Central Directory Record Information:" << std::endl;
    std::cout << "  Signature: 0x" << std::hex << signature << std::dec << std::endl;
    std::cout << "  Disk Number: " << disk_number << std::endl;
    std::cout << "  Disk with Central Directory Start: " << disk_with_central_dir_start << std::endl;
    std::cout << "  Central Directory Record Count: " << central_dir_record_count << std::endl;
    std::cout << "  Total Central Directory Record Count: " << total_central_dir_record_count << std::endl;
    std::cout << "  Central Directory Size: " << central_dir_size << " bytes" << std::endl;
    std::cout << "  Central Directory Offset: 0x" << std::hex << central_dir_offset << std::dec << std::endl;
    std::cout << "  ZIP File Comment Length: " << zip_file_comment_length << " bytes" << std::endl;

    if (zip_file_comment_length > 0 && zip_file_comment) {
        std::cout << "  ZIP File Comment: " << zip_file_comment.get() << std::endl;
    }
}

bool EndOfCentralDirectoryRecord::readFromFile(std::ifstream& file) {
    if (!file.is_open() || !file.good()) {
        return false;
    }

    // 读取固定大小的字段
    signature = readLittleEndian<uint32_t>(file);

    // 检查签名是否正确
    if (signature != END_OF_CENTRAL_DIRECTORY_SIG) {
        return false;
    }

    disk_number = readLittleEndian<uint16_t>(file);
    disk_with_central_dir_start = readLittleEndian<uint16_t>(file);
    central_dir_record_count = readLittleEndian<uint16_t>(file);
    total_central_dir_record_count = readLittleEndian<uint16_t>(file);
    central_dir_size = readLittleEndian<uint32_t>(file);
    central_dir_offset = readLittleEndian<uint32_t>(file);
    zip_file_comment_length = readLittleEndian<uint16_t>(file);

    // 读取zip文件注释
    if (zip_file_comment_length > 0) {
        zip_file_comment = std::make_unique<char[]>(zip_file_comment_length + 1);  // +1 for null terminator
        file.read(zip_file_comment.get(), zip_file_comment_length);
        zip_file_comment[zip_file_comment_length] = '\0';  // 添加null终止符
    }

    return !file.fail();
}
