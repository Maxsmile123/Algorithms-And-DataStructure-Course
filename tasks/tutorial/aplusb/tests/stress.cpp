
#include <benchmark/benchmark.h>
#include <fmt/core.h>

#include "../aplusb.hpp"

////////////////////////////////////////////////////////////////////////////////
void BM_CustomSum(benchmark::State& state) {
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      Sum(i, i);
    }
  }
  state.SetComplexityN(state.range(0));
}

void BM_MutByTwo(benchmark::State& state) {
  auto f = [](int x) {
    return x * 2;
  };

  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i) {
      f(i);
    }
  }
  state.SetComplexityN(state.range(0));
}


BENCHMARK(BM_CustomSum)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_MutByTwo)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();