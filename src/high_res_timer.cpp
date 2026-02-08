#include "high_res_timer.hpp"

#include <algorithm>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "utils.hpp"

namespace ts {

HighResTimer::HighResTimer(double intervalSec)
    : intervalSec_(intervalSec),
      interval_(
          std::chrono::nanoseconds(static_cast<long long>(intervalSec * 1e9))),
      intervals_() {
    intervals_.reserve(100);
}

std::chrono::steady_clock::time_point HighResTimer::now() const {
    return std::chrono::steady_clock::now();
}

void HighResTimer::run(std::size_t iterations) {
    lastTimePoint_ = now();

    std::cout << "Start Timestamp (us):"
              << ::utils::toMicroseconds(lastTimePoint_) << "\n";

    auto nextHeartbeat =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(lastTimePoint_);

    auto awakeBeforeHeartbeat = std::chrono::nanoseconds(
        1000000);  // 1 ms (reduced for higher precision)

    for (std::size_t i = 0; i < iterations; ++i) {
        nextHeartbeat += interval_;
        std::this_thread::sleep_until(nextHeartbeat - awakeBeforeHeartbeat);

        // Busy-wait loop for higher precision on sub-millisecond intervals
        while (now() < nextHeartbeat) {
            // Yield to reduce CPU usage while waiting
            std::this_thread::yield();
        }

        auto nowTp          = now();
        auto diff           = nowTp - lastTimePoint_;
        double realInterval = ::utils::toMicroseconds(diff);

        intervals_.push_back(realInterval);

        printTimestamp(realInterval);
        lastTimePoint_ = nowTp;
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
    std::cout << "Timestamp (us):" << ::utils::toMicroseconds(now()) << "\t"
              << "(real interval: " << realInterval << " us)\n";
}

}  // namespace ts
