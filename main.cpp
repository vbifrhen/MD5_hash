#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

bool binarySearch(const std::string& filename, const std::string& targetHash, std::string& foundValue) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Failed to open the file: " << filename << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line)) {
        size_t pos = line.find('\t');
        if (pos != std::string::npos) {
            std::string hashInFile = line.substr(0, pos);
            std::string value = line.substr(pos + 1);

            if (hashInFile == targetHash) {
                foundValue = value;
                std::cout << "Match found: " << hashInFile << " - " << foundValue << std::endl;
                return true;
            } else if (hashInFile > targetHash) {
                break;  
            }
        }
    }

    return false;
}

bool searchInTables(const std::vector<std::string>& tableFiles, const std::string& targetHash, std::string& foundValue) {
    for (const auto& tableFile : tableFiles) {
        if (binarySearch(tableFile, targetHash, foundValue)) {
            return true;
        }
    }
    return false;
}

int main() {
    std::string targetHash;
    std::cout << "Enter MD5 hash: ";
    std::cin >> targetHash;  

    std::string directoryPath = "./";
    std::vector<std::string> tableFiles;

    auto startTime = std::chrono::high_resolution_clock::now();

    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (entry.is_regular_file() && entry.path().filename().string().find("md5_rainbow_table_part_") != std::string::npos) {
            tableFiles.push_back(entry.path().string());
        }
    }

    std::sort(tableFiles.begin(), tableFiles.end());

    std::string foundValue;
    if (!searchInTables(tableFiles, targetHash, foundValue)) {
        std::cout << "Match not found: " << targetHash << std::endl;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Search execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}
