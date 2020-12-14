#include "de_serialization.hpp"

#include <rapidcsv.h>
#include <benchmark/benchmark.h>

#include <fstream>

namespace my_benchmark
{

void rapid_serialization(rapidcsv::Document& doc, benchmark::State& state)
{
    state.PauseTiming();
    {
    std::stringstream s;
    state.ResumeTiming();
    doc.Save(s);
    state.PauseTiming();  // before killing stream
    }
    state.ResumeTiming();
}

void rapid_deserialization(std::istringstream& ss, benchmark::State& state)
{
    {
    rapidcsv::Document doc{ss};
    benchmark::DoNotOptimize(doc);
    state.PauseTiming();
    }
    state.ResumeTiming();
}

}  // my_benchmark
