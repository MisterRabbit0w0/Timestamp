#include <iostream>
#include <memory>
#include <stdexcept>

#include "base_timer.hpp"
#include "high_res_timer.hpp"
#include "logger.hpp"
#include "timer.hpp"

void printUsage(const char* programName) {
    std::cerr
        << "Usage: " << programName << " <seconds>\n"
        << "  seconds: Target interval duration in seconds (positive number, "
           "supports sub-millisecond)\n"
        << "Example: " << programName << " 0.001  # 1ms interval\n"
        << "         " << programName << " 0.0001 # 100us interval\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            printUsage(argv[0]);
            return 1;
        }

        double intervalSec = ::utils::parseInterval(argv[1]);

        std::unique_ptr<ts::BaseTimer> timer;
        if (intervalSec < 0.002) {
            timer = std::make_unique<ts::HighResTimer>(intervalSec);
        } else {
            timer = std::make_unique<ts::Timer>(intervalSec);
        }

        timer->run();

        auto stats = timer->calculateStatistics();
        logger << "interval = " << intervalSec << " s\n";
        timer->printStatistics(stats);

        return 0;

    } catch (const std::invalid_argument& e) {
        std::cerr << "Error: " << e.what() << "\n";
        printUsage(argv[0]);
        return 1;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        return 1;
    }
}
