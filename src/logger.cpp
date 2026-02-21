#include "logger.hpp"

#include <chrono>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>

namespace logging {

Logger::Logger(const std::string& folderPath) {
    if (!checkFolderExists(folderPath)) {
        createFolder(folderPath);
    }

    std::string filename = generateFilenameWithoutExtension();
    std::string fullPath = folderPath + "/" + filename + ".log";

    if (checkFileExists(fullPath)) {
        int i = 1;

        while (checkFileExists(folderPath + "/" + filename + "_" +
                               std::to_string(i) + ".log")) {
            ++i;
        }

        fullPath =
            folderPath + "/" + filename + "_" + std::to_string(i) + ".log";
    }

    file.open(fullPath, std::ios::out);
    if (!file.is_open()) {
        throw std::runtime_error("Failed to open log file: " + fullPath);
    }
    logFileOpened = true;
}

Logger::~Logger() {
    if (logFileOpened && file.is_open()) {
        file.close();
    }
}

std::string Logger::generateFilenameWithoutExtension() {
    auto now      = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm bt{};
#ifdef _WIN32
    localtime_s(&bt, &t);
#else
    localtime_r(&t, &bt);
#endif
    std::ostringstream oss;
    oss << "log_" << std::put_time(&bt, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}

bool Logger::checkFolderExists(const std::string& folderPath) {
    return std::filesystem::exists(folderPath);
}

void Logger::createFolder(const std::string& folderPath) {
    try {
        std::filesystem::create_directories(folderPath);
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Failed to create log folder: " + folderPath +
                                 " - " + e.what());
    }
}

bool Logger::checkFileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

}  // namespace logging

logging::Logger logger;
