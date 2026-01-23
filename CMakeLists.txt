# Minimum CMake version required
cmake_minimum_required(VERSION 3.14)

# Project name and version
project(VectorSearchEngine VERSION 1.0)

# Use C++17 standard (modern C++ with good performance features)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Enable compiler optimizations for release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native")

# Find Boost libraries (we installed libboost-all-dev in Docker)
find_package(Boost REQUIRED)

# Include directories
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)

# Collect all source files from src/ directory
file(GLOB_RECURSE SOURCES 
    "src/*.cpp"
)

# If no source files exist yet, create a placeholder main
if(NOT SOURCES)
    message(STATUS "No source files found, will create placeholder")
    set(SOURCES "${CMAKE_SOURCE_DIR}/src/main.cpp")
endif()

# Create the executable
add_executable(vector-search-engine ${SOURCES})

# Link Boost libraries
target_link_libraries(vector-search-engine ${Boost_LIBRARIES} pthread)

# Installation rules (where to put the binary)
install(TARGETS vector-search-engine DESTINATION bin)

# Print some helpful info during build
message(STATUS "===========================================")
message(STATUS "Vector Search Engine Build Configuration")
message(STATUS "===========================================")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "===========================================")
