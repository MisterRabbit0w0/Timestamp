#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "utils.hpp"

namespace ts {

/**
 * @brief High-resolution timer for sub-millisecond interval measurement
 * Uses steady_clock for accurate timing and provides microsecond precision
 */
class HighResTimer {
public:
    /**
     * @brief Construct a new HighResTimer with specified interval in seconds
     * @param intervalSec Target interval in seconds (supports sub-millisecond)
     */
    explicit HighResTimer(double intervalSec);

    /**
     * @brief Run the timing loop for specified number of iterations
     * @param iterations Number of timing iterations (default: 100)
     */
    void run(std::size_t iterations = 100);

    /**
     * @brief Get the collected interval measurements in microseconds
     * @return Vector of interval durations in microseconds
     */
    const std::vector<double>& getIntervals() const {
        return intervals_;
    }

    /**
     * @brief Calculate statistics from collected intervals
     * @return TimingStats structure with calculated percentiles
     */
    ::utils::TimingStats calculateStatistics() const;

    /**
     * @brief Get the configured interval in seconds
     * @return Interval in seconds
     */
    double getIntervalSec() const {
        return intervalSec_;
    }

private:
    double intervalSec_;
    std::chrono::nanoseconds interval_;
    std::vector<double> intervals_;
    std::chrono::steady_clock::time_point lastTimePoint_;

    /**
     * @brief Get current time with highest available resolution
     * @return Time point with microsecond precision
     */
    std::chrono::steady_clock::time_point now() const;

    void printTimestamp(double realInterval);
};

}  // namespace ts
