#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "high_res_timer.hpp"
#include "logger.hpp"
#include "timer.hpp"

/**
 * @brief Print usage information
 */
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

        if (intervalSec < 0.001) {  // 1 ms threshold for high-resolution timer
            ts::HighResTimer timer(intervalSec);
            timer.run();

            ::utils::TimingStats stats = timer.calculateStatistics();

            logger << "interval = " << intervalSec << " s\n";
            timer.printStatistics(stats);
        } else {
            ts::Timer timer(intervalSec);
            timer.run();

            ::utils::TimingStats stats = timer.calculateStatistics();

            logger << "interval = " << intervalSec << " s\n";
            timer.printStatistics(stats);
        }

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
