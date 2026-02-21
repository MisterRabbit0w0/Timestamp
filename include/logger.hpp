#pragma once

#include <fstream>
#include <iostream>
#include <string>

namespace logging {

class Logger {
public:
    /**
     * @brief open a file to write logs
     * @param folderPath The folder of the log file
     */
    explicit Logger(const std::string& folderPath = "logs");

    /** @brief close the log file */
    ~Logger();

    /**
     * @brief Log a value to both console and file
     * @param value The value to log
     */
    template <typename T>
    Logger& operator<<(const T& value) {
        std::cout << value;
        if (logFileOpened && file.is_open()) {
            file << value;
        }
        return *this;
    }

    /**
     * @brief Log manipulators (e.g., std::endl) to both console and file
     * @param manip The manipulator function
     */
    Logger& operator<<(std::ostream& (*manip)(std::ostream&)) {
        manip(std::cout);
        if (logFileOpened && file.is_open()) {
            manip(file);
        }
        return *this;
    }

    /**
     * @brief Log ios_base manipulators (e.g., std::fixed) to both console and
     * file
     * @param manip The ios_base manipulator function
     */
    Logger& operator<<(std::ios_base& (*manip)(std::ios_base&)) {
        manip(std::cout);
        if (logFileOpened && file.is_open()) {
            manip(file);
        }
        return *this;
    }

    /**
     * @brief Proxy that writes only to the log file, not to console
     */
    class FileOnlyProxy {
    public:
        explicit FileOnlyProxy(std::ofstream& f, bool open)
            : file_(f), open_(open) {}

        template <typename T>
        FileOnlyProxy& operator<<(const T& value) {
            if (open_)
                file_ << value;
            return *this;
        }

        FileOnlyProxy& operator<<(std::ostream& (*manip)(std::ostream&)) {
            if (open_)
                manip(file_);
            return *this;
        }

        FileOnlyProxy& operator<<(std::ios_base& (*manip)(std::ios_base&)) {
            if (open_)
                manip(file_);
            return *this;
        }

    private:
        std::ofstream& file_;
        bool open_;
    };

    /**
     * @brief Get a proxy that writes only to the log file
     */
    FileOnlyProxy fileOnly() {
        return FileOnlyProxy(file, logFileOpened && file.is_open());
    }

private:
    std::ofstream file;

    bool logFileOpened = false;

    std::string generateFilenameWithoutExtension();

    bool checkFolderExists(const std::string& folderPath);

    void createFolder(const std::string& folderPath);

    bool checkFileExists(const std::string& filePath);
};

}  // namespace logging

extern logging::Logger logger;