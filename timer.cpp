#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
#include <thread>
#include <algorithm>

double validInterval(const char* arg) {
    try {
        std::istringstream iss(arg);
        double interval;
        iss >> interval;
        if (iss.fail() || !iss.eof() || interval <= 0) {
            throw std::invalid_argument("Invalid interval");
        }
        return interval;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << "\n";
        std::exit(1);
    }
}

/*
    @args interval: seconds
*/

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "usage: timer <seconds>\n";
        return 1;
    }
    double intervalSec = validInterval(argv[1]);
    auto interval = std::chrono::nanoseconds(static_cast<long long>(intervalSec * 1e9));

    auto last_tp = std::chrono::system_clock::now();

    std::vector<double> timestampsInterval;
    auto next_heartbeat = std::chrono::time_point_cast<
        std::chrono::nanoseconds>(last_tp);

    auto get_ms = [](std::chrono::system_clock::time_point tp) {
        return std::chrono::duration_cast<std::chrono::milliseconds>(
            tp.time_since_epoch()
        ).count();
    };
    
    std::cout << "Start Timestamp:" << get_ms(last_tp) << "\n";
    
    for (int i = 0; i < 100; ++i) {
        next_heartbeat += interval;

        std::this_thread::sleep_until(next_heartbeat);
        
        auto now_tp = std::chrono::system_clock::now();

        std::chrono::duration<double, std::milli> diff = now_tp - last_tp;
        timestampsInterval.push_back(diff.count());
        last_tp = now_tp;

        std::cout << "Timestamp:" << get_ms(now_tp) << "\t" <<
            "(real interval: " << diff.count() << " ms)\n";
    }

    // process timestampsInterval
    std::cout << "Intervals average (ms): ";
    double sum = 0.0;
    for (const auto& t : timestampsInterval) {
        sum += t;
    }
    double average = sum / timestampsInterval.size();
    std::cout << average << "\n";

    // process 50th percentile
    std::sort(timestampsInterval.begin(), timestampsInterval.end());
    
    // we have 100 intervals
    double p50 = timestampsInterval[49]; // 50th percentile
    std::cout << "Intervals 50th Percentile (ms): " << p50 << "\n";

    double p75 = timestampsInterval[74]; // 75th percentile
    std::cout << "Intervals 75th Percentile (ms): " << p75 << "\n";

    double p90 = timestampsInterval[89]; // 90th percentile
    std::cout << "Intervals 90th Percentile (ms): " << p90 << "\n";

    double p95 = timestampsInterval[94]; // 95th percentile
    std::cout << "Intervals 95th Percentile (ms): " << p95 << "\n";

    double p99 = timestampsInterval[98]; // 99th percentile
    std::cout << "Intervals 99th Percentile (ms): " << p99 << "\n";

    return 0;
}