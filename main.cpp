#include <benchmark/benchmark.h>
#include <csv2.hpp>
#include <rapidcsv.h>
#include <parser.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <vector>

namespace fs = std::filesystem;

void set_label(benchmark::State& state, const std::string& filename)
{
    std::uintmax_t filesize = fs::file_size(filename);
    state.SetLabel(std::to_string(filesize));
}

static void BM_RAPID_SERIALIZATION(benchmark::State& state)
{
    std::string filename = "csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());
    std::istringstream ss(std::move(str));
    rapidcsv::Document doc(ss, rapidcsv::LabelParams(-1,-1));

    set_label(state, filename);
    for (auto _ : state)
    {
        {
            state.PauseTiming();
            std::stringstream s;
            state.ResumeTiming();
            doc.Save(s);
            state.PauseTiming();  // before killing stream and doc
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
    std::istringstream ss(std::move(str));

    set_label(state, filename);
    for (auto _ : state)
    {
        {
            rapidcsv::Document doc(ss, rapidcsv::LabelParams(-1,-1));
            benchmark::DoNotOptimize(doc);
            state.PauseTiming();  // before killing doc
        }
        state.ResumeTiming();
    }
}

namespace acsv = aria::csv;

static void BM_CSV_PARSER_DESERIALIZATION(benchmark::State& state)
{

    std::string filename = "csv/file" + std::to_string(state.range(0)) + ".csv";
    std::ifstream file(filename);
    std::string str((std::istreambuf_iterator<char>(file)),
                     std::istreambuf_iterator<char>());

    set_label(state, filename);
    for (auto _ : state)
    {
        {
            state.PauseTiming();
            std::istringstream ss(str);
            acsv::CsvParser parser(ss);
            state.ResumeTiming();

            for (const auto& row : parser)
            {
                benchmark::DoNotOptimize(row);
                for (const auto& field : row)
                {
                    benchmark::DoNotOptimize(field);
                }
            }
            state.PauseTiming();  // before killing stream and parser
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

    set_label(state, filename);
    for (auto _ : state)
    {
        {
            state.PauseTiming();
            std::ofstream out_fs;
            csv2::Writer<csv2::delimiter<','>> writer(out_fs);
            state.ResumeTiming();

            writer.write_rows(csv);

            state.PauseTiming();  // before killing stream and writer
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

    set_label(state, filename);
    for (auto _ : state)
    {
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
            state.PauseTiming(); // before killing reader
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
