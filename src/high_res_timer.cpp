#include "high_res_timer.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <numeric>
#include <queue>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "logger.hpp"
#include "utils.hpp"

namespace ts {

HighResTimer::HighResTimer(double intervalSec)
    : intervalSec_(intervalSec),
      interval_(
          std::chrono::nanoseconds(static_cast<long long>(intervalSec * 1e9))),
      intervals_(),
      stopOutputThread_(false) {
    intervals_.reserve(100);
}

HighResTimer::~HighResTimer() {
    // Signal the output thread to stop and wait for it
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stopOutputThread_ = true;
    }
    queueCV_.notify_all();
    if (outputThread_.joinable()) {
        outputThread_.join();
    }
}

std::chrono::steady_clock::time_point HighResTimer::now() const {
    return std::chrono::steady_clock::now();
}

void HighResTimer::run(std::size_t iterations) {
    // Start the output worker thread
    stopOutputThread_ = false;
    outputThread_     = std::thread(&HighResTimer::outputWorker, this);

    lastTimePoint_ = now();

    std::cout << "Start Timestamp (us):"
              << ::utils::toMicroseconds(lastTimePoint_) << "\n";

    // not use heartbeat approach for sub-millisecond intervals, instead use
    // busy-wait loop
    auto nextHeartbeat =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(lastTimePoint_);

    for (std::size_t i = 0; i < iterations; ++i) {
        nextHeartbeat += interval_;

        // Busy-wait loop for higher precision on sub-millisecond intervals
        while (now() < nextHeartbeat) {
            // Yield to reduce CPU usage while waiting
            std::this_thread::yield();
        }

        auto nowTp          = now();
        auto diff           = nowTp - lastTimePoint_;
        double realInterval = ::utils::toMicroseconds(diff);

        intervals_.push_back(realInterval);

        // use another thread to print timestamp to avoid affecting timing
        // accuracy
        printTimestamp(realInterval);
        lastTimePoint_ = nowTp;
    }

    // Signal the output thread to stop and wait for it to finish
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stopOutputThread_ = true;
    }
    queueCV_.notify_all();
    if (outputThread_.joinable()) {
        outputThread_.join();
    }
}

::utils::TimingStats HighResTimer::calculateStatistics() const {
    if (intervals_.empty()) {
        throw std::runtime_error("No intervals collected");
    }

    ::utils::TimingStats stats{};

    // Calculate average
    double sum    = std::accumulate(intervals_.begin(), intervals_.end(), 0.0);
    stats.average = sum / intervals_.size();

    // Calculate percentiles (requires sorted data)
    std::vector<double> sorted = intervals_;
    std::sort(sorted.begin(), sorted.end());

    auto percentile = [&sorted](double p) -> double {
        std::size_t index = static_cast<std::size_t>(p * sorted.size());
        if (index >= sorted.size())
            index = sorted.size() - 1;
        if (sorted.empty())
            return 0.0;
        return sorted[index];
    };

    stats.p50 = percentile(0.50);
    stats.p75 = percentile(0.75);
    stats.p90 = percentile(0.90);
    stats.p95 = percentile(0.95);
    stats.p99 = percentile(0.99);

    return stats;
}

void HighResTimer::printTimestamp(double realInterval) {
    // Create the output string and push to queue for async printing
    std::ostringstream oss;
    oss << "Timestamp (us):" << ::utils::toMicroseconds(now()) << "\t"
        << "(real interval: " << realInterval << " us)\n";

    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        outputQueue_.push(oss.str());
    }
    queueCV_.notify_one();
}

void HighResTimer::outputWorker() {
    while (true) {
        std::string output;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCV_.wait(lock, [this] {
                return !outputQueue_.empty() || stopOutputThread_;
            });

            // Exit if stopped and queue is empty
            if (stopOutputThread_ && outputQueue_.empty()) {
                break;
            }

            if (!outputQueue_.empty()) {
                output = outputQueue_.front();
                outputQueue_.pop();
            }
        }

        // Print outside the lock
        if (!output.empty()) {
            std::cout << output;
        }
    }
}

void HighResTimer::printStatistics(const ::utils::TimingStats& stats) const {
    logger << std::fixed << std::setprecision(2);
    logger << "\n========== Timing Statistics ==========\n"
           << "Intervals average (us): " << stats.average << "\n"
           << "Intervals 50th Percentile (us): " << stats.p50 << "\n"
           << "Intervals 75th Percentile (us): " << stats.p75 << "\n"
           << "Intervals 90th Percentile (us): " << stats.p90 << "\n"
           << "Intervals 95th Percentile (us): " << stats.p95 << "\n"
           << "Intervals 99th Percentile (us): " << stats.p99 << "\n"
           << "========================================\n";
}

}  // namespace ts
