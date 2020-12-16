#include <rapidcsv.h>
#include <parser.hpp>
#include <csv2.hpp>
#include <benchmark/benchmark.h>

#include <iostream>
#include <fstream>
#include <vector>

namespace acsv = aria::csv;

void set_label(benchmark::State& state)
{
    switch(state.range(0))
    {
    case 0:
        state.SetLabel(std::to_string(0.032));
        break;
    case 1:
        state.SetLabel(std::to_string(0.06));
        break;
    case 2:
        state.SetLabel(std::to_string(0.111));
        break;
    case 3:
        state.SetLabel(std::to_string(0.225));
        break;
    case 4:
        state.SetLabel(std::to_string(0.460));
        break;
    case 5:
        state.SetLabel(std::to_string(0.922));
        break;
    case 6:
        state.SetLabel(std::to_string(1.8));
        break;
    case 7:
        state.SetLabel(std::to_string(3.7));
        break;
    case 8:
        state.SetLabel(std::to_string(7.4));
        break;
    case 9:
        state.SetLabel(std::to_string(14.8));
        break;
    }
}

static void BM_RAPID_SERIALIZATION(benchmark::State& state)
{
    std::string filename = "csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    std::istringstream ss(str);
    rapidcsv::Document doc(ss, rapidcsv::LabelParams(-1,-1));

    for (auto _ : state)
    {
        state.PauseTiming();
        set_label(state);
        state.ResumeTiming();
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
    std::string filename = "csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    std::istringstream ss(str);
    for (auto _ : state)
    {
        state.PauseTiming();
        set_label(state);
        state.ResumeTiming();
        {
            rapidcsv::Document doc(ss, rapidcsv::LabelParams(-1,-1));
            benchmark::DoNotOptimize(doc);
            state.PauseTiming();
        }
        state.ResumeTiming();
    }
}

static void BM_CSV_PARSER_DESERIALIZATION(benchmark::State& state)
{

    std::string filename = "csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    for (auto _ : state)
    {
        state.PauseTiming();
        set_label(state);
        state.ResumeTiming();
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


using row_vec = std::vector<std::string>;
using csv_vec = std::vector<row_vec>;

csv_vec parse_csv_into_vector(const std::string& filename)
{
    csv_vec answer;

    csv2::Reader<csv2::delimiter<','>,
             csv2::quote_character<'"'>,
             csv2::first_row_is_header<false>,
             csv2::trim_policy::trim_whitespace> reader;
    reader.mmap(filename);

    for (const auto& row : reader)
    {
        row_vec cur_row;
        for (const auto& cell : row)
        {
            std::string cur_cell;
            cell.read_value(cur_cell);
            cur_row.emplace_back(cur_cell);
        }
        if (!cur_row.empty())
            answer.emplace_back(cur_row);
    }
    return answer;
}


static void BM_CSV2_SERIALIZATION(benchmark::State& state)
{
    std::string filename = "csv/file" + std::to_string(state.range(0)) + ".csv";

    csv_vec csv = parse_csv_into_vector(filename);

    for (auto _ : state)
    {
        state.PauseTiming();
        set_label(state);
        state.ResumeTiming();
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

static void BM_CSV2_DESERIALIZATION(benchmark::State& state)
{
    std::string filename = "csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string csv((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

    for (auto _ : state)
    {
        state.PauseTiming();
        set_label(state);
        state.ResumeTiming();
        {
            csv2::Reader<csv2::delimiter<','>,
                     csv2::quote_character<'"'>,
                     csv2::first_row_is_header<false>,
                     csv2::trim_policy::trim_whitespace> reader;
            reader.parse(csv);
            for (const auto& row : reader)
            {
                for (const auto& cell : row)
                {
                    std::string cellstr;
                    cell.read_value(cellstr);
                }
            }
            state.PauseTiming();
        }
        state.ResumeTiming();
    }
}


BENCHMARK(BM_RAPID_SERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_RAPID_DESERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV_PARSER_DESERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV2_SERIALIZATION)->DenseRange(0,9,1);
BENCHMARK(BM_CSV2_DESERIALIZATION)->DenseRange(0,9,1);

BENCHMARK_MAIN();
