#include "base_timer.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <stdexcept>

#include "logger.hpp"

namespace ts {

BaseTimer::BaseTimer(double intervalSec, const std::string& unit)
    : interval_(
          std::chrono::nanoseconds(static_cast<long long>(intervalSec * 1e9))),
      intervals_(),
      unit_(unit) {
    intervals_.reserve(100);
}

BaseTimer::~BaseTimer() {
    // Ensure output thread is stopped if still running
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stopOutputThread_ = true;
    }
    queueCV_.notify_all();
    if (outputThread_.joinable()) {
        outputThread_.join();
    }
}

void BaseTimer::startOutputThread() {
    stopOutputThread_ = false;
    outputThread_     = std::thread(&BaseTimer::outputWorker, this);
}

void BaseTimer::stopOutputThreadAndJoin() {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        stopOutputThread_ = true;
    }
    queueCV_.notify_all();
    if (outputThread_.joinable()) {
        outputThread_.join();
    }
}

void BaseTimer::enqueueOutput(const OutputData& data) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        outputQueue_.push(data);
    }
    queueCV_.notify_one();
}

void BaseTimer::outputWorker() {
    while (true) {
        OutputData data;
        bool hasData = false;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            queueCV_.wait(lock, [this] {
                return !outputQueue_.empty() || stopOutputThread_;
            });

            if (stopOutputThread_ && outputQueue_.empty()) {
                break;
            }

            if (!outputQueue_.empty()) {
                data    = outputQueue_.front();
                hasData = true;
                outputQueue_.pop();
            }
        }

        if (hasData) {
            if (data.type == OutputData::Type::Interval) {
                std::cout << "Timestamp (" << unit_ << "): " << data.timestamp
                          << "\t"
                          << "(real interval: " << data.realInterval << " "
                          << unit_ << ")\n";
            } else {
                std::cout << "Start Timestamp (" << unit_
                          << "): " << data.timestamp << "\n";
            }
        }
    }
}

::utils::TimingStats BaseTimer::calculateStatistics() const {
    if (intervals_.empty()) {
        throw std::runtime_error("No intervals collected");
    }

    ::utils::TimingStats stats{};

    double sum    = std::accumulate(intervals_.begin(), intervals_.end(), 0.0);
    stats.average = sum / intervals_.size();

    std::vector<double> sorted = intervals_;
    std::sort(sorted.begin(), sorted.end());

    stats.p50 = ::utils::calculatePercentile(sorted, 0.50);
    stats.p75 = ::utils::calculatePercentile(sorted, 0.75);
    stats.p90 = ::utils::calculatePercentile(sorted, 0.90);
    stats.p95 = ::utils::calculatePercentile(sorted, 0.95);
    stats.p99 = ::utils::calculatePercentile(sorted, 0.99);

    return stats;
}

void BaseTimer::printStatistics(const ::utils::TimingStats& stats) const {
    logger << std::fixed << std::setprecision(2);
    logger << "\n========== Timing Statistics ==========\n"
           << "Intervals average (" << unit_ << "): " << stats.average << "\n"
           << "Intervals 50th Percentile (" << unit_ << "): " << stats.p50
           << "\n"
           << "Intervals 75th Percentile (" << unit_ << "): " << stats.p75
           << "\n"
           << "Intervals 90th Percentile (" << unit_ << "): " << stats.p90
           << "\n"
           << "Intervals 95th Percentile (" << unit_ << "): " << stats.p95
           << "\n"
           << "Intervals 99th Percentile (" << unit_ << "): " << stats.p99
           << "\n"
           << "========================================\n";

    logger.fileOnly() << "\n========== Raw Interval Data (" << unit_
                      << ") ==========\n";
    for (std::size_t i = 0; i < intervals_.size(); ++i) {
        logger.fileOnly() << i + 1 << ": " << intervals_[i] << "\n";
    }
}

}  // namespace ts
