#include "timer.hpp"

#include <chrono>
#include <thread>

#include "utils.hpp"

namespace ts {

Timer::Timer(double intervalSec) : BaseTimer(intervalSec, "ms") {}

void Timer::run(std::size_t iterations) {
    intervals_.clear();
    intervals_.reserve(iterations);

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
}

}  // namespace ts
