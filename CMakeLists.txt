cmake_minimum_required(VERSION 3.5)

project(csv-libs-benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(csv-libs-benchmark main.cpp)

# google-benchmark
find_package(benchmark REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)

# rapidcsv
add_library(rapidcsv libs/rapidcsv.h)
set_target_properties(rapidcsv PROPERTIES LINKER_LANGUAGE CXX)

# csv-parser
add_library(csv-parser libs/parser.hpp)
set_target_properties(csv-parser PROPERTIES LINKER_LANGUAGE CXX)

# csv2
add_library(csv2 libs/csv2.hpp)
set_target_properties(csv2 PROPERTIES LINKER_LANGUAGE CXX)

set(libs_INCLUDE_DIR libs)
target_include_directories(${PROJECT_NAME} PRIVATE ${libs_INCLUDE_DIR})
