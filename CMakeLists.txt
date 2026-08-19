cmake_minimum_required(VERSION 3.10)
project(project1 CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Collect all library sources (excluding executable main files)
set(PROJECT_SOURCES
    src/CoilController.cpp
    src/CoilDriver.cpp
    src/ComfortStrategy.cpp
    src/DampingCalculator.cpp
    src/SensorReader.cpp
    src/SportStrategy.cpp
    src/SuspensionController.cpp
    src/TelemetryFormatter.cpp
    src/TelemetryLogger.cpp
)

# Main Application Target
add_executable(project1 src/main.cpp ${PROJECT_SOURCES})
target_include_directories(project1 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Test Target
enable_testing()
add_executable(unit_tests tests/test_main.cpp ${PROJECT_SOURCES})
target_include_directories(unit_tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME unit_tests COMMAND unit_tests)

# Add IoT Demo Test Target
add_executable(iot_test tests/test_iot.cpp)
target_include_directories(iot_test PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_test(NAME iot_test COMMAND iot_test)