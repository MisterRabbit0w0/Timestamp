#pragma once

#include <chrono>
#include <condition_variable>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
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
     * @brief Destructor - stops output thread if running
     */
    ~HighResTimer();

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

    /**
     * @brief Print timing statistics to console
     * @param stats TimingStats structure with calculated percentiles
     */
    void printStatistics(const ::utils::TimingStats& stats) const;

private:
    struct OutputData {
        long long timestampUs;
        double realIntervalUs;
    };

    double intervalSec_;
    std::chrono::nanoseconds interval_;
    std::vector<double> intervals_;
    std::chrono::steady_clock::time_point lastTimePoint_;

    // Output thread and thread-safe queue
    std::thread outputThread_;
    std::queue<OutputData> outputQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    bool stopOutputThread_;

    /**
     * @brief Get current time with highest available resolution
     * @return Time point with microsecond precision
     */
    std::chrono::steady_clock::time_point now() const;

    void printTimestamp(long long timestampUs, double realIntervalUs);
    void outputWorker();
};

}  // namespace ts
