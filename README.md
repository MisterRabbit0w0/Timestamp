# Timestamp

A high-precision interval timing tool that measures and analyzes timing accuracy over 100 iterations.

## Args

`interval`: Target interval duration in seconds (must be a positive number)

## Outputs

The program outputs the following to console and log file:

- **Start Timestamp**: Milliseconds since epoch when the timer starts
- **Per-iteration timestamps**: Each iteration prints the current timestamp and the actual interval duration in milliseconds
- **Statistics (after 100 iterations)**:
  - Average interval (ms)
  - 50th percentile (ms)
  - 75th percentile (ms)
  - 90th percentile (ms)
  - 95th percentile (ms)
  - 99th percentile (ms)

Log files are automatically created in the `logs/` directory with timestamped filenames.

## Build

### Option 1: Using CMake (Recommended)

```bash
git clone https://github.com/MisterRabbit0w0/Timestamp && cd Timestamp
mkdir build && cd build
cmake ..
cmake --build .
cd ..
```

### Option 2: Using g++

```bash
git clone https://github.com/MisterRabbit0w0/Timestamp && cd Timestamp
g++ -std=c++17 -I./include src/main.cpp src/timer.cpp src/logger.cpp -o timer
```

## Run

```bash
./timer <seconds>
```

### Example:

```bash
./timer 1.0    # Target 1 second intervals
./timer 0.5    # Target 500ms intervals
```

## Example Output

```
Start Timestamp:1707280123456
Timestamp:1707280124456	(real interval: 1000.12 ms)
Timestamp:1707280125456	(real interval: 999.98 ms)
...
========== Timing Statistics ==========
Intervals average (ms): 1000.05
Intervals 50th Percentile (ms): 1000.02
Intervals 75th Percentile (ms): 1000.08
Intervals 90th Percentile (ms): 1000.15
Intervals 95th Percentile (ms): 1000.25
Intervals 99th Percentile (ms): 1000.45
========================================
```

## Requirements

- C++17 compatible compiler
- CMake 3.10+ (for CMake build)

## Features

- **High-precision timing**: Uses `std::chrono::steady_clock` for accurate interval measurements
- **Statistical analysis**: Computes percentile statistics (50th, 75th, 90th, 95th, 99th) for timing analysis
- **Logging**: Automatic logging to timestamped files in the `logs/` directory
- **Cross-platform**: Supports Windows, Linux, and macOS
