#include "../vector.hpp"
#include "../vector.cpp"

#include <random>
#include <vector>
#include <string>

#include <benchmark/benchmark.h>
#include <fmt/core.h>

void ConstructRandomVector(Vector<int>& vec, int sz) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  while(sz) {
    random_key = dist(mt);
    vec.PushBack(random_key);
    --sz;
  }
}

void ConstructRandomVector(std::vector<int>& vec, int sz) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  while(sz) {
    random_key = dist(mt);
    vec.push_back(random_key);
    --sz;
  }
}

////////////////////////////////////////////////////////////////////////////////
void BM_CustomVectorPushBack(benchmark::State& state) {
  Vector<int> vec;
  for (auto _ : state) {
    ConstructRandomVector(vec, state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdVectorPushBack(benchmark::State& state) {
  std::vector<int> vec;
  for (auto _ : state) {
    ConstructRandomVector(vec, state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void BM_CustomVectorMiddleInsert(benchmark::State& state) {
  Vector<int> vec;
  ConstructRandomVector(vec, 100);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i){
      vec.Insert(vec.Size() / 2, 50);
    }
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdVectorMiddleInsert(benchmark::State& state) {
  std::vector<int> vec;
  ConstructRandomVector(vec, 100);
  auto it = vec.begin();
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i){
      it = vec.begin();
      std::advance(it, vec.size() / 2);
      vec.insert(it, 50);
    }
  }
  state.SetComplexityN(state.range(0));
}


BENCHMARK(BM_CustomVectorPushBack)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdVectorPushBack)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CustomVectorMiddleInsert)->Range(1<<10, 1<<15)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdVectorMiddleInsert)->Range(1<<10, 1<<15)->Complexity()->Unit(benchmark::kMillisecond);

BENCHMARK_MAIN();