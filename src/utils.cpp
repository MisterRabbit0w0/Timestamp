#include "utils.hpp"

#include <sstream>
#include <stdexcept>

namespace utils {

double parseInterval(const char* arg) {
    std::istringstream iss(arg);
    double interval;
    iss >> interval;

    if (iss.fail() || !iss.eof() || interval <= 0) {
        throw std::invalid_argument(
            "Invalid interval: must be a positive number");
    }
    return interval;
}

long long toMilliseconds(const std::chrono::system_clock::time_point& tp) {
    return std::chrono::duration_cast<std::chrono::milliseconds>(
               tp.time_since_epoch())
        .count();
}

double toMilliseconds(const std::chrono::steady_clock::duration& dur) {
    return std::chrono::duration<double, std::milli>(dur).count();
}

long long toMicroseconds(const std::chrono::steady_clock::time_point& tp) {
    return std::chrono::duration_cast<std::chrono::microseconds>(
               tp.time_since_epoch())
        .count();
}

double toMicroseconds(const std::chrono::steady_clock::duration& dur) {
    return std::chrono::duration<double, std::micro>(dur).count();
}

double calculatePercentile(const std::vector<double>& sortedData, double p) {
    if (sortedData.empty()) return 0.0;
    std::size_t index = static_cast<std::size_t>(p * sortedData.size());
    if (index >= sortedData.size()) index = sortedData.size() - 1;
    return sortedData[index];
}

}  // namespace utils