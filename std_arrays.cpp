#include "const.h"
#include <iostream>
#include <chrono>
#include <array>


using namespace std::chrono;

int main() {
  std::cout << "std::arrays\n";
  std::array<double, SIZE> a;
  std::array<double, SIZE> b;
  std::array<double, SIZE> c;
  std::array<double, SIZE> d;
  std::array<double, SIZE> e;
  srand(1);
  int m = rand();
  double n = (1.0 + m * 1.0e-9) / (2.0 + m * 1.0e-9);
  for (int j=0; j<SIZE; j++) {
    a[j] = 1/n + j;
  }
  auto start = high_resolution_clock::now();
  for (int i=0; i<ITERATIONS; i++) {
    for (int j=0; j<SIZE; j++) {
      a[j] += j - n;
    }
  }
  auto stop = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(stop - start);
  std::cout << "1. " << (duration.count() * 1.0e-6) << " seconds\n";

  start = high_resolution_clock::now();
  for (int i=0; i<ITERATIONS; i++) {
    for (int j=0; j<SIZE; j++) {
      a[j] += n + 1/(j + 1);
      b[j] = a[j];
      c[j] = b[j];
      d[j] = c[j];
      e[j] = d[j];
    }
  }
  stop = high_resolution_clock::now();
  duration = duration_cast<microseconds>(stop - start);
  std::cout << "2. " << (duration.count() * 1.0e-6) << " seconds\n";
  double r = 0;
  for (int j=0; j<SIZE; j++) {
    r += e[j];
  }
  std::cout << r << std::endl << std::endl;

  return 0;
}