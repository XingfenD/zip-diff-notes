#include "zip_seg.hpp"
#include "utils.hpp"
#include "defs.hpp"
#include <iostream>
#include <stdexcept>

ZipHandler::ZipHandler(std::ifstream& file) : file(std::move(file)) {}

bool ZipHandler::parse(std::string mode) {
    if (mode == "standard") {
        return parseStandard();
    } else if (mode == "stream") {
        return parseStream();
    } else {
        return false;
    }
}

bool ZipHandler::parseStandard() {
    if (!file.is_open() || !file.good()) {
        return false;
    }

    /* find EndOfCentralDirectoryRecord from end of file */
    std::streampos record_pos = EndOfCentralDirectoryRecord::findFromEnd(file);
    if (record_pos == -1) {
        return false;
    }

    /* move file pointer to record position and read */
    file.seekg(record_pos);
    if (!end_of_central_directory_record.readFromFile(file)) {
        return false;
    }

    /* move file pointer to start of central directory */
    file.seekg(end_of_central_directory_record.getCentralDirOffset());

    for (uint16_t i = 0; i < end_of_central_directory_record.getCentralDirRecordCount(); ++i) {
        CentralDirectoryHeader header;
        if (!header.readFromFile(file)) {
            return false;
        }
        central_directory_headers.push_back(std::move(header));
    }

    for (const auto& header : central_directory_headers) {
        file.seekg(header.getLocalFileHeaderOffset());
        LocalFileHeader local_header;
        if (!local_header.readFromFile(file)) {
            return false;
        }
        local_file_headers.push_back(std::move(local_header));
    }

    return true;
}

bool ZipHandler::parseStream() {
    return true;
}

void ZipHandler::print() const {
    printLocalFileHeaders();
    printCentralDirectoryHeaders();
    printEndOfCentralDirectoryRecord();
}

void ZipHandler::printLocalFileHeaders() const {
    for (const auto& header : local_file_headers) {
        header.print();
    }
}
void ZipHandler::printCentralDirectoryHeaders() const {
    for (const auto& header : central_directory_headers) {
        header.print();
    }
}

void ZipHandler::printEndOfCentralDirectoryRecord() const {
    end_of_central_directory_record.print();
}



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

    if (filename_length > 0) {
        std::cout << "  Filename: " << filename << std::endl;
    }

    /* will not print extra field */
}

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
        filename = std::string(filename_length, '\0');
        file.read(&filename[0], filename_length);
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
    std::cout << "  Local Header Offset: 0x" << std::hex << local_header_offset << std::dec << std::endl;

    if (filename_length > 0) {
        std::cout << "  Filename: " << filename << std::endl;
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
    local_header_offset = readLittleEndian<uint32_t>(file);
    // 读取文件名
    if (filename_length > 0) {
        filename = std::string(filename_length, '\0');
        file.read(&filename[0], filename_length);
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

    if (zip_file_comment_length > 0) {
        std::cout << "  ZIP File Comment: " << zip_file_comment << std::endl;
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
        zip_file_comment = std::string(zip_file_comment_length, '\0');
        file.read(&zip_file_comment[0], zip_file_comment_length);
    }

    return !file.fail();
}

std::streampos EndOfCentralDirectoryRecord::findFromEnd(std::ifstream& file) {
    if (!file.is_open() || !file.good()) {
        return -1; // 返回无效位置
    }

    // 保存当前文件位置
    std::streampos original_pos = file.tellg();

    // 获取文件大小
    file.seekg(0, std::ios::end);
    std::streampos file_size = file.tellg();

    // EndOfCentralDirectoryRecord最小大小为22字节（不包括注释）
    // 最大注释长度为65535字节，所以我们从文件末尾开始向前搜索
    // 但要确保我们不会搜索超过文件的前半部分（避免过度搜索）
    const size_t max_search_size = std::min(static_cast<size_t>(file_size) / 2, static_cast<size_t>(65535 + 22));

    // 从文件末尾向前搜索，但留出EndOfCentralDirectoryRecord的最小大小
    std::streampos search_start_pos = file_size - static_cast<std::streampos>(max_search_size);
    if (search_start_pos < 0) {
        search_start_pos = 0;
    }

    file.seekg(search_start_pos, std::ios::beg);

    // 读取搜索区域的内容到缓冲区
    size_t buffer_size = static_cast<size_t>(file_size - search_start_pos);
    std::vector<char> buffer(buffer_size);
    file.read(buffer.data(), buffer_size);

    if (file.fail()) {
        // 恢复文件位置
        file.seekg(original_pos, std::ios::beg);
        return -1; // 返回无效位置
    }

    // 从缓冲区末尾向前搜索签名
    const uint32_t signature = END_OF_CENTRAL_DIRECTORY_SIG;
    const char* signature_bytes = reinterpret_cast<const char*>(&signature);

    for (size_t i = buffer_size - 4; i > 0; --i) {
        bool signature_match = true;
        for (size_t j = 0; j < 4; ++j) {
            if (buffer[i + j] != signature_bytes[j]) {
                signature_match = false;
                break;
            }
        }

        if (signature_match) {
            // 找到签名，计算其在文件中的绝对位置
            std::streampos record_pos = search_start_pos + static_cast<std::streampos>(i);

            // 移动文件指针到记录开始位置
            file.seekg(record_pos, std::ios::beg);

            // 验证这是一个有效的记录
            // 移动文件指针到记录开始位置并读取签名进行确认
            file.seekg(record_pos, std::ios::beg);
            uint32_t temp_signature = readLittleEndian<uint32_t>(file);

            // 恢复文件位置
            file.seekg(original_pos, std::ios::beg);

            // 如果签名有效，返回找到的位置
            if (temp_signature == END_OF_CENTRAL_DIRECTORY_SIG) {
                return record_pos;
            }
        }
    }

    // 未找到有效签名，恢复文件位置
    file.seekg(original_pos, std::ios::beg);
    return -1; // 返回无效位置
}

