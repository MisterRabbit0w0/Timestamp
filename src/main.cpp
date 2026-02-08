#include <iomanip>
#include <iostream>
#include <stdexcept>

#include "high_res_timer.hpp"
#include "logger.hpp"

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

/**
 * @brief Print timing statistics in formatted output
 */
void printStatistics(const ::utils::TimingStats& stats) {
    logger << std::fixed << std::setprecision(2);
    logger << "\n========== Timing Statistics ==========\n"
           << "Intervals average (us): " << stats.average << "\n"
           << "Intervals 50th Percentile (us): " << stats.p50 << "\n"
           << "Intervals 75th Percentile (us): " << stats.p75 << "\n"
           << "Intervals 90th Percentile (us): " << stats.p90 << "\n"
           << "Intervals 95th Percentile (us): " << stats.p95 << "\n"
           << "Intervals 99th Percentile (us): " << stats.p99 << "\n"
           << "========================================\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            printUsage(argv[0]);
            return 1;
        }

        double intervalSec = ::utils::parseInterval(argv[1]);

        ts::HighResTimer timer(intervalSec);
        timer.run();

        ::utils::TimingStats stats = timer.calculateStatistics();

        logger << "interval = " << intervalSec << " s\n";
        printStatistics(stats);

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
