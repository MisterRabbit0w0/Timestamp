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

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <mmsystem.h>

namespace {

// RAII wrapper for timeBeginPeriod/timeEndPeriod to ensure cleanup
class TimerResolutionGuard {
public:
    explicit TimerResolutionGuard(UINT period) : period_(period) {
        timeBeginPeriod(period_);
    }

    ~TimerResolutionGuard() {
        timeEndPeriod(period_);
    }

    // Disable copying
    TimerResolutionGuard(const TimerResolutionGuard&)            = delete;
    TimerResolutionGuard& operator=(const TimerResolutionGuard&) = delete;

private:
    UINT period_;
};

}  // anonymous namespace

#endif

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
    intervals_.clear();
    intervals_.reserve(iterations);

#ifdef _WIN32
    // Request 1ms timer resolution on Windows to improve scheduling precision
    TimerResolutionGuard timerGuard(1);
    // Set current thread to high priority to reduce preemption
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
#endif

    // Start the output worker thread
    stopOutputThread_ = false;
    outputThread_     = std::thread(&HighResTimer::outputWorker, this);

    // Warm up to stabilize CPU frequency and cache
    for (int i = 0; i < 1000; ++i) {
        now();
    }

    lastTimePoint_     = now();
    auto nextHeartbeat = lastTimePoint_;

    // Initial output before starting the timed loop to avoid affecting the
    // first interval
    {
        // use a special value
        std::lock_guard<std::mutex> lock(queueMutex_);
        outputQueue_.push({::utils::toMicroseconds(lastTimePoint_), -1.0});
    }

    for (std::size_t i = 0; i < iterations; ++i) {
        nextHeartbeat += interval_;

        // Busy-wait loop for higher precision on sub-millisecond intervals
        while (now() < nextHeartbeat) {
            // No yield for high precision sub-millisecond timing to avoid OS
            // scheduling latency
        }

        auto nowTp          = now();
        auto diff           = nowTp - lastTimePoint_;
        double realInterval = ::utils::toMicroseconds(diff);

        intervals_.push_back(realInterval);

        // use another thread to print timestamp to avoid affecting timing
        // accuracy
        printTimestamp(::utils::toMicroseconds(nowTp), realInterval);
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

#ifdef _WIN32
    // Restore thread priority (timer resolution is automatically restored by
    // TimerResolutionGuard)
    SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
#endif
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

void HighResTimer::printTimestamp(long long timestampUs,
                                  double realIntervalUs) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        outputQueue_.push({timestampUs, realIntervalUs});
    }
    queueCV_.notify_one();
}

void HighResTimer::outputWorker() {
    while (true) {
        OutputData data;
        bool hasData = false;
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
                data    = outputQueue_.front();
                hasData = true;
                outputQueue_.pop();
            }
        }

        // Print outside the lock
        if (hasData) {
            if (data.realIntervalUs != -1.0) {
                std::cout << "Timestamp (us):" << data.timestampUs << "\t"
                          << "(real interval: " << data.realIntervalUs
                          << " us)\n";
            } else {
                std::cout << "Start Timestamp (us):" << data.timestampUs
                          << "\n";
            }
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
