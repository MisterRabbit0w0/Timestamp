#include "timer.hpp"
#include "logger.hpp"

#include <iostream>
#include <iomanip>
#include <stdexcept>

/**
 * @brief Print usage information
 */
void printUsage(const char* programName) {
    std::cerr << "Usage: " << programName << " <seconds>\n"
              << "  seconds: Target interval duration in seconds (positive number)\n"
              << "Example: " << programName << " 1.0\n";
}

/**
 * @brief Print timing statistics in formatted output
 */
void printStatistics(const ts::TimingStats& stats) {
    logger << std::fixed << std::setprecision(2);
    logger << "\n========== Timing Statistics ==========\n"
              << "Intervals average (ms): " << stats.average << "\n"
              << "Intervals 50th Percentile (ms): " << stats.p50 << "\n"
              << "Intervals 75th Percentile (ms): " << stats.p75 << "\n"
              << "Intervals 90th Percentile (ms): " << stats.p90 << "\n"
              << "Intervals 95th Percentile (ms): " << stats.p95 << "\n"
              << "Intervals 99th Percentile (ms): " << stats.p99 << "\n"
              << "========================================\n";
}

int main(int argc, char* argv[]) {
    try {
        if (argc != 2) {
            printUsage(argv[0]);
            return 1;
        }

        double intervalSec = ts::utils::parseInterval(argv[1]);
        
        ts::Timer timer(intervalSec);
        timer.run();
        
        ts::TimingStats stats = timer.calculateStatistics();

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
