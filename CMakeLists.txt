cmake_minimum_required(VERSION 3.6)

project(InfinitimeDataExtractor)

set (CMAKE_CXX_STANDARD 17)
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/bin")
set (CMAKE_BUILD_TYPE Debug)

#include SimpleBLE
add_subdirectory(lib/SimpleBLE/simpleble ${CMAKE_BINARY_DIR}/simpleble)

#include Threads
find_package(Threads REQUIRED)

add_executable(extracting_values_test src/extracting_values_test.cpp
src/pinetime_communicator.cpp src/csv_handler.cpp
)
target_link_libraries(extracting_values_test simpleble::simpleble)
target_link_libraries(extracting_values_test Threads::Threads)

add_executable(data_extractor src/data_extractor.cpp
src/pinetime_communicator.cpp src/csv_handler.cpp)
target_link_libraries(data_extractor simpleble::simpleble)
target_link_libraries(data_extractor Threads::Threads)