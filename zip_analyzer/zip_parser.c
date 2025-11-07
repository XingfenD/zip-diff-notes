#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

// ZIP文件头签名
#define LOCAL_FILE_HEADER_SIGNATURE 0x04034B50
#define CENTRAL_DIR_HEADER_SIGNATURE 0x02014B50
#define END_CENTRAL_DIR_SIGNATURE 0x06054B50

// 压缩方法枚举
typedef enum {
    COMPRESSION_STORED = 0,        // 无压缩
    COMPRESSION_SHRUNK = 1,        // 缩小压缩
    COMPRESSION_REDUCED1 = 2,      // 压缩因子1
    COMPRESSION_REDUCED2 = 3,      // 压缩因子2
    COMPRESSION_REDUCED3 = 4,      // 压缩因子3
    COMPRESSION_REDUCED4 = 5,      // 压缩因子4
    COMPRESSION_IMPLODED = 6,      // 自展压缩
    COMPRESSION_DEFLATED = 8,      // Deflate压缩
    COMPRESSION_DEFLATE64 = 9,     // Deflate64压缩
    COMPRESSION_BZIP2 = 12,        // BZIP2压缩
    COMPRESSION_LZMA = 14,         // LZMA压缩
    COMPRESSION_WAVPACK = 97,      // WavPack压缩
    COMPRESSION_PPMD = 98          // PPMd压缩
} CompressionMethod;

// 本地文件头结构
typedef struct {
    uint32_t signature;            // 0x04034B50
    uint16_t version_needed;       // 解压所需版本
    uint16_t general_bit_flag;     // 通用比特标志
    uint16_t compression_method;   // 压缩方法
    uint16_t last_mod_time;        // 最后修改时间
    uint16_t last_mod_date;        // 最后修改日期
    uint32_t crc32;                // CRC32校验码
    uint32_t compressed_size;      // 压缩后大小
    uint32_t uncompressed_size;    // 未压缩大小
    uint16_t filename_length;      // 文件名长度
    uint16_t extra_field_length;   // 扩展区长度
    char *filename;                // 文件名
    uint8_t *extra_field;          // 扩展区数据
} LocalFileHeader;

// 中央目录文件头结构
typedef struct {
    uint32_t signature;            // 0x02014B50
    uint16_t version_made_by;      // 压缩所用版本
    uint16_t version_needed;       // 解压所需版本
    uint16_t general_bit_flag;     // 通用比特标志
    uint16_t compression_method;   // 压缩方法
    uint16_t last_mod_time;        // 最后修改时间
    uint16_t last_mod_date;        // 最后修改日期
    uint32_t crc32;                // CRC32校验码
    uint32_t compressed_size;      // 压缩后大小
    uint32_t uncompressed_size;    // 未压缩大小
    uint16_t filename_length;      // 文件名长度
    uint16_t extra_field_length;   // 扩展区长度
    uint16_t file_comment_length;  // 文件注释长度
    uint16_t disk_number_start;    // 磁盘起始号
    uint16_t internal_file_attr;   // 内部文件属性
    uint32_t external_file_attr;   // 外部文件属性
    uint32_t local_header_offset;  // 本地文件头偏移
    char *filename;                // 文件名
    uint8_t *extra_field;          // 扩展区数据
    char *file_comment;            // 文件注释
} CentralDirHeader;

// 中央目录结束记录结构
typedef struct {
    uint32_t signature;            // 0x06054B50
    uint16_t disk_number;          // 磁盘号
    uint16_t disk_dir_start;       // 中央目录起始磁盘号
    uint16_t disk_entries;         // 本磁盘中央目录项数
    uint16_t total_entries;        // 总中央目录项数
    uint32_t dir_size;             // 中央目录大小
    uint32_t dir_offset;           // 中央目录偏移
    uint16_t comment_length;       // 注释长度
    char *comment;                 // 注释内容
} EndCentralDirRecord;

// 从文件读取小端序16位整数
uint16_t read_le16(FILE *file) {
    uint8_t bytes[2];
    fread(bytes, 1, 2, file);
    return (uint16_t)((bytes[1] << 8) | bytes[0]);
}

// 从文件读取小端序32位整数
uint32_t read_le32(FILE *file) {
    uint8_t bytes[4];
    fread(bytes, 1, 4, file);
    return (uint32_t)((bytes[3] << 24) | (bytes[2] << 16) | (bytes[1] << 8) | bytes[0]);
}

// 解析MS-DOS时间
void parse_dos_time(uint16_t dos_time, uint16_t dos_date, struct tm *tm) {
    // 时间格式: hhmmss
    tm->tm_sec = ((dos_time & 0x1F) << 1);  // 0-4位: 秒/2
    tm->tm_min = ((dos_time >> 5) & 0x3F);  // 5-10位: 分钟
    tm->tm_hour = ((dos_time >> 11) & 0x1F); // 11-15位: 小时

    // 日期格式: yyyymmdd
    tm->tm_mday = (dos_date & 0x1F);         // 0-4位: 日
    tm->tm_mon = ((dos_date >> 5) & 0x0F) - 1; // 5-8位: 月 (0-11)
    tm->tm_year = ((dos_date >> 9) & 0x7F) + 80; // 9-15位: 年 (1980起)
}

// 获取压缩方法描述
const char *get_compression_method_desc(uint16_t method) {
    switch (method) {
        case COMPRESSION_STORED: return "Stored (无压缩)";
        case COMPRESSION_SHRUNK: return "Shrunk (缩小压缩)";
        case COMPRESSION_REDUCED1: return "Reduced with compression factor 1";
        case COMPRESSION_REDUCED2: return "Reduced with compression factor 2";
        case COMPRESSION_REDUCED3: return "Reduced with compression factor 3";
        case COMPRESSION_REDUCED4: return "Reduced with compression factor 4";
        case COMPRESSION_IMPLODED: return "Imploded (自展压缩)";
        case COMPRESSION_DEFLATED: return "Deflated (Deflate压缩)";
        case COMPRESSION_DEFLATE64: return "Deflate64 (Deflate64压缩)";
        case COMPRESSION_BZIP2: return "BZIP2 (BZIP2压缩)";
        case COMPRESSION_LZMA: return "LZMA (LZMA压缩)";
        case COMPRESSION_WAVPACK: return "WavPack (WavPack压缩)";
        case COMPRESSION_PPMD: return "PPMd (PPMd压缩)";
        default: return "Unknown (未知压缩方法)";
    }
}

// 解析本地文件头
int parse_local_file_header(FILE *file, LocalFileHeader *header) {
    // 读取基本字段
    header->signature = read_le32(file);
    if (header->signature != LOCAL_FILE_HEADER_SIGNATURE) {
        printf("错误: 不是有效的本地文件头 (签名: 0x%08X)\n", header->signature);
        return -1;
    }

    header->version_needed = read_le16(file);
    header->general_bit_flag = read_le16(file);
    header->compression_method = read_le16(file);
    header->last_mod_time = read_le16(file);
    header->last_mod_date = read_le16(file);
    header->crc32 = read_le32(file);
    header->compressed_size = read_le32(file);
    header->uncompressed_size = read_le32(file);
    header->filename_length = read_le16(file);
    header->extra_field_length = read_le16(file);

    // 读取文件名
    header->filename = (char *)malloc(header->filename_length + 1);
    if (!header->filename) {
        perror("malloc failed");
        return -1;
    }
    fread(header->filename, 1, header->filename_length, file);
    header->filename[header->filename_length] = '\0';

    // 读取扩展区
    if (header->extra_field_length > 0) {
        header->extra_field = (uint8_t *)malloc(header->extra_field_length);
        if (!header->extra_field) {
            perror("malloc failed");
            free(header->filename);
            return -1;
        }
        fread(header->extra_field, 1, header->extra_field_length, file);
    } else {
        header->extra_field = NULL;
    }

    return 0;
}

// 显示本地文件头信息
void display_local_file_header(const LocalFileHeader *header) {
    struct tm file_time;
    char time_str[64];

    parse_dos_time(header->last_mod_time, header->last_mod_date, &file_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &file_time);

    printf("\n=== 本地文件头信息 ===\n");
    printf("签名: 0x%08X\n", header->signature);
    printf("解压所需版本: %d.%d\n", header->version_needed / 10, header->version_needed % 10);
    printf("通用比特标志: 0x%04X\n", header->general_bit_flag);
    printf("压缩方法: 0x%04X (%s)\n", header->compression_method, get_compression_method_desc(header->compression_method));
    printf("最后修改时间: %s\n", time_str);
    printf("CRC32校验码: 0x%08X\n", header->crc32);
    printf("压缩后大小: %u 字节\n", header->compressed_size);
    printf("未压缩大小: %u 字节\n", header->uncompressed_size);
    printf("文件名长度: %u 字节\n", header->filename_length);
    printf("扩展区长度: %u 字节\n", header->extra_field_length);
    printf("文件名: %s\n", header->filename);

    // 显示压缩率
    if (header->uncompressed_size > 0) {
        double compression_ratio = (double)(header->uncompressed_size - header->compressed_size) /
                                  header->uncompressed_size * 100;
        printf("压缩率: %.2f%%\n", compression_ratio);
    }
}

// 释放本地文件头内存
void free_local_file_header(LocalFileHeader *header) {
    if (header) {
        free(header->filename);
        free(header->extra_field);
        memset(header, 0, sizeof(LocalFileHeader));
    }
}

// 解析中央目录文件头
int parse_central_dir_header(FILE *file, CentralDirHeader *header) {
    header->signature = read_le32(file);
    if (header->signature != CENTRAL_DIR_HEADER_SIGNATURE) {
        printf("错误: 不是有效的中央目录文件头 (签名: 0x%08X)\n", header->signature);
        return -1;
    }

    header->version_made_by = read_le16(file);
    header->version_needed = read_le16(file);
    header->general_bit_flag = read_le16(file);
    header->compression_method = read_le16(file);
    header->last_mod_time = read_le16(file);
    header->last_mod_date = read_le16(file);
    header->crc32 = read_le32(file);
    header->compressed_size = read_le32(file);
    header->uncompressed_size = read_le32(file);
    header->filename_length = read_le16(file);
    header->extra_field_length = read_le16(file);
    header->file_comment_length = read_le16(file);
    header->disk_number_start = read_le16(file);
    header->internal_file_attr = read_le16(file);
    header->external_file_attr = read_le32(file);
    header->local_header_offset = read_le32(file);

    // 读取文件名
    header->filename = (char *)malloc(header->filename_length + 1);
    if (!header->filename) {
        perror("malloc failed");
        return -1;
    }
    fread(header->filename, 1, header->filename_length, file);
    header->filename[header->filename_length] = '\0';

    // 读取扩展区
    if (header->extra_field_length > 0) {
        header->extra_field = (uint8_t *)malloc(header->extra_field_length);
        if (!header->extra_field) {
            perror("malloc failed");
            free(header->filename);
            return -1;
        }
        fread(header->extra_field, 1, header->extra_field_length, file);
    } else {
        header->extra_field = NULL;
    }

    // 读取文件注释
    if (header->file_comment_length > 0) {
        header->file_comment = (char *)malloc(header->file_comment_length + 1);
        if (!header->file_comment) {
            perror("malloc failed");
            free(header->filename);
            free(header->extra_field);
            return -1;
        }
        fread(header->file_comment, 1, header->file_comment_length, file);
        header->file_comment[header->file_comment_length] = '\0';
    } else {
        header->file_comment = NULL;
    }

    return 0;
}

// 显示中央目录文件头信息
void display_central_dir_header(const CentralDirHeader *header) {
    struct tm file_time;
    char time_str[64];

    parse_dos_time(header->last_mod_time, header->last_mod_date, &file_time);
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", &file_time);

    printf("\n=== 中央目录文件头信息 ===\n");
    printf("签名: 0x%08X\n", header->signature);
    printf("压缩所用版本: %d.%d\n", header->version_made_by / 10, header->version_made_by % 10);
    printf("解压所需版本: %d.%d\n", header->version_needed / 10, header->version_needed % 10);
    printf("通用比特标志: 0x%04X\n", header->general_bit_flag);
    printf("压缩方法: 0x%04X (%s)\n", header->compression_method, get_compression_method_desc(header->compression_method));
    printf("最后修改时间: %s\n", time_str);
    printf("CRC32校验码: 0x%08X\n", header->crc32);
    printf("压缩后大小: %u 字节\n", header->compressed_size);
    printf("未压缩大小: %u 字节\n", header->uncompressed_size);
    printf("文件名长度: %u 字节\n", header->filename_length);
    printf("扩展区长度: %u 字节\n", header->extra_field_length);
    printf("文件注释长度: %u 字节\n", header->file_comment_length);
    printf("磁盘起始号: %u\n", header->disk_number_start);
    printf("内部文件属性: 0x%04X\n", header->internal_file_attr);
    printf("外部文件属性: 0x%08X\n", header->external_file_attr);
    printf("本地文件头偏移: 0x%08X\n", header->local_header_offset);
    printf("文件名: %s\n", header->filename);
    if (header->file_comment) {
        printf("文件注释: %s\n", header->file_comment);
    }
}

// 释放中央目录文件头内存
void free_central_dir_header(CentralDirHeader *header) {
    if (header) {
        free(header->filename);
        free(header->extra_field);
        free(header->file_comment);
        memset(header, 0, sizeof(CentralDirHeader));
    }
}

// 解析中央目录结束记录
int parse_end_central_dir_record(FILE *file, EndCentralDirRecord *record) {
    record->signature = read_le32(file);
    if (record->signature != END_CENTRAL_DIR_SIGNATURE) {
        printf("错误: 不是有效的中央目录结束记录 (签名: 0x%08X)\n", record->signature);
        return -1;
    }

    record->disk_number = read_le16(file);
    record->disk_dir_start = read_le16(file);
    record->disk_entries = read_le16(file);
    record->total_entries = read_le16(file);
    record->dir_size = read_le32(file);
    record->dir_offset = read_le32(file);
    record->comment_length = read_le16(file);

    // 读取注释
    if (record->comment_length > 0) {
        record->comment = (char *)malloc(record->comment_length + 1);
        if (!record->comment) {
            perror("malloc failed");
            return -1;
        }
        fread(record->comment, 1, record->comment_length, file);
        record->comment[record->comment_length] = '\0';
    } else {
        record->comment = NULL;
    }

    return 0;
}

// 显示中央目录结束记录信息
void display_end_central_dir_record(const EndCentralDirRecord *record) {
    printf("\n=== 中央目录结束记录 ===\n");
    printf("签名: 0x%08X\n", record->signature);
    printf("磁盘号: %u\n", record->disk_number);
    printf("中央目录起始磁盘号: %u\n", record->disk_dir_start);
    printf("本磁盘中央目录项数: %u\n", record->disk_entries);
    printf("总中央目录项数: %u\n", record->total_entries);
    printf("中央目录大小: %u 字节\n", record->dir_size);
    printf("中央目录偏移: 0x%08X\n", record->dir_offset);
    printf("注释长度: %u 字节\n", record->comment_length);
    if (record->comment) {
        printf("ZIP文件注释: %s\n", record->comment);
    }
}

// 释放中央目录结束记录内存
void free_end_central_dir_record(EndCentralDirRecord *record) {
    if (record) {
        free(record->comment);
        memset(record, 0, sizeof(EndCentralDirRecord));
    }
}

// 查找中央目录结束记录
int find_end_central_dir_record(FILE *file, EndCentralDirRecord *record) {
    long file_size = ftell(file);
    fseek(file, 0, SEEK_END);
    file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // 从文件末尾向前搜索中央目录结束记录
    const int MAX_SEARCH_SIZE = 65536; // 最大搜索64KB
    long search_start = (file_size > MAX_SEARCH_SIZE) ? (file_size - MAX_SEARCH_SIZE) : 0;

    for (long offset = file_size - 4; offset >= search_start; offset--) {
        fseek(file, offset, SEEK_SET);
        uint32_t signature = read_le32(file);

        if (signature == END_CENTRAL_DIR_SIGNATURE) {
            // 找到了中央目录结束记录
            fseek(file, offset, SEEK_SET);
            return parse_end_central_dir_record(file, record);
        }
    }

    printf("错误: 未找到中央目录结束记录\n");
    return -1;
}

// 解析整个ZIP文件
int parse_zip_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    if (!file) {
        perror("无法打开文件");
        return -1;
    }

    printf("正在解析ZIP文件: %s\n", filename);
    printf("文件路径: %s\n", filename);

    // 解析本地文件头
    LocalFileHeader local_header;
    if (parse_local_file_header(file, &local_header) == 0) {
        display_local_file_header(&local_header);
        free_local_file_header(&local_header);
    }

    // 查找并解析中央目录结束记录
    EndCentralDirRecord end_record;
    if (find_end_central_dir_record(file, &end_record) == 0) {
        display_end_central_dir_record(&end_record);

        // 解析中央目录
        fseek(file, end_record.dir_offset, SEEK_SET);
        CentralDirHeader central_header;
        if (parse_central_dir_header(file, &central_header) == 0) {
            display_central_dir_header(&central_header);
            free_central_dir_header(&central_header);
        }

        free_end_central_dir_record(&end_record);
    }

    fclose(file);
    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("用法: %s <zip文件名>\n", argv[0]);
        printf("示例: %s test.zip\n", argv[0]);
        return 1;
    }

    return parse_zip_file(argv[1]);
}