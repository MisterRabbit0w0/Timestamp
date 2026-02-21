#pragma once

#include <chrono>
#include <cstddef>

#include "base_timer.hpp"

namespace ts {

class Timer : public BaseTimer {
public:
    explicit Timer(double intervalSec);

    void run(std::size_t iterations = 100) override;

private:
    std::chrono::system_clock::time_point lastTimePoint_;
};

}  // namespace ts
