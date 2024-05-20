#include "const.h"
#include <iostream>
#include <chrono>
#include <vector>
#include <cmath>

using namespace std::chrono;

static constexpr double epsilon = 1.0e-6;

void step(std::vector<double>& x) {
  double p = x[0];
  x[0] += epsilon * x[1];
  x[1] += epsilon * std::sin(p);
}

int main() {
  std::cout << "euler preallocator\n";
  std::vector<double> x = {0.1, 0};
  auto start = high_resolution_clock::now();
  for (int i = 0; i < ITERATIONS; i++) {
    step(x);
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::cout << (duration.count() * 1.0e-6) << " seconds\n";
  std::cout << x[0] << "," << x[1] << std::endl << std::endl;
}