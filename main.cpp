#include "de_serialization.hpp"

#include <rapidcsv.h>
#include <benchmark/benchmark.h>

#include <iostream>
#include <fstream>

namespace csv_file
{
const std::string tiny = "../../csv/tiny.csv";
const std::string medium = "../../csv/medium.csv";
const std::string huge = "../../csv/huge.csv";

const std::string cur = medium;
}  // csv_file

namespace mb = my_benchmark;

static void BM_RAPID_SERIALIZATION(benchmark::State& state)
{
    std::ifstream file(csv_file::cur);
    rapidcsv::Document doc{file};

    for (auto _ : state)
        mb::rapid_serialization(doc, state);
}

static void BM_RAPID_DESERIALIZATION(benchmark::State& state)
{
    std::ifstream file(csv_file::cur);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    std::istringstream stream(str);
    for (auto _ : state)
    {
        mb::rapid_deserialization(stream, state);
    }

}

BENCHMARK(BM_RAPID_SERIALIZATION);
BENCHMARK(BM_RAPID_DESERIALIZATION);


BENCHMARK_MAIN();
