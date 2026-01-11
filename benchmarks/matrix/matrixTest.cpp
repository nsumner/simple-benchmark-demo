// For best results use `sudo cpupower frequency-set -g performance` first.
// Collect JSON results using
// `bin/matrixTest --benchmark_out_format=json --benchmark_out=results.json`.

#include <random>
#include <benchmark/benchmark.h>


constexpr size_t maxElementCount = 81920;

// For a machine with 16GB RAM, this is reasonable. More than this seems
// to start exceeding available RAM.
// constexpr size_t maxElementCount = 53248;
//
// Around 26GB for a matrix of ints:
// constexpr size_t maxElementCount = 81920;

#define BENCHMARK_SAMPLES(func, ...)                                           \
        BENCHMARK_TEMPLATE(func, __VA_ARGS__)                                  \
          ->Arg(32)                                                            \
          ->Arg(512)                                                           \
          ->Arg(1024)                                                          \
          ->Arg(2048)                                                          \
          ->Arg(4096)                                                          \
          ->Arg(6144)                                                          \
          ->Arg(8192)                                                          \
          ->Arg(10240)                                                         \
          ->Arg(12288)                                                         \
          ->Arg(14336)                                                         \
          ->Arg(16384)                                                         \
          ->Arg(18432)                                                         \
          ->Arg(20480)                                                         \
          ->Arg(22528)                                                         \
          ->Arg(24576)                                                         \
          ->Arg(26624)                                                         \
          ->Arg(28672)                                                         \
          ->Arg(30720)                                                         \
          ->Arg(32768)                                                         \
          ->Arg(34816)                                                         \
          ->Arg(36864)                                                         \
          ->Arg(40960)                                                         \
          ->Arg(45056)                                                         \
          ->Arg(49152)                                                         \
          ->Arg(53248)                                                         \
          ->Arg(57344)                                                         \
          ->Arg(61440)                                                         \
          ->Arg(65536)                                                         \
          ->Arg(69632)                                                         \
          ->Arg(73728)                                                         \
          ->Arg(77824)                                                         \
          ->Arg(maxElementCount)                                               \
          ->Complexity(benchmark::oAuto)                                       \
;


using Element = int;

constexpr size_t bufferSize = maxElementCount * maxElementCount;
std::vector<Element> randomElements(bufferSize);


////////////////////////////////////////////////////////////////////////////////
// We start by filling a vector with random values. We will treat this vector
// as if it were a matrix represented in row major order. We can benchmark
// smaller matrix sizes by only traversing a portion of the vector later.
////////////////////////////////////////////////////////////////////////////////


struct Initializer {
  Initializer() {
    std::random_device rd;
    std::mt19937 gen{rd()};
    using Limits = std::numeric_limits<Element>;
    std::uniform_int_distribution<Element> dis{Limits::min(), Limits::max()};
    for (auto& element : randomElements) {
      element = dis(gen);
    }
  }
} globalDataInitializerObject;


////////////////////////////////////////////////////////////////////////////////
// Access order of a matrix can be row major or column major. This is assuming
// that the underlying matrix is itself represented as a sequential row-major
// list of the elements in the matrix.
//
// Recall, for a matrix:
//   a11, a12, a13
//   a21, a22, a23
//   a31, a32, a33
//
// The row major traversal is:
//   a11, a12, a13, a21, a22, a23, a31, a32, a33
//
// And the column major traversal is:
//   a11, a21, a31, a12, a22, a32, a13, a23, a33
//
// When the representation and traversal order are the same, the behavior is
// friendly to the cache. When they differ, the behavior is unfriendly.
////////////////////////////////////////////////////////////////////////////////


struct Friendly {
  unsigned
  getIndex(unsigned count, unsigned row, unsigned col) const {
    return row * count + col;
  }
};
struct Unfriendly {
  unsigned
  getIndex(unsigned count, unsigned row, unsigned col) const {
    return col * count + row;
  }
};


////////////////////////////////////////////////////////////////////////////////
// The work of the benchmark can either be to read data from an index
// or to write data to an index. Note that reads may also be used to perform
// some form of data dependent computation.
////////////////////////////////////////////////////////////////////////////////


struct Read {
  void
  work(const Element value, unsigned index) const noexcept {
    // Note, without this the read of value can get thrown away, making the
    // operation effectively a no-op. This is visible in compiler explorer.
    benchmark::DoNotOptimize(randomElements[index]);
  }
};
struct ReadDependent {
  void
  work(Element& value, unsigned index) const noexcept {
    value ^= randomElements[index];
  }
};
struct Write {
  void
  work(const Element value, unsigned index) const noexcept {
    randomElements[index] = value;
  }
};
struct WriteDependent {
  void
  work(const Element value, unsigned index) const noexcept {
    randomElements[index] ^= value;
  }
};


////////////////////////////////////////////////////////////////////////////////
// We can then easily test the behavior of different traversal orders combined
// with different forms of operations using a single test driver. The driver
// below may be parameterized by both an indexing scheme and an action to
// perform at each location in the matrix. While the core algorithm is the same,
// the actual performance can vary dramatically.
////////////////////////////////////////////////////////////////////////////////


template<class Indexer, class Work>
static void
testAccess(benchmark::State& state) {
  const Indexer indexer;
  const Work worker;
  const int count = state.range(0);
  Element value = 0;
  benchmark::DoNotOptimize(&value);
  for (auto _ : state) {
    for (int row = 0; row < count; ++row) {
      for (int col = 0; col < count; ++col) {
        auto index = indexer.getIndex(count, row, col);
        worker.work(value, index);
      }
    }
  }
  benchmark::ClobberMemory();
  const size_t items = state.iterations() * state.range(0) * state.range(0);
  state.SetItemsProcessed(items);
  state.SetBytesProcessed(items * sizeof(Element));
  state.SetComplexityN(count);
}


BENCHMARK_SAMPLES(testAccess, Friendly,   Read);
BENCHMARK_SAMPLES(testAccess, Unfriendly, Read);
BENCHMARK_SAMPLES(testAccess, Friendly,   ReadDependent);
BENCHMARK_SAMPLES(testAccess, Unfriendly, ReadDependent);
BENCHMARK_SAMPLES(testAccess, Friendly,   Write);
BENCHMARK_SAMPLES(testAccess, Unfriendly, Write);
BENCHMARK_SAMPLES(testAccess, Friendly,   WriteDependent);
BENCHMARK_SAMPLES(testAccess, Unfriendly, WriteDependent);


BENCHMARK_MAIN();
