#include <rapidcsv.h>
#include <parser.hpp>
#include <csv2.hpp>
#include <benchmark/benchmark.h>

#include <iostream>
#include <fstream>
#include <vector>
#include <regex>

namespace acsv = aria::csv;

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
            rapidcsv::Document doc(ss, rapidcsv::LabelParams(-1,-1));
            benchmark::DoNotOptimize(doc);
            state.PauseTiming();
        }
        state.ResumeTiming();
    }
}

static void BM_RAPID_SERIALIZATION(benchmark::State& state)
{
    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    std::istringstream ss(str);
    rapidcsv::Document doc(ss, rapidcsv::LabelParams(-1,-1));

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

static void BM_CSV_PARSER_DESERIALIZATION(benchmark::State& state)
{

    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    for (auto _ : state)
    {
        {
            state.PauseTiming();
            std::istringstream ss(std::move(str));
            acsv::CsvParser parser(ss);
            std::ostringstream parsed_csv;
            state.ResumeTiming();

            for (const auto& row : parser)
            {
                benchmark::DoNotOptimize(row);
                for (const auto& field : row)
                {
                    benchmark::DoNotOptimize(field);
                }
            }
            state.PauseTiming();  // before killing streams
        }
        state.ResumeTiming();
    }
}

static void BM_CSV_PARSER_SERIALIZATION(benchmark::State& state)
{
    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    for (auto _ : state)
    {
        {
            state.PauseTiming();
            std::istringstream ss(std::move(str));
            acsv::CsvParser parser(ss);
            std::ostringstream parsed_csv;
            state.ResumeTiming();

            for (const auto& row : parser)
            {
              for (const auto& field : row)
              {
                parsed_csv << field << ',';
              }
              parsed_csv << '\n';   // to make output like output of other libs
            }
            state.PauseTiming();  // before killing streams
        }
        state.ResumeTiming();
    }
}

static void BM_CSV2_DESERIALIZATION(benchmark::State& state)
{
    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";

    for (auto _ : state)
    {
        {
            //state.PauseTiming();  // если вставить, увеличивает время теста на несколько порядков
            std::ifstream file(filename);
            std::string csv((std::istreambuf_iterator<char>(file)),
                             std::istreambuf_iterator<char>());
            csv2::Reader<csv2::first_row_is_header<false>> doc;
            //state.ResumeTiming();
            doc.parse(csv);
            state.PauseTiming();
        }
        state.ResumeTiming();
    }
}

using row_vec = std::vector<std::string>;
using csv_vec = std::vector<row_vec>;

csv_vec parse_csv_into_vector(const std::string& filename)
{
    const std::regex comma(",");
    std::ifstream file(filename);
    csv_vec answer;

    std::string line{};
    while (file && getline(file, line)) {
        std::vector<std::string> row{ std::sregex_token_iterator(line.begin(),line.end(),comma,-1), std::sregex_token_iterator() };
        answer.push_back(row);
    }
    return answer;
}


static void BM_CSV2_SERIALIZATION(benchmark::State& state)
{
    std::string filename = "../../csv/file" + std::to_string(state.range(0)) + ".csv";

    // how can i not to benchmark that...
    csv_vec csv = parse_csv_into_vector(filename);

    for (auto _ : state)
    {
        {
            state.PauseTiming();
            std::ofstream out_fs;
            csv2::Writer<csv2::delimiter<','>> writer(out_fs);
            state.ResumeTiming();

            writer.write_rows(csv);

            state.PauseTiming();  // before killing stream
        }
        state.ResumeTiming();
    }
}


BENCHMARK(BM_RAPID_SERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_RAPID_DESERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV_PARSER_SERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV_PARSER_DESERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV2_DESERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV2_SERIALIZATION)->DenseRange(0,9,1);

BENCHMARK_MAIN();
