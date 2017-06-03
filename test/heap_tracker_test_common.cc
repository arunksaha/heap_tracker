// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_tracker_test_common.h"
#include <memory>
#include <unordered_set>
#include <vector>

enum { ArrayLen = 1000 };

int64_t num_leaked_bytes = 0;
int64_t num_leaked_count = 0;

static void
LeakyFunctionCaller() {
  LeakyFunction();
}

void
LeakyFunctionDetour() {
  LeakyFunctionCaller();
}

void
LeakyFunction() {
  new char[ArrayLen];
  num_leaked_bytes += ArrayLen;
  num_leaked_count += 1;
}

struct LargeCharArray {
  char array_[100 * 1024 * 1024];
};

void
GoodFunction() {
  char * pchar1 = new char[40 * 1024 * 1024];
  char * pchar2 = new char[9 * 1024 * 1024];
  std::shared_ptr<char> pchar3{new char[10 * 1024 * 1024]};
  std::shared_ptr<LargeCharArray> large = std::make_shared<LargeCharArray>();

  std::vector<int> vi;
  std::unordered_set<int> si;
  for (int i = 0; i != 10; ++i) {
    std::shared_ptr<int> pint = std::make_shared<int>();
    vi.push_back(i);
    si.insert(i);
  }

  delete[] pchar1;
  delete[] pchar2;
}
