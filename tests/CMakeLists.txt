# Find Google Test
find_package(GTest QUIET)

if(GTEST_FOUND)
    # Add test sources
    set(TEST_SOURCES
        test_text_processor.cpp
    )

    # Create test executable
    add_executable(text_reference_breaker_tests ${TEST_SOURCES})

    # Include directories
    target_include_directories(text_reference_breaker_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/include
        ${CMAKE_SOURCE_DIR}/lib/aho_corasick/src
        ${GTEST_INCLUDE_DIRS}
    )

    # Link libraries
    target_link_libraries(text_reference_breaker_tests PRIVATE
        OpenMP::OpenMP_CXX
        nlohmann_json::nlohmann_json
        ${GTEST_LIBRARIES}
        pthread
    )

    # Add test
    add_test(NAME text_reference_breaker_tests COMMAND text_reference_breaker_tests)
    
    message(STATUS "Google Test found, tests will be built")
else()
    message(STATUS "Google Test not found, tests will be skipped")
endif() 