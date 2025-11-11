#ifndef ZIP_SEG_HPP
#define ZIP_SEG_HPP

#include <cstdint>
#include <memory>
#include <fstream>
#include <vector>

class ZipSeg {
public:
    virtual void print() const = 0;
    virtual bool readFromFile(std::ifstream& file) = 0;
    virtual ~ZipSeg() = default;
};

class LocalFileHeader: public ZipSeg {
public:
    void print() const override;
    bool readFromFile(std::ifstream& file) override;
    ~LocalFileHeader() = default;

private:
    uint32_t signature;
    uint16_t version_needed;
    uint16_t general_bit_flag;
    uint16_t compression_method;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_length;
    uint16_t extra_field_length;
    std::string filename;
    std::unique_ptr<uint8_t[]> extra_field;
};

class CentralDirectoryHeader: public ZipSeg {
public:
    void print() const override;
    bool readFromFile(std::ifstream& file) override;
    ~CentralDirectoryHeader() = default;

private:
    uint32_t signature;
    uint16_t version_made_by;
    uint16_t version_needed;
    uint16_t general_bit_flag;
    uint16_t compression_method;
    uint16_t last_mod_time;
    uint16_t last_mod_date;
    uint32_t crc32;
    uint32_t compressed_size;
    uint32_t uncompressed_size;
    uint16_t filename_length;
    uint16_t extra_field_length;
    uint16_t file_comment_length;
    uint16_t disk_number_start;
    uint16_t internal_attr;
    uint32_t external_attr;
    uint32_t relative_offset;
    std::string filename;
    std::unique_ptr<uint8_t[]> extra_field;
    std::string file_comment;
};

class EndOfCentralDirectoryRecord: public ZipSeg {
public:
    void print() const override;
    bool readFromFile(std::ifstream& file) override;
    // 静态函数：从文件末尾向前寻找EndOfCentralDirectoryRecord签名，返回找到的位置
    static std::streampos findFromEnd(std::ifstream& file);
    ~EndOfCentralDirectoryRecord() = default;

private:
    uint32_t signature;
    uint16_t disk_number;
    uint16_t disk_with_central_dir_start;
    uint16_t central_dir_record_count;
    uint16_t total_central_dir_record_count;
    uint32_t central_dir_size;
    uint32_t central_dir_offset;
    uint16_t zip_file_comment_length;
    std::string zip_file_comment;
};

class ZipHandler {
public:
    ZipHandler(std::ifstream& file);
    ~ZipHandler() = default;
    bool parse(std::string mode);
    bool parseStream();
    bool parseStandard();
    void print() const;
    void printLocalFileHeaders() const;
    void printCentralDirectoryHeaders() const;
    void printEndOfCentralDirectoryRecord() const;

private:
    std::ifstream file;
    std::vector<LocalFileHeader> local_file_headers;
    std::vector<CentralDirectoryHeader> central_directory_headers;
    EndOfCentralDirectoryRecord end_of_central_directory_record;
};

#endif /* ZIP_SEG_HPP */
