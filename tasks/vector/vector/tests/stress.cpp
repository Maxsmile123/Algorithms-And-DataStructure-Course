#include <random>
#include <map>
#include <string>

#include <benchmark/benchmark.h>
#include <fmt/core.h>

#include "../map.hpp"

void ConstructRandomMap(Map<int, int>& mp, int sz) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  while(sz) {
    random_key = dist(mt);
    mp.Insert(std::pair{random_key, 1});
    --sz;
  }
}

void ConstructRandomMap(std::map<int, int>& mp, int sz) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  while(sz) {
    random_key = dist(mt);
    mp.insert(std::pair{random_key, 1});
    --sz;
  }
}

void ConstructLinearMap(Map<int, int>& mp, int sz) {
  while(sz) {
    mp.Insert(std::pair{sz, 1});
    --sz;
  }
}

void ConstructLinearMap(std::map<int, int>& mp, int sz) {
  while(sz) {
    mp.insert(std::pair{sz, 1});
    --sz;
  }
}

////////////////////////////////////////////////////////////////////////////////
void BM_CustomMapRandomInsert(benchmark::State& state) {
  Map<int, int> mp;
  for (auto _ : state) {
    ConstructRandomMap(mp, state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdMapRandomInsert(benchmark::State& state) {
  std::map<int, int> mp;
  for (auto _ : state) {
    ConstructRandomMap(mp, state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void BM_CustomMapLinearInsert(benchmark::State& state) {
  Map<int, int> mp;
  for (auto _ : state) {
    ConstructLinearMap(mp, state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdMapLinearInsert(benchmark::State& state) {
  std::map<int, int> mp;
  for (auto _ : state) {
    ConstructLinearMap(mp, state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void BM_CustomMapErase(benchmark::State& state) {
  Map<int, int> mp;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomMap(mp, state.range(0));
    state.ResumeTiming();
    for (int64_t i = 0; i < state.range(0); ++i) {
      state.PauseTiming();
      random_key = dist(mt);
      state.ResumeTiming();
      try{
        mp.Erase(random_key);
      } catch(std::runtime_error&){}
    }
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdMapErase(benchmark::State& state) {
  std::map<int, int> mp;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomMap(mp, state.range(0));
    state.ResumeTiming();
    for (int64_t i = 0; i < state.range(0); ++i) {
      state.PauseTiming();
      random_key = dist(mt);
      state.ResumeTiming();
      try{
        mp.erase(random_key);
      } catch(std::runtime_error&){}
    }
  }
  state.SetComplexityN(state.range(0));
}

void BM_CustomMapClear(benchmark::State& state) {
  Map<int, int> mp;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomMap(mp, state.range(0));
    state.ResumeTiming();
    mp.Clear();
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdMapClear(benchmark::State& state) {
  std::map<int, int> mp;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomMap(mp, state.range(0));
    state.ResumeTiming();
    mp.clear();
  }
  state.SetComplexityN(state.range(0));
}


BENCHMARK(BM_CustomMapRandomInsert)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdMapRandomInsert)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CustomMapLinearInsert)->Range(1<<10, 1<<15)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdMapLinearInsert)->Range(1<<10, 1<<15)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CustomMapErase)->Range(1<<10, 1<<17)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdMapErase)->Range(1<<10, 1<<17)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CustomMapClear)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdMapClear)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();