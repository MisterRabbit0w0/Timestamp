#include "timer.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <stdexcept>
#include <thread>

#include "logger.hpp"

namespace ts {

Timer::Timer(double intervalSec)
    : interval_(
          std::chrono::nanoseconds(static_cast<long long>(intervalSec * 1e9))),
      intervals_() {
    intervals_.reserve(100);
}

void Timer::run(std::size_t iterations) {
    lastTimePoint_ = std::chrono::system_clock::now();

    std::cout << "Start Timestamp:" << utils::toMilliseconds(lastTimePoint_)
              << "\n";

    auto nextHeartbeat =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(lastTimePoint_);

    auto awakeBeforeHeartbeat = std::chrono::nanoseconds(10000000);  // 10 ms

    for (std::size_t i = 0; i < iterations; ++i) {
        nextHeartbeat += interval_;
        std::this_thread::sleep_until(nextHeartbeat - awakeBeforeHeartbeat);

        while (std::chrono::system_clock::now() < nextHeartbeat) {
        }

        auto nowTp = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> diff = nowTp - lastTimePoint_;

        double realInterval = diff.count();
        intervals_.push_back(realInterval);

        printTimestamp(nowTp, realInterval);
        lastTimePoint_ = nowTp;
    }
}

::utils::TimingStats Timer::calculateStatistics() const {
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
        std::size_t index = static_cast<std::size_t>(p * sorted.size()) - 1;
        return sorted[index];
    };

    stats.p50 = percentile(0.50);
    stats.p75 = percentile(0.75);
    stats.p90 = percentile(0.90);
    stats.p95 = percentile(0.95);
    stats.p99 = percentile(0.99);

    return stats;
}

void Timer::printTimestamp(const std::chrono::system_clock::time_point& tp,
                           double realInterval) {
    std::cout << "Timestamp:" << ::utils::toMilliseconds(tp) << "\t"
              << "(real interval: " << realInterval << " ms)\n";
}

void Timer::printStatistics(const ::utils::TimingStats& stats) const {
    logger << std::fixed << std::setprecision(2);
    logger << "\n========== Timing Statistics ==========\n"
           << "Intervals average (ms): " << stats.average << "\n"
           << "Intervals 50th Percentile (ms): " << stats.p50 << "\n"
           << "Intervals 75th Percentile (ms): " << stats.p75 << "\n"
           << "Intervals 90th Percentile (ms): " << stats.p90 << "\n"
           << "Intervals 95th Percentile (ms): " << stats.p95 << "\n"
           << "Intervals 99th Percentile (ms): " << stats.p99 << "\n"
           << "========================================\n";
}

}  // namespace ts
