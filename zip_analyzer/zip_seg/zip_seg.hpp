#ifndef ZIP_SEG_HPP
#define ZIP_SEG_HPP

#include <cstdint>
#include <memory>
#include <fstream>


class ZipSeg {
public:
    virtual void print() const = 0;
    virtual bool readFromFile(std::ifstream& file) = 0;
    virtual ~ZipSeg() = default;
};

class LocalFileHeader : public ZipSeg {
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
    std::unique_ptr<char[]> filename;
    std::unique_ptr<uint8_t[]> extra_field;
};

class CentralDirectoryHeader : public ZipSeg {
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
    std::unique_ptr<char[]> filename;
    std::unique_ptr<uint8_t[]> extra_field;
    std::unique_ptr<char[]> file_comment;
};

class EndOfCentralDirectoryRecord : public ZipSeg {
public:
    void print() const override;
    bool readFromFile(std::ifstream& file) override;
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
    std::unique_ptr<char[]> zip_file_comment;
};

#endif /* ZIP_SEG_HPP */
