#include "high_res_timer.hpp"

#include <iostream>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <windows.h>

#include <mmsystem.h>

namespace {

class TimerResolutionGuard {
public:
    explicit TimerResolutionGuard(UINT period) : period_(period) {
        timeBeginPeriod(period_);
    }

    ~TimerResolutionGuard() {
        timeEndPeriod(period_);
    }

    TimerResolutionGuard(const TimerResolutionGuard&)            = delete;
    TimerResolutionGuard& operator=(const TimerResolutionGuard&) = delete;

private:
    UINT period_;
};

}  // anonymous namespace

#endif

#include "utils.hpp"

namespace ts {

HighResTimer::HighResTimer(double intervalSec)
    : BaseTimer(intervalSec, "us"), intervalSec_(intervalSec) {}

std::chrono::steady_clock::time_point HighResTimer::now() const {
    return std::chrono::steady_clock::now();
}

void HighResTimer::run(std::size_t iterations) {
    intervals_.clear();
    intervals_.reserve(iterations);

#ifdef _WIN32
    TimerResolutionGuard timerGuard(1);
    BOOL result =
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
    if (!result) {
        std::cerr << "Warning: Failed to set thread priority to highest. "
                  << "Timing precision may be affected.\n";
    }
#endif

    startOutputThread();

    // Warm up to stabilize CPU frequency and cache
    for (int i = 0; i < 1000; ++i) {
        now();
    }

    lastTimePoint_     = now();
    auto nextHeartbeat = lastTimePoint_;

    enqueueOutput({OutputData::Type::Start,
                   ::utils::toMicroseconds(lastTimePoint_), 0.0});

    for (std::size_t i = 0; i < iterations; ++i) {
        nextHeartbeat += interval_;

        while (now() < nextHeartbeat) {
        }

        auto nowTp          = now();
        auto diff           = nowTp - lastTimePoint_;
        double realInterval = ::utils::toMicroseconds(diff);

        intervals_.push_back(realInterval);

        enqueueOutput({OutputData::Type::Interval,
                       ::utils::toMicroseconds(nowTp), realInterval});
        lastTimePoint_ = nowTp;
    }

    stopOutputThreadAndJoin();

#ifdef _WIN32
    BOOL restoreResult =
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    if (!restoreResult) {
        std::cerr << "Warning: Failed to restore thread priority to normal.\n";
    }
#endif
}

}  // namespace ts
