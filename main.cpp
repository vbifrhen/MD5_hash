#include <iostream>
#include <fstream>
#include <chrono>
#include <filesystem>
#include <vector>
#include <algorithm>

namespace fs = std::filesystem;

bool binarySearchInFile(std::ifstream& file, const std::string& targetHash, std::string& foundValue) {
    std::streampos low = 0;
    file.seekg(0, std::ios::end);
    std::streampos high = file.tellg();

    while (low <= high) {
        std::streampos mid = low + (high - low) / 2;
        mid -= static_cast<std::streampos>(sizeof(char));  // Align to record boundary
        file.seekg(mid);

        // Find the beginning of the current line
        while (mid > low) {
            char c;
            file.seekg(mid);
            file.get(c);
            if (c == '\n') {
                break;
            }
            mid -= static_cast<std::streampos>(sizeof(char));
        }

        std::string line;
        std::getline(file, line);

        // Extract hash and name from the line
        size_t tabPos = line.find('\t');
        if (tabPos != std::string::npos) {
            std::string hash = line.substr(0, tabPos); 
            // Perform binary search
            if (hash == targetHash) {
                foundValue = line.substr(tabPos + 1);
                std::cout << "Hash found! " << line << std::endl;
                return true;
            } else if (hash < targetHash) {
                // The target hash is to the right
                low = mid + static_cast<std::streampos>(sizeof(char));
            } else {
                // The target hash is to the left
                high = mid - static_cast<std::streampos>(sizeof(char));
            }
        } else {
            std::cerr << "Error: Invalid file format" << std::endl;
            return false;
        }
    }

    return false;
}

bool searchInTables(const std::vector<std::string>& tableFiles, const std::string& targetHash, std::string& foundValue) {
    for (const auto& tableFile : tableFiles) {
        std::ifstream file(tableFile);
        if (file.is_open() && binarySearchInFile(file, targetHash, foundValue)) {
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
    } else {
        std::cout << "Value in the line after the hash: " << foundValue << std::endl;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);

    std::cout << "Search execution time: " << duration.count() << " ms" << std::endl;

    return 0;
}
