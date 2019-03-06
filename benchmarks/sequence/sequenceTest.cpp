// For best results use `sudo cpupower frequency-set -g performance` first.
// Collect JSON results using `bin/sequenceTest --benchmark_format=json`.

#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <limits>
#include <list>
#include <random>
#include <set>
#include <vector>
#include <benchmark/benchmark.h>


class LargeObject {
  std::array<int,16> data;
public:
  LargeObject(int value) {
    data[0] = value;
  }

  friend bool
  operator<(const LargeObject &l, const LargeObject &r) {
    return l.data[0] < r.data[0];
  }
  friend bool
  operator==(const LargeObject &l, const LargeObject &r) {
    return l.data[0] == r.data[0];
  }
};


//constexpr size_t maxElementCount = 134217758;
//constexpr size_t maxElementCount = 67108864;
//constexpr size_t maxElementCount = 4194304;
//constexpr size_t maxElementCount = 65536;
//constexpr size_t maxElementCount = 32768;
//constexpr size_t maxElementCount = 16384;
constexpr size_t maxElementCount = 262144;


#define BENCHMARK_SAMPLES(func, ...)                                           \
        BENCHMARK_TEMPLATE(func, __VA_ARGS__)                                  \
          ->Arg(32)                                                            \
          ->Arg(512)                                                           \
          ->Arg(1024)                                                          \
          ->Arg(2048)                                                          \
          ->Arg(4096)                                                          \
          ->Arg(6144)                                                          \
          ->Arg(8192)                                                          \
          ->Arg(16384)                                                         \
          ->Arg(32768)                                                         \
          ->Arg(65536)                                                         \
          ->Arg(131072)                                                        \
          ->Arg(maxElementCount)                                               \
;
//          ->Arg(262144)                                                        \
          ->Arg(524288)                                                        \
          ->Arg(1048576)                                                       \
          ->Arg(2097152)                                                       \
          ->Arg(4194304)                                                       \
          ->Arg(8388608)                                                       \
          ->Arg(16777216)                                                      \
          ->Arg(33554432)                                                      \
          ->Arg(67108864)                                                      \


std::vector<int> randomInts(maxElementCount);

struct Initializer {
  Initializer() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(std::numeric_limits<int>::min(),
                                           std::numeric_limits<int>::max());
    for (auto &element : randomInts) {
      element = dis(gen);
    }
  }
} globalDataInitializerObject;


template<class Sequence>
static void
testPushBack(benchmark::State& state) {
  const typename Sequence::value_type val = 11235;
  while (state.KeepRunning()) {
    Sequence sequence;
    size_t count = state.range(0);
    while (count) {
      sequence.push_back(val);
      --count;
    }
  }
  const size_t items = state.iterations() * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(val));
}
BENCHMARK_SAMPLES(testPushBack, std::vector<int>);
BENCHMARK_SAMPLES(testPushBack, std::list<int>);
BENCHMARK_SAMPLES(testPushBack, std::deque<int>);
BENCHMARK_SAMPLES(testPushBack, std::vector<LargeObject>);
BENCHMARK_SAMPLES(testPushBack, std::list<LargeObject>);
BENCHMARK_SAMPLES(testPushBack, std::deque<LargeObject>);


template<class Sequence>
static void
testPushFront(benchmark::State& state) {
  const typename Sequence::value_type val = 11235;
  while (state.KeepRunning()) {
    Sequence sequence;
    size_t count = state.range(0);
    while (count) {
      sequence.insert(sequence.begin(), val);
      --count;
    }
  }
  const size_t items = state.iterations() * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(val));
}
/*BENCHMARK_SAMPLES(testPushFront, std::vector<int>);
BENCHMARK_SAMPLES(testPushFront, std::list<int>);
BENCHMARK_SAMPLES(testPushFront, std::deque<int>);
BENCHMARK_SAMPLES(testPushFront, std::vector<LargeObject>);
BENCHMARK_SAMPLES(testPushFront, std::list<LargeObject>);
BENCHMARK_SAMPLES(testPushFront, std::deque<LargeObject>);
*/

template<class Sequence>
static void
testNaiveInsert(benchmark::State& state) {
  while (state.KeepRunning()) {
    Sequence s;
    size_t count = state.range(0);
    auto value = randomInts.begin();
    while (count) {
      const auto &v = *value;
      auto pos = std::find_if(s.begin(), s.end(),
        [&v] (auto elt) { return !(elt < v); });
      s.insert(pos, v);
      ++value;
      --count;
    }
  }
  const size_t items = state.iterations() * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(typename Sequence::value_type));
}
BENCHMARK_SAMPLES(testNaiveInsert, std::vector<int>);
BENCHMARK_SAMPLES(testNaiveInsert, std::list<int>);
BENCHMARK_SAMPLES(testNaiveInsert, std::deque<int>);
BENCHMARK_SAMPLES(testNaiveInsert, std::vector<LargeObject>);
BENCHMARK_SAMPLES(testNaiveInsert, std::list<LargeObject>);
BENCHMARK_SAMPLES(testNaiveInsert, std::deque<LargeObject>);


template<class Sequence>
static void
testAddThenSort(benchmark::State& state) {
  while (state.KeepRunning()) {
    Sequence s;
    size_t count = state.range(0);
    auto value = randomInts.begin();
    while (count) {
      const auto &v = *value;
      s.push_back(v);
      --count;
    }
    std::sort(s.begin(), s.end());
  }
  const size_t items = state.iterations() * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(typename Sequence::value_type));
}

template<typename Element>
static void
testLinkedListAddThenSort(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::list<Element> s;
    size_t count = state.range(0);
    auto value = randomInts.begin();
    while (count) {
      const auto &v = *value;
      s.push_back(v);
      --count;
    }
    s.sort();
  }
  const size_t items = state.iterations() * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(Element));
}

template<>
void
testAddThenSort<std::list<int>>(benchmark::State& state) {
  testLinkedListAddThenSort<int>(state);
}

template<>
void
testAddThenSort<std::list<LargeObject>>(benchmark::State& state) {
  testLinkedListAddThenSort<LargeObject>(state);
}

BENCHMARK_SAMPLES(testAddThenSort, std::vector<int>);
BENCHMARK_SAMPLES(testAddThenSort, std::list<int>);
BENCHMARK_SAMPLES(testAddThenSort, std::deque<int>);
BENCHMARK_SAMPLES(testAddThenSort, std::vector<LargeObject>);
BENCHMARK_SAMPLES(testAddThenSort, std::list<LargeObject>);
BENCHMARK_SAMPLES(testAddThenSort, std::deque<LargeObject>);


template<class Sequence>
static void
testInsert(benchmark::State& state) {
  while (state.KeepRunning()) {
    Sequence s;
    size_t count = state.range(0);
    auto value = randomInts.begin();
    while (count) {
      const auto &v = *value;
      auto pos = std::lower_bound(s.begin(), s.end(), v);
      s.insert(pos, v);
      ++value;
      --count;
    }
  }
  const size_t items = state.iterations() * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(typename Sequence::value_type));
}

template<>
void
testInsert<std::multiset<int>>(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::multiset<int> s;
    size_t count = state.range(0);
    auto value = randomInts.begin();
    while (count) {
      const auto &v = *value;
      s.insert(v);
      ++value;
      --count;
    }
  }
  const size_t items = state.iterations() * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(int));
}

template<>
void
testInsert<std::multiset<LargeObject>>(benchmark::State& state) {
  while (state.KeepRunning()) {
    std::multiset<LargeObject> s;
    size_t count = state.range(0);
    auto value = randomInts.begin();
    while (count) {
      const auto &v = *value;
      s.insert(v);
      ++value;
      --count;
    }
  }
  const size_t items = state.iterations() * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(LargeObject));
}

BENCHMARK_SAMPLES(testInsert, std::vector<int>);
BENCHMARK_SAMPLES(testInsert, std::list<int>);
BENCHMARK_SAMPLES(testInsert, std::deque<int>);
BENCHMARK_SAMPLES(testInsert, std::multiset<int>);
BENCHMARK_SAMPLES(testInsert, std::vector<LargeObject>);
BENCHMARK_SAMPLES(testInsert, std::list<LargeObject>);
BENCHMARK_SAMPLES(testInsert, std::deque<LargeObject>);
BENCHMARK_SAMPLES(testInsert, std::multiset<LargeObject>);


BENCHMARK_MAIN();

