cmake_minimum_required(VERSION 3.10)
project(text_reference_breaker VERSION 1.0)

# Set C++11 standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Add json library
include(FetchContent)
FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz
)
FetchContent_MakeAvailable(json)

# Add source files
set(SOURCES
    src/main.cpp
    src/text_processor.cpp
    src/utils.cpp
)

# Add header files
set(HEADERS
    include/text_processor.hpp
    include/utils.hpp
)

# Create executable
add_executable(text_reference_breaker ${SOURCES} ${HEADERS})

# Include directories
target_include_directories(text_reference_breaker PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/lib/aho_corasick/src
)

# Link libraries
target_link_libraries(text_reference_breaker PRIVATE
    OpenMP::OpenMP_CXX
    nlohmann_json::nlohmann_json
) 