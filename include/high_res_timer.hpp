#pragma once

#include <chrono>
#include <cstddef>

#include "base_timer.hpp"

namespace ts {

class HighResTimer : public BaseTimer {
public:
    explicit HighResTimer(double intervalSec);

    void run(std::size_t iterations = 100) override;

    double getIntervalSec() const {
        return intervalSec_;
    }

private:
    double intervalSec_;
    std::chrono::steady_clock::time_point lastTimePoint_;
    std::chrono::steady_clock::time_point now() const;
};

}  // namespace ts
