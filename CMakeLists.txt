# Minimum CMake version required
cmake_minimum_required(VERSION 3.14)

# Project name and version
project(VectorSearchEngine VERSION 1.0)

# Use C++23 standard (latest C++ with all modern features)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Disable compiler-specific extensions

# Enable compiler optimizations for release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -DNDEBUG")
# Debug builds with sanitizers (helps catch bugs)
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 -Wall -Wextra -pedantic")

# Find Boost libraries (we installed libboost-all-dev in Docker)
find_package(Boost REQUIRED)

# Include directories
include_directories(${Boost_INCLUDE_DIRS})
include_directories(${CMAKE_SOURCE_DIR}/src)

# Collect all source files from src/ directory (excluding tests)
file(GLOB_RECURSE SOURCES 
    "src/common/*.cpp"
    "src/distance/*.cpp"
    "src/index/*.cpp"
    "src/metrics/*.cpp"
    "src/main.cpp"
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

# ============================================
# Test Executable Configuration
# ============================================

# Build test executable for distance layer
add_executable(test_distance 
    tests/test_distance.cpp
    src/distance/distance.cpp
)

# Link required libraries for tests
target_link_libraries(test_distance pthread)

# Build test executable for vector store layer
add_executable(test_vector_store 
    tests/test_vector_store.cpp
    src/common/vector_store.cpp
    src/distance/distance.cpp
)

# Link required libraries for vector store tests
target_link_libraries(test_vector_store pthread)

# Build test executable for HNSW index
add_executable(test_hnsw
    tests/test_hnsw.cpp
    src/index/hnsw.cpp
    src/common/vector_store.cpp
    src/distance/distance.cpp
)

# Link required libraries for HNSW tests
target_link_libraries(test_hnsw pthread)

# Print some helpful info during build
message(STATUS "===========================================")
message(STATUS "Vector Search Engine Build Configuration")
message(STATUS "===========================================")
message(STATUS "CMake version: ${CMAKE_VERSION}")
message(STATUS "C++ Standard: C++${CMAKE_CXX_STANDARD}")
message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "===========================================")
