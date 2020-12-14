cmake_minimum_required(VERSION 3.5)

project(csv-libs-benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(csv-libs-benchmark main.cpp de_serialization.cpp)

# google-benchmark
find_package(benchmark REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)

# rapidcsv
add_library(rapidcsv rapidcsv/rapidcsv.h)
set(rapidcsv_INCLUDE_DIR rapidcsv)
target_include_directories(${PROJECT_NAME} PRIVATE ${rapidcsv_INCLUDE_DIR})
set_target_properties(rapidcsv PROPERTIES LINKER_LANGUAGE CXX)
