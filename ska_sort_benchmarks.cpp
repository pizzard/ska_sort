/*
 * ska_sort_benchmarks.cpp
 *
 *  Created on: 08.11.2017
 *      Author: pizzard
 */

#include "ska_sort.hpp"
#include "benchmark/benchmark.h"

#include <random>
#include <deque>

//benchmark_inplace_sort/2M    103155817 ns  103115547 ns          7
//benchmark_inplace_sort/2M     73961470 ns   73923293 ns          9
//benchmark_inplace_sort/2M     69165349 ns   69165032 ns         10
//benchmark_ska_sort/2M        68850732 ns   68845594 ns         10
//benchmark_ska_sort/2M        67791104 ns   67794472 ns         10

#define SKA_SORT_NOINLINE __attribute__((noinline))

enum class DataTypes {
  vector_int32_t,
  vector_bool_float_pair,
  deque_bool,
  vector_uint8,
  vector_uint8_01,
  vector_uint8_geometric,
  vector_uint8_geometric_001,
  vector_uint16,
  vector_int64,
  vector_tuple_int64,
  vector_tuple_int32_int32_int64,
  vector_vector_int,
  vector_vector_string,
  vector_string,
  vector_vector_int_random_size
};

template <enum DataTypes>
auto  create_radix_sort_data(std::mt19937_64 & randomness, int size);

template <>
auto create_radix_sort_data<DataTypes::vector_int32_t>(std::mt19937_64 & randomness, int size)
{
    std::vector<int32_t> result;
    result.reserve(size);
    std::uniform_int_distribution<int32_t> distribution;
    for (int i = 0; i < size; ++i)
    {
        result.push_back(distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_bool_float_pair>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::pair<bool, float>> result;
    result.reserve(size);
    std::uniform_int_distribution<int> int_distribution(0, 1);
    std::uniform_real_distribution<float> real_distribution;
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness) != 0, real_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::deque_bool>(std::mt19937_64 & randomness, int size)
{
    std::deque<bool> result;
    std::uniform_int_distribution<int> int_distribution(0, 1);
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness) != 0);
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_uint8>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::int8_t> result;
    result.reserve(size);
    std::uniform_int_distribution<std::int8_t> int_distribution(-128, 127);
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_uint8_01>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::int8_t> result;
    result.reserve(size);
    std::uniform_int_distribution<std::int8_t> int_distribution(0, 1);
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_uint8_geometric>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::uint8_t> result;
    result.reserve(size);
    std::geometric_distribution<std::uint8_t> int_distribution(0.05);
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_uint8_geometric_001>(std::mt19937_64 & randomness, int size)
{
    std::vector<int> result;
    result.reserve(size);
    std::geometric_distribution<int> int_distribution(0.001);
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_uint16>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::int16_t> result;
    result.reserve(size);
    std::uniform_int_distribution<std::int16_t> int_distribution(-32768, 32767);
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_int64>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::int64_t> result;
    result.reserve(size);
    std::uniform_int_distribution<std::int64_t> int_distribution(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_tuple_int64>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::tuple<std::int64_t, std::int64_t>> result;
    result.reserve(size);
    std::uniform_int_distribution<std::int64_t> int_distribution(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int_distribution(randomness), int_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_tuple_int32_int32_int64>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::tuple<std::int32_t, std::int32_t, std::int64_t>> result;
    result.reserve(size);
    std::uniform_int_distribution<std::int32_t> int32_distribution(std::numeric_limits<int32_t>::lowest(), std::numeric_limits<int32_t>::max());
    std::uniform_int_distribution<std::int64_t> int64_distribution(std::numeric_limits<int64_t>::lowest(), std::numeric_limits<int64_t>::max());
    for (int i = 0; i < size; ++i)
    {
        result.emplace_back(int32_distribution(randomness), int32_distribution(randomness), int64_distribution(randomness));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_vector_int>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::vector<int>> result;
    result.reserve(size);
    std::uniform_int_distribution<int> size_distribution(0, 20);
    std::uniform_int_distribution<int> value_distribution;
    for (int i = 0; i < size; ++i)
    {
        std::vector<int> to_add(size_distribution(randomness));
        std::generate(to_add.begin(), to_add.end(), [&]{ return value_distribution(randomness); });
        result.push_back(std::move(to_add));
    }
    return result;
}

template <>
auto create_radix_sort_data<DataTypes::vector_vector_string>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::vector<std::string>> result;
    result.reserve(size);
    std::uniform_int_distribution<int> size_distribution(0, 10);
    std::uniform_int_distribution<int> string_length_distribution(0, 5);
    std::uniform_int_distribution<char> string_content_distribution('a', 'c');
    for (int i = 0; i < size; ++i)
    {
        std::vector<std::string> to_add(size_distribution(randomness));
        std::generate(to_add.begin(), to_add.end(), [&]
        {
#if 0
            std::string new_string = "hello";
            for (int i = 0, end = string_length_distribution(randomness); i != end; ++i)
                new_string.push_back('\0');
            std::generate(new_string.begin() + 5, new_string.end(), [&]
            {
                return string_content_distribution(randomness);
            });
#else
            std::string new_string(string_length_distribution(randomness), '\0');
            std::generate(new_string.begin(), new_string.end(), [&]
            {
                return string_content_distribution(randomness);
            });
#endif
            return new_string;
        });
        result.push_back(std::move(to_add));
    }
    return result;
}


template <>
auto create_radix_sort_data<DataTypes::vector_string>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::string> result;
    result.reserve(size);
    std::uniform_int_distribution<int> string_length_distribution(0, 20);
    std::uniform_int_distribution<char> string_content_distribution('a', 'z');
    for (int i = 0; i < size; ++i)
    {
        std::string to_add(string_length_distribution(randomness), '\0');
        std::generate(to_add.begin(), to_add.end(), [&]
        {
            return string_content_distribution(randomness);
        });
        result.push_back(std::move(to_add));
    }
    return result;
}



//extern const std::vector<const char *> & get_word_list();
//
//static std::vector<std::string> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
//{
//    const std::vector<const char *> & words = get_word_list();
//    std::vector<std::string> result;
//    result.reserve(size);
//    std::uniform_int_distribution<int> string_length_distribution(0, 10);
//    //std::uniform_int_distribution<int> string_length_distribution(1, 3);
//    std::uniform_int_distribution<size_t> word_picker(0, words.size() - 1);
//    for (int i = 0; i < size; ++i)
//    {
//        std::string to_add;
//        for (int i = 0, end = string_length_distribution(randomness); i < end; ++i)
//        {
//            to_add += words[word_picker(randomness)];
//        }
//        result.push_back(std::move(to_add));
//    }
//    return result;
//}

template <>
auto create_radix_sort_data<DataTypes::vector_vector_int_random_size>(std::mt19937_64 & randomness, int size)
{
    std::vector<std::vector<int>> result;
    std::uniform_int_distribution<int> random_size(0, 128);
    result.reserve(size);
    for (int i = 0; i < size; ++i)
    {
        std::vector<int> to_add(random_size(randomness));
        std::iota(to_add.begin(), to_add.end(), 0);
        result.push_back(std::move(to_add));
    }
    return result;
}
//
//template<size_t Size>
//struct SizedStruct
//{
//    uint8_t array[Size] = {};
//};
//template<>
//struct SizedStruct<0>
//{
//};
//#define SORT_ON_FIRST_ONLY
//typedef std::int64_t benchmark_sort_key;
//#define NUM_SORT_KEYS 1
//typedef SizedStruct<1016> benchmark_sort_value;
//#if NUM_SORT_KEYS == 1
//static std::vector<std::tuple<benchmark_sort_key, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
//{
//    std::vector<std::tuple<benchmark_sort_key, benchmark_sort_value>> result;
//    result.reserve(size);
//    std::uniform_int_distribution<benchmark_sort_key> distribution(std::numeric_limits<benchmark_sort_key>::lowest(), std::numeric_limits<benchmark_sort_key>::max());
//    for (int i = 0; i < size; ++i)
//    {
//        result.emplace_back(distribution(randomness), benchmark_sort_value());
//    }
//    return result;
//}
//#elif NUM_SORT_KEYS == 2
//static std::vector<std::tuple<std::pair<benchmark_sort_key, benchmark_sort_key>, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
//{
//    std::vector<std::tuple<std::pair<benchmark_sort_key, benchmark_sort_key>, benchmark_sort_value>> result;
//    result.reserve(size);
//    std::uniform_int_distribution<benchmark_sort_key> distribution(std::numeric_limits<benchmark_sort_key>::lowest(), std::numeric_limits<benchmark_sort_key>::max());
//    for (int i = 0; i < size; ++i)
//    {
//        result.emplace_back(std::make_pair(distribution(randomness), distribution(randomness)), benchmark_sort_value());
//    }
//    return result;
//}
//#else
//static std::vector<std::tuple<std::array<benchmark_sort_key, NUM_SORT_KEYS>, benchmark_sort_value>> SKA_SORT_NOINLINE create_radix_sort_data(std::mt19937_64 & randomness, int size)
//{
//    std::vector<std::tuple<std::array<benchmark_sort_key, NUM_SORT_KEYS>, benchmark_sort_value>> result;
//    result.reserve(size);
//    std::uniform_int_distribution<benchmark_sort_key> distribution(std::numeric_limits<benchmark_sort_key>::lowest(), std::numeric_limits<benchmark_sort_key>::max());
//    for (int i = 0; i < size; ++i)
//    {
//        std::array<benchmark_sort_key, NUM_SORT_KEYS> key;
//        for (int i = 0; i < NUM_SORT_KEYS; ++i)
//            key[i] = distribution(randomness);
//        result.emplace_back(key, benchmark_sort_value());
//    }
//    return result;
//}
//#endif


template <enum DataTypes val>
void benchmark_radix_sort_copy(benchmark::State & state)
{
    std::mt19937_64 randomness(77342348);
    auto to_sort = create_radix_sort_data<val>(randomness, state.range(0));
    typedef decltype(to_sort) cont;
    cont buffer(to_sort.size());
    benchmark::DoNotOptimize(buffer.data());
    buffer.clear();
    for (auto _ : state)
    {
        radix_sort(to_sort.begin(), to_sort.end(), buffer.begin());
        benchmark::ClobberMemory();
        buffer.clear();

    }
    state.SetItemsProcessed(state.iterations() * to_sort.size());
    state.SetBytesProcessed(state.iterations() * to_sort.size() * sizeof(typename cont::value_type));
}


template <enum DataTypes val>
void benchmark_ska_sort_copy(benchmark::State & state)
{
  std::mt19937_64 randomness(77342348);
  auto to_sort = create_radix_sort_data<val>(randomness, state.range(0));
  typedef decltype(to_sort) cont;
  cont buffer(to_sort.size());
  benchmark::DoNotOptimize(buffer.data());
  buffer.clear();
  for (auto _ : state)
  {
    ska_sort_copy(to_sort.begin(), to_sort.end(), buffer.begin());
    benchmark::ClobberMemory();
    buffer.clear();
  }
  state.SetItemsProcessed(state.iterations() * to_sort.size());
  state.SetBytesProcessed(state.iterations() * to_sort.size() * sizeof(typename cont::value_type));
}

template <enum DataTypes val>
static void benchmark_std_sort_copy(benchmark::State & state)
{
  std::mt19937_64 randomness(77342348);
  auto to_sort = create_radix_sort_data<val>(randomness, state.range(0));
  typedef decltype(to_sort) cont;
  cont buffer(to_sort.size());
  benchmark::DoNotOptimize(buffer.data());
  for (auto _ : state)
  {
    std::partial_sort_copy(to_sort.begin(), to_sort.end(), buffer.begin(), buffer.end());
    benchmark::ClobberMemory();
  }
  state.SetItemsProcessed(state.iterations() * to_sort.size());
  state.SetBytesProcessed(state.iterations() * to_sort.size() * sizeof(typename cont::value_type));
}


template <enum DataTypes val>
static void benchmark_radix_sort(benchmark::State & state)
{
  std::mt19937_64 randomness(77342348);
  auto to_sort = create_radix_sort_data<val>(randomness, state.range(0));
  typedef decltype(to_sort) cont;
  cont buffer(to_sort.size());
  benchmark::DoNotOptimize(buffer.data());
  buffer.clear();
  for (auto _ : state)
  {
      buffer = to_sort;
      benchmark::DoNotOptimize(buffer.data());
      inplace_radix_sort(buffer.begin(), buffer.end());
      benchmark::ClobberMemory();
      buffer.clear();
  }
  state.SetItemsProcessed(state.iterations() * to_sort.size());
  state.SetBytesProcessed(state.iterations() * to_sort.size() * sizeof(typename cont::value_type));
}

template <enum DataTypes val>
static void benchmark_ska_sort(benchmark::State & state)
{
  std::mt19937_64 randomness(77342348);
  auto to_sort = create_radix_sort_data<val>(randomness, state.range(0));
  typedef decltype(to_sort) cont;
  cont buffer(to_sort.size());
  benchmark::DoNotOptimize(buffer.data());
  buffer.clear();
  for (auto _ : state)
  {
      buffer = to_sort;
      benchmark::DoNotOptimize(buffer.data());
      ska_sort(buffer.begin(), buffer.end());
      benchmark::ClobberMemory();
      buffer.clear();
  }
  state.SetItemsProcessed(state.iterations() * to_sort.size());
  state.SetBytesProcessed(state.iterations() * to_sort.size() * sizeof(typename cont::value_type));
}

template <enum DataTypes val>
static void benchmark_std_sort(benchmark::State & state)
{
    std::mt19937_64 randomness(77342348);
    auto to_sort = create_radix_sort_data<val>(randomness, state.range(0));
    typedef decltype(to_sort) cont;
    cont buffer(to_sort.size());
    benchmark::DoNotOptimize(buffer.data());
    buffer.clear();
    for (auto _ : state)
    {
        buffer = to_sort;
        benchmark::DoNotOptimize(buffer.data());
        std::sort(buffer.begin(), buffer.end());
        benchmark::ClobberMemory();
        buffer.clear();
    }
    state.SetItemsProcessed(state.iterations() * to_sort.size());
    state.SetBytesProcessed(state.iterations() * to_sort.size() * sizeof(typename cont::value_type));
}


template <enum DataTypes val>
static void benchmark_american_flag_sort(benchmark::State & state)
{
    std::mt19937_64 randomness(77342348);
    auto to_sort = create_radix_sort_data<val>(randomness, state.range(0));
    typedef decltype(to_sort) cont;
    cont buffer(to_sort.size());
    benchmark::DoNotOptimize(buffer.data());
    buffer.clear();
    for (auto _ : state)
    {
        buffer = to_sort;
        benchmark::DoNotOptimize(buffer.data());
        american_flag_sort(buffer.begin(), buffer.end());
        benchmark::ClobberMemory();
        buffer.clear();
    }
    state.SetItemsProcessed(state.iterations() * to_sort.size());
    state.SetBytesProcessed(state.iterations() * to_sort.size() * sizeof(typename cont::value_type));
}



template <enum DataTypes val>
static void benchmark_generation(benchmark::State & state)
{
  std::mt19937_64 randomness(77342348);
  auto to_sort = create_radix_sort_data<val>(randomness, state.range(0));
  typedef decltype(to_sort) cont;
  cont buffer(to_sort.size());
  benchmark::DoNotOptimize(buffer.data());
  buffer.clear();
  for (auto _ : state)
  {
      buffer = to_sort;
      benchmark::DoNotOptimize(buffer.data());
      benchmark::ClobberMemory();
      buffer.clear();
  }
  state.SetItemsProcessed(state.iterations() * to_sort.size());
  state.SetBytesProcessed(state.iterations() * to_sort.size() * sizeof(typename cont::value_type));
}

static std::vector<std::int8_t> SKA_SORT_NOINLINE create_limited_radix_sort_data(std::mt19937_64 & randomness, int8_t range_end)
{
    int8_t permutation[256];
    std::iota(permutation, permutation + 256, -128);
    std::shuffle(permutation, permutation + 256, randomness);
    std::vector<std::int8_t> result;
    size_t size = 2 * 1024 * 1024;
    result.reserve(size);
    std::uniform_int_distribution<std::int8_t> int_distribution(-128, range_end);
    for (size_t i = 0; i < size; ++i)
    {
        result.emplace_back(permutation[detail::to_unsigned_or_bool(int_distribution(randomness))]);
    }
    return result;
}
static void benchmark_limited_generation(benchmark::State & state)
{
    std::mt19937_64 randomness(77342348);
    auto buffer = create_limited_radix_sort_data(randomness, state.range(0));
    for (auto _ : state)
    {
        auto to_sort = buffer;
        benchmark::DoNotOptimize(to_sort.data());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * buffer.size());
    state.SetBytesProcessed(state.iterations() * buffer.size() * sizeof(typename decltype(buffer)::value_type));
}


static void benchmark_limited_inplace_sort(benchmark::State & state)
{
    std::mt19937_64 randomness(77342348);
    auto buffer = create_limited_radix_sort_data(randomness, state.range(0));
    for (auto _ : state)
    {
        auto to_sort = buffer;
        benchmark::DoNotOptimize(to_sort.data());
        ska_sort(to_sort.begin(), to_sort.end());
        benchmark::ClobberMemory();
    }
    state.SetItemsProcessed(state.iterations() * buffer.size());
    state.SetBytesProcessed(state.iterations() * buffer.size() * sizeof(typename decltype(buffer)::value_type));
}

#define LIMITED_RANGE() Arg(-128)->Arg(-127)->Arg(-120)->Arg(-96)->Arg(-64)->Arg(-32)->Arg(0)->Arg(32)->Arg(64)->Arg(96)->Arg(127)
//BENCHMARK(benchmark_limited_generation)->LIMITED_RANGE();
//BENCHMARK(benchmark_limited_inplace_sort)->LIMITED_RANGE();

static constexpr int profile_multiplier = 2;
static constexpr int min_profile_range = 1 << 5;
static constexpr int max_profile_range = 1 << 20;

#define RANGE_ARGS() RangeMultiplier(profile_multiplier)->Range(min_profile_range, max_profile_range)

#define BENCHMARK_SUITE(DATA_TYPE) \
BENCHMARK_TEMPLATE(benchmark_radix_sort_copy, DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_ska_sort_copy  , DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_std_sort_copy  , DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_radix_sort, DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_ska_sort  , DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_std_sort  , DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_american_flag_sort, DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_generation, DATA_TYPE)->RANGE_ARGS();

#define REDUCED_BENCHMARK_SUITE(DATA_TYPE) \
BENCHMARK_TEMPLATE(benchmark_radix_sort, DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_ska_sort  , DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_std_sort  , DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_american_flag_sort, DATA_TYPE)->RANGE_ARGS(); \
BENCHMARK_TEMPLATE(benchmark_generation, DATA_TYPE)->RANGE_ARGS();

BENCHMARK_SUITE(DataTypes::vector_int32_t)
BENCHMARK_SUITE(DataTypes::vector_bool_float_pair)
BENCHMARK_SUITE(DataTypes::vector_uint8)
BENCHMARK_SUITE(DataTypes::vector_uint8_01)
BENCHMARK_SUITE(DataTypes::vector_uint8_geometric)
BENCHMARK_SUITE(DataTypes::vector_uint8_geometric_001)
BENCHMARK_SUITE(DataTypes::vector_uint16)
BENCHMARK_SUITE(DataTypes::vector_int64)


REDUCED_BENCHMARK_SUITE(DataTypes::vector_tuple_int64)
REDUCED_BENCHMARK_SUITE(DataTypes::vector_tuple_int32_int32_int64)
REDUCED_BENCHMARK_SUITE(DataTypes::vector_vector_int)
REDUCED_BENCHMARK_SUITE(DataTypes::vector_vector_string)
REDUCED_BENCHMARK_SUITE(DataTypes::vector_string)
REDUCED_BENCHMARK_SUITE(DataTypes::vector_vector_int_random_size)


BENCHMARK_MAIN();

