#include "logger.hpp"

#include <iostream>
#include <fstream>
#include <chrono>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <filesystem>
#include <stdexcept>

namespace Logger {

Logger::Logger(const std::string& folderPath = "logs") {

    if (!checkFolderExists(folderPath)) {
        createFolder(folderPath);
    }

    std::string filename = generateFilename();
    std::string fullPath = folderPath + "/" + filename;

    if (checkFileExists(fullPath)) {
        throw std::runtime_error("Log file already exists: " + fullPath);
    }

    createLogFile(fullPath);
    
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

std::string Logger::generateFilename() {
    auto now = std::chrono::system_clock::now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm bt{};
#ifdef _WIN32
    localtime_s(&bt, &t);
#else
    localtime_r(&t, &bt);
#endif
    std::ostringstream oss;
    oss << "log_" << std::put_time(&bt, "%Y-%m-%d_%H-%M-%S") << ".log";
    return oss.str();
}

bool Logger::checkFolderExists(const std::string& folderPath) {
    return std::filesystem::exists(folderPath);
}

void Logger::createFolder(const std::string& folderPath) {
    try {
        std::filesystem::create_directories(folderPath);
    } catch (const std::filesystem::filesystem_error& e) {
        throw std::runtime_error("Failed to create log folder: " + folderPath + " - " + e.what());
    }
}

bool Logger::checkFileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

void Logger::createLogFile(const std::string& filePath) {
    std::ofstream ofs(filePath);
    if (!ofs) {
        throw std::runtime_error("Failed to create log file: " + filePath);
    }
    ofs.close();
}

} // namespace Logger

Logger::Logger logger;
