#pragma once

#include <chrono>

/**
 * @brief Utility namespace for common operations
 */
namespace utils {

/**
 * @brief Structure to hold timing statistics
 */
struct TimingStats {
    double average;
    double p50;
    double p75;
    double p90;
    double p95;
    double p99;
};

/**
 * @brief Parse and validate interval argument
 * @param arg Command line argument string
 * @return Validated interval in seconds
 * @throws std::invalid_argument if argument is invalid
 */
double parseInterval(const char* arg);

/**
 * @brief Convert time point to milliseconds since epoch
 * @param tp Time point to convert
 * @return Milliseconds since epoch
 */
long long toMilliseconds(const std::chrono::system_clock::time_point& tp);

/**
 * @brief Convert steady_clock duration to milliseconds
 * @param dur Duration to convert
 * @return Milliseconds
 */
double toMilliseconds(const std::chrono::steady_clock::duration& dur);

/**
 * @brief Convert steady_clock time_point to microseconds since epoch
 * @param tp Time point to convert
 * @return Microseconds since epoch
 */
long long toMicroseconds(const std::chrono::steady_clock::time_point& tp);

/**
 * @brief Convert steady_clock duration to microseconds
 * @param dur Duration to convert
 * @return Microseconds
 */
double toMicroseconds(const std::chrono::steady_clock::duration& dur);

}  // namespace utils
