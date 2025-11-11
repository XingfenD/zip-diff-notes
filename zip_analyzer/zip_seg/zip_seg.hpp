#ifndef ZIP_SEG_HPP
#define ZIP_SEG_HPP

#include <cstdint>
#include <memory>
#include <fstream>
#include <vector>

/* virtual base class for zip segment */
class ZipSeg {
public:
    virtual void print() const = 0;
    virtual bool readFromFile(std::ifstream& file) = 0;
    virtual ~ZipSeg() = default;
};

class LocalFileHeader: public ZipSeg {
public:
    /* define default constructor */
    LocalFileHeader() :
        signature(0), version_needed(0), general_bit_flag(0),
        compression_method(0), last_mod_time(0), last_mod_date(0),
        crc32(0), compressed_size(0), uncompressed_size(0),
        filename_length(0), extra_field_length(0) {}

    void print() const override;
    bool readFromFile(std::ifstream& file) override;
    ~LocalFileHeader() = default;

    /* define move constructor and assignment operator */
    LocalFileHeader(LocalFileHeader&& other) noexcept = default;
    LocalFileHeader& operator=(LocalFileHeader&& other) noexcept = default;

    /* forbid copy constructor and assignment operator */
    LocalFileHeader(const LocalFileHeader&) = delete;
    LocalFileHeader& operator=(const LocalFileHeader&) = delete;

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
    /* define default constructor */
    CentralDirectoryHeader() :
        signature(0), version_made_by(0), version_needed(0),
        general_bit_flag(0), compression_method(0), last_mod_time(0),
        last_mod_date(0), crc32(0), compressed_size(0), uncompressed_size(0),
        filename_length(0), extra_field_length(0), file_comment_length(0),
        disk_number_start(0), internal_attr(0), external_attr(0),
        local_header_offset(0) {}

    void print() const override;
    bool readFromFile(std::ifstream& file) override;
    std::streampos getLocalFileHeaderOffset() const { return local_header_offset; }
    ~CentralDirectoryHeader() = default;

    /* define move constructor and assignment operator */
    CentralDirectoryHeader(CentralDirectoryHeader&& other) noexcept = default;
    CentralDirectoryHeader& operator=(CentralDirectoryHeader&& other) noexcept = default;

    /* forbid copy constructor and assignment operator */
    CentralDirectoryHeader(const CentralDirectoryHeader&) = delete;
    CentralDirectoryHeader& operator=(const CentralDirectoryHeader&) = delete;

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
    uint32_t local_header_offset;
    std::string filename;
    std::unique_ptr<uint8_t[]> extra_field;
    std::string file_comment;
};

class EndOfCentralDirectoryRecord: public ZipSeg {
public:
    void print() const override;
    bool readFromFile(std::ifstream& file) override;
    /* return the position of EndOfCentralDirectoryRecord signature found from end of file, or -1 if not found */
    static std::streampos findFromEnd(std::ifstream& file);
    std::streampos getCentralDirOffset() const { return central_dir_offset; }
    uint16_t getCentralDirRecordCount() const { return central_dir_record_count; }
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
