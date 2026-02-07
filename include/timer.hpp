#pragma once

#include <chrono>
#include <vector>
#include <string>

namespace ts {

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
 * @brief High-precision interval timer for measuring timing accuracy
 */
class Timer {
public:
    /**
     * @brief Construct a new Timer with specified interval
     * @param intervalSec Target interval in seconds
     */
    explicit Timer(double intervalSec);

    /**
     * @brief Run the timing loop for specified number of iterations
     * @param iterations Number of timing iterations (default: 100)
     */
    void run(std::size_t iterations = 100);

    /**
     * @brief Get the collected interval measurements
     * @return Vector of interval durations in milliseconds
     */
    const std::vector<double>& getIntervals() const { return intervals_; }

    /**
     * @brief Calculate statistics from collected intervals
     * @return TimingStats structure with calculated percentiles
     */
    TimingStats calculateStatistics() const;

private:
    std::chrono::nanoseconds interval_;
    std::vector<double> intervals_;
    std::chrono::system_clock::time_point lastTimePoint_;

    void printTimestamp(const std::chrono::system_clock::time_point& tp, double realInterval);
};

/**
 * @brief Utility namespace for common operations
 */
namespace utils {
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
} // namespace utils

} // namespace ts
