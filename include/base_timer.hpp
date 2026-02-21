#pragma once

#include <chrono>
#include <condition_variable>
#include <cstddef>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "utils.hpp"

namespace ts {

class BaseTimer {
public:
    struct OutputData {
        enum class Type { Start, Interval };
        Type type;
        long long timestamp;
        double realInterval;
    };

    explicit BaseTimer(double intervalSec, const std::string& unit);
    virtual ~BaseTimer();

    // Disable copying and moving
    BaseTimer(const BaseTimer&)            = delete;
    BaseTimer& operator=(const BaseTimer&) = delete;
    BaseTimer(BaseTimer&&)                 = delete;
    BaseTimer& operator=(BaseTimer&&)      = delete;

    virtual void run(std::size_t iterations = 100) = 0;

    ::utils::TimingStats calculateStatistics() const;

    void printStatistics(const ::utils::TimingStats& stats) const;

    const std::vector<double>& getIntervals() const {
        return intervals_;
    }

protected:
    std::chrono::nanoseconds interval_;
    std::vector<double> intervals_;
    std::string unit_;

    void startOutputThread();
    void stopOutputThreadAndJoin();
    void enqueueOutput(const OutputData& data);

private:
    std::thread outputThread_;
    std::queue<OutputData> outputQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    bool stopOutputThread_ = false;

    void outputWorker();
};

}  // namespace ts
