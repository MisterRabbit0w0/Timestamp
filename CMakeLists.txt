cmake_minimum_required(VERSION 3.10)

project(TimestampProject)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set output directory to project root for all platforms and configurations
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL ${CMAKE_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO ${CMAKE_SOURCE_DIR})

# Source files
set(SOURCES
    src/main.cpp
    src/timer.cpp
    src/high_res_timer.cpp
    src/utils.cpp
    src/logger.cpp
)

# Create executable
add_executable(timer ${SOURCES})

# Include directories (target-scoped)
target_include_directories(timer PRIVATE ${CMAKE_SOURCE_DIR}/include)

# Link winmm on Windows for timeBeginPeriod
if(WIN32)
    target_link_libraries(timer winmm)
endif()
