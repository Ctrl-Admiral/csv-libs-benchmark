#ifndef DE_SERIALIZATION_HPP
#define DE_SERIALIZATION_HPP

#include <rapidcsv.h>
#include <benchmark/benchmark.h>

namespace my_benchmark
{

void rapid_serialization(rapidcsv::Document& doc, benchmark::State& state);
void rapid_deserialization(std::istringstream& ss, benchmark::State& state);

}  // my_benchmark

#endif // DE_SERIALIZATION_HPP
