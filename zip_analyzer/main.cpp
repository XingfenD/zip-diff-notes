#include "cxxopts.hpp"
#include "zip_seg/zip_seg.hpp"
#include <iostream>
#include <string>
#include <fstream>

int main(int argc, char *argv[]) {
    cxxopts::Options options("zip_analyzer", "A tool to analyze ZIP files");
    options.add_options()
        ("f,file", "ZIP file to analyze", cxxopts::value<std::string>())
        ("m,mode", "Parsing mode (standard or stream)", cxxopts::value<std::string>()->default_value("standard"))
        ("h,help", "Print help");

    auto result = options.parse(argc, argv);

    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }

    if (!result.count("file")) {
        std::cerr << "Error: ZIP file not specified" << std::endl;
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::string zip_file = result["file"].as<std::string>();
    std::string mode = result["mode"].as<std::string>();

    /* validate mode */
    if (mode != "standard" && mode != "stream") {
        std::cerr << "Error: Invalid mode specified. Use 'standard' or 'stream'" << std::endl;
        std::cout << options.help() << std::endl;
        return 1;
    }

    std::cout << "Analyzing ZIP file: " << zip_file << " in " << mode << " mode" << std::endl;

    /* read the file content */
    std::ifstream file(zip_file, std::ios::binary);
    if (!file.is_open() || !file.good()) {
        std::cerr << "Error: Failed to open ZIP file for reading" << std::endl;
        return 1;
    }

    file.close();
    return 0;
}