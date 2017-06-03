// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_tracker.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include "heap_observer_callstack_stats.h"
#include "heap_tracker_test_common.h"

int
main() {
  HeapTracker heap_tracker;
  std::unique_ptr<AbstractHeapObserver> heap_observer =
      std::make_unique<CallstackStatsObserver>();
  heap_tracker.ObserverAttach(heap_observer.get());

  heap_tracker.StartTrackingAll();
  LeakyFunctionDetour();
  LeakyFunction();
  GoodFunction();
  for (int i = 0; i != 2; ++i) {
    LeakyFunctionDetour();
  }
  heap_tracker.StopTrackingAll();

  CallstackStatsObserver const * const callstack_stats_observer =
      dynamic_cast<CallstackStatsObserver const * const>(heap_observer.get());
  assert(callstack_stats_observer);

  OutstandingReport const & outstanding_report =
      callstack_stats_observer->GetOutstandingReport();
  std::cout << outstanding_report.ToString();
}
