cmake_minimum_required(VERSION 3.5)

project(csv-libs-benchmark LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(csv-libs-benchmark main.cpp)

file(COPY
    ${CMAKE_CURRENT_LIST_DIR}/csv
    DESTINATION
    ${CMAKE_CURRENT_BINARY_DIR}
)

# google-benchmark
find_package(benchmark REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE benchmark::benchmark)


set(libs_INCLUDE_DIR libs)
target_include_directories(${PROJECT_NAME} PRIVATE ${libs_INCLUDE_DIR})
