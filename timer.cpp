#include <iostream>
#include <chrono>
#include <vector>
#include <sstream>
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
    double interval = validInterval(argv[1]);

    std::vector<double> timestampsInterval;
    
    int counter = 1;
    
    auto timer = std::chrono::high_resolution_clock::now();

    auto duration = timer.time_since_epoch();

    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
    
    std::cout << "Timestamp:" << milliseconds << "\n";
    
    while (counter <= 101) {
        
        auto current = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> delta = current - timer;
        
        // count use second
        if (delta.count() >= interval) {
            counter++;

            // use milliseconds
            timestampsInterval.push_back(delta.count() * 1000);
            
            timer = current;
            duration = timer.time_since_epoch();
            milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
            std::cout << "Timestamp:" << milliseconds << "\n";
        }
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