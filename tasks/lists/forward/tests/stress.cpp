#include <random>
#include <forward_list>
#include <string>

#include <benchmark/benchmark.h>
#include <fmt/core.h>

#include "../forward_list.hpp"

void ConstructRandomList(ForwardList<int>& list, int sz) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  while(sz) {
    random_key = dist(mt);
    list.PushFront(random_key);
    --sz;
  }
}

void ConstructRandomList(std::forward_list<int>& list, int sz) {
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  while(sz) {
    random_key = dist(mt);
    list.push_front(random_key);
    --sz;
  }
}

////////////////////////////////////////////////////////////////////////////////
void BM_CustomListPushFront(benchmark::State& state) {
  ForwardList<int> list;
  for (auto _ : state) {
    ConstructRandomList(list, state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdListPushFront(benchmark::State& state) {
  std::forward_list<int> list;
  for (auto _ : state) {
    ConstructRandomList(list, state.range(0));
  }
  state.SetComplexityN(state.range(0));
}

void BM_CustomListMiddleInsert(benchmark::State& state) {
  ForwardList<int> list;
  ConstructRandomList(list, 100);
  auto it = list.Begin();
  std::advance(it, 50);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i){
      list.InsertAfter(it, 50);
    }
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdListMiddleInsert(benchmark::State& state) {
  std::forward_list<int> list;
  ConstructRandomList(list, 100);
  auto it = list.begin();
  std::advance(it, 50);
  for (auto _ : state) {
    for (int i = 0; i < state.range(0); ++i){
      list.insert_after(it, 50);
    }
  }
  state.SetComplexityN(state.range(0));
}

void BM_CustomListErase(benchmark::State& state) {
  ForwardList<int> list;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomList(list, state.range(0));
    state.ResumeTiming();
    for (int64_t i = 0; i < state.range(0); ++i) {
      list.EraseAfter(list.Begin());
    }
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdListErase(benchmark::State& state) {
  std::forward_list<int> list;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomList(list, state.range(0));
    state.ResumeTiming();
    for (int64_t i = 0; i < state.range(0); ++i) {
      list.erase_after(list.begin());
    }
  }
  state.SetComplexityN(state.range(0));
}

void BM_CustomListClear(benchmark::State& state) {
  ForwardList<int> list;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomList(list, state.range(0));
    state.ResumeTiming();
    list.Clear();
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdListClear(benchmark::State& state) {
  std::forward_list<int> list;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomList(list, state.range(0));
    state.ResumeTiming();
    list.clear();
  }
  state.SetComplexityN(state.range(0));
}

void BM_CustomListFind(benchmark::State& state) {
  ForwardList<int> list;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomList(list, state.range(0));
    state.ResumeTiming();
    state.PauseTiming();
    random_key = dist(mt);
    state.ResumeTiming();
    list.Find(random_key);
  }
  state.SetComplexityN(state.range(0));
}

void BM_StdListFind(benchmark::State& state) {
  std::forward_list<int> list;
  std::random_device rd;
  std::mt19937 mt(rd());
  std::uniform_int_distribution<int> dist(INT_MIN, INT_MAX);
  int random_key;
  for (auto _ : state) {
    state.PauseTiming();
    ConstructRandomList(list, state.range(0));
    state.ResumeTiming();
    state.PauseTiming();
    random_key = dist(mt);
    state.ResumeTiming();
    std::find(list.begin(), list.end(), random_key);
  }
  state.SetComplexityN(state.range(0));
}


BENCHMARK(BM_CustomListPushFront)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdListPushFront)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CustomListMiddleInsert)->Range(1<<10, 1<<15)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdListMiddleInsert)->Range(1<<10, 1<<15)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CustomListErase)->Range(1<<10, 1<<17)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdListErase)->Range(1<<10, 1<<17)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CustomListClear)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdListClear)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_CustomListFind)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);
BENCHMARK(BM_StdListFind)->Range(1<<10, 1<<20)->Complexity()->Unit(benchmark::kMillisecond);


BENCHMARK_MAIN();