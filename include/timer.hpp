#pragma once

#include <chrono>
#include <string>
#include <vector>

#include "utils.hpp"

namespace ts {

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
    const std::vector<double>& getIntervals() const {
        return intervals_;
    }

    /**
     * @brief Print timing statistics to console
     * @param stats TimingStats structure with calculated percentiles
     */
    void printStatistics(const ::utils::TimingStats& stats) const;

    /**
     * @brief Calculate statistics from collected intervals
     * @return TimingStats structure with calculated percentiles
     */
    ::utils::TimingStats calculateStatistics() const;

private:
    std::chrono::nanoseconds interval_;
    std::vector<double> intervals_;
    std::chrono::system_clock::time_point lastTimePoint_;

    void printTimestamp(const std::chrono::system_clock::time_point& tp,
                        double realInterval);
};

}  // namespace ts
