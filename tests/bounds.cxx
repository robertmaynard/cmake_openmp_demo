
#include <catch2/catch.hpp>
#include <demo/lib.h>
#include <omp.h>

namespace {
std::vector<float> one_value = {2.0f};
std::vector<float> two_value = {-2.0f, 2.0f};
std::vector<float> eight_value = {1.0f, 2.0f, 3.0f, 4.0f, 5.0, 6.0f, 7.0f, 8.0f};
} // namespace

TEST_CASE("min_max works on simple arrays", "[demo.min_max]") {

  REQUIRE(demo::min_max(one_value)[0] == 2.0f);
  REQUIRE(demo::min_max(two_value)[0] == -2.0f);
  REQUIRE(demo::min_max(two_value)[1] == 2.0f);
  REQUIRE(demo::min_max(eight_value)[1] == 8.0f);
}

TEST_CASE("Benchmark min_max", "[demo.min_max.benchmark]") {
  constexpr std::size_t size = 4194304;
  std::mt19937 rng;
  std::uniform_real_distribution<float> range(-2.0f, 2.0f);
  std::vector<float> values;
  values.resize(size);

  for (std::size_t i = 0; i < size; ++i) {
    auto v = range(rng);
    values[i] = v;
  }

  BENCHMARK("serial sum random vector") {
    demo::bounds b = {3.0f, -3.0f};
    for (std::size_t i = 0; i < size; ++i) {
      b[0] = std::min(b[0], values[i]);
      b[1] = std::max(b[1], values[i]);
    }
    REQUIRE(b[0] < -1.5f);
    REQUIRE(b[1] > 1.5f);
  };

  BENCHMARK("openmp all cores sum random vector") {
    demo::bounds min_max = demo::min_max(values);
    REQUIRE(min_max[0] < -1.5f);
    REQUIRE(min_max[1] > 1.5f);
  };

  BENCHMARK("openmp 1 core sum random vector") {
    omp_set_num_threads(1);
    demo::bounds min_max = demo::min_max(values);
    REQUIRE(min_max[0] < -1.5f);
    REQUIRE(min_max[1] > 1.5f);
  };
}
