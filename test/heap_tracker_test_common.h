// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_TRACKER_TEST_COMMON_H
#define HEAP_TRACKER_TEST_COMMON_H

#include <cstdint>

extern int64_t num_leaked_count;
extern int64_t num_leaked_bytes;

void LeakyFunctionDetour();

void LeakyFunction();

void GoodFunction();

#endif
