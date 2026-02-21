#include "timer.hpp"

#include <chrono>
#include <iostream>
#include <thread>

#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
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

Timer::Timer(double intervalSec) : BaseTimer(intervalSec, "ms") {}

void Timer::run(std::size_t iterations) {
    intervals_.clear();
    intervals_.reserve(iterations);

#ifdef _WIN32
    TimerResolutionGuard timerGuard(1);
    BOOL result =
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
    if (!result) {
        std::cerr << "Warning: Failed to set thread priority. "
                  << "Timing precision may be affected.\n";
    }
#endif

    startOutputThread();

    lastTimePoint_ = std::chrono::system_clock::now();

    enqueueOutput({OutputData::Type::Start,
                   ::utils::toMilliseconds(lastTimePoint_), 0.0});

    auto nextHeartbeat =
        std::chrono::time_point_cast<std::chrono::nanoseconds>(lastTimePoint_);

    auto awakeBeforeHeartbeat =
        std::min(std::chrono::nanoseconds(10000000), interval_ / 2);

    for (std::size_t i = 0; i < iterations; ++i) {
        nextHeartbeat += interval_;
        std::this_thread::sleep_until(nextHeartbeat - awakeBeforeHeartbeat);

        while (std::chrono::system_clock::now() < nextHeartbeat) {
        }

        auto nowTp = std::chrono::system_clock::now();
        std::chrono::duration<double, std::milli> diff = nowTp - lastTimePoint_;

        double realInterval = diff.count();
        intervals_.push_back(realInterval);

        enqueueOutput({OutputData::Type::Interval,
                       ::utils::toMilliseconds(nowTp), realInterval});
        lastTimePoint_ = nowTp;
    }

    stopOutputThreadAndJoin();

#ifdef _WIN32
    BOOL restoreResult =
        SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
    if (!restoreResult) {
        std::cerr << "Warning: Failed to restore thread priority.\n";
    }
#endif
}

}  // namespace ts
