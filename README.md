# Timestamp

A high-precision interval timing tool that measures and analyzes timing accuracy over 100 iterations.

- **Dual timer**: `Timer` (ms, `system_clock`) for intervals >= 1ms, `HighResTimer` (us, `steady_clock`) for sub-millisecond intervals, automatically selected based on input
- **Statistical analysis**: Computes percentile statistics (p50, p75, p90, p95, p99)
- **Logging**: Automatic logging to timestamped `.log` files in the `logs/` directory, raw interval data written to log file only
- **Cross-platform**: Supports Windows, Linux, and macOS; Windows builds use `timeBeginPeriod` and thread priority elevation for improved precision

## Build

### Using CMake (Recommended)

```bash
git clone https://github.com/MisterRabbit0w0/Timestamp && cd Timestamp
cmake -S . -B build
cmake --build build --config Release
```

### Using g++ (Linux/macOS)

```bash
git clone https://github.com/MisterRabbit0w0/Timestamp && cd Timestamp
g++ -std=c++17 -O2 -I./include \
    src/main.cpp src/base_timer.cpp src/timer.cpp src/high_res_timer.cpp \
    src/utils.cpp src/logger.cpp \
    -o timer -lpthread
```

## Usage

```bash
./timer <seconds>
```

### Examples

```bash
./timer 1.0      # 1s interval, uses Timer (ms)
./timer 0.01     # 10ms interval, uses Timer (ms)
./timer 0.0005   # 500us interval, uses HighResTimer (us)
./timer 0.0001   # 100us interval, uses HighResTimer (us)
```

## Example Output

```
Start Timestamp (ms): 1707280123456
Timestamp (ms): 1707280124456	(real interval: 1000.12 ms)
Timestamp (ms): 1707280125456	(real interval: 999.98 ms)
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
