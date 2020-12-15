#include <rapidcsv.h>
#include <parser.hpp>
#include <benchmark/benchmark.h>

#include <iostream>
#include <fstream>

namespace acsv = aria::csv;

static void BM_RAPID_SERIALIZATION(benchmark::State& state)
{
    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    rapidcsv::Document doc{file};

    for (auto _ : state)
    {
        {
            state.PauseTiming();
            std::stringstream s;
            state.ResumeTiming();
            doc.Save(s);
            state.PauseTiming();  // before killing stream
        }
        state.ResumeTiming();
    }
}

static void BM_RAPID_DESERIALIZATION(benchmark::State& state)
{
    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    std::istringstream ss(str);
    for (auto _ : state)
    {
        {
            rapidcsv::Document doc{ss};
            benchmark::DoNotOptimize(doc);
            state.PauseTiming();
        }
        state.ResumeTiming();
    }
}

static void BM_CSV_PARSER_SERIALIZATION(benchmark::State& state)
{
    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    acsv::CsvParser doc(file);

    for (auto _ : state)
    {
        {
            state.PauseTiming();
            std::stringstream parsed_csv;
            state.ResumeTiming();
            for (auto& row : doc)
            {
              for (auto& field : row)
              {
                parsed_csv << field << ',';
              }
              parsed_csv << '\n';   // to make output like output of other libs
            }
            state.PauseTiming();  // before killing stream
        }
        state.ResumeTiming();
    }
}

static void BM_CSV_PARSER_DESERIALIZATION(benchmark::State& state)
{
    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    std::istringstream ss(str);
    for (auto _ : state)
    {
        {
            acsv::CsvParser doc{ss};
            benchmark::DoNotOptimize(doc);
            state.PauseTiming();
        }
        state.ResumeTiming();
    }
}

BENCHMARK(BM_RAPID_SERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_RAPID_DESERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV_PARSER_SERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV_PARSER_DESERIALIZATION)->DenseRange(0,9,1);

BENCHMARK_MAIN();
