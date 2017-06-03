// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_tracker.h"
#include <cassert>
#include <cstdio>
#include <iostream>
#include <memory>
#include "heap_observer_summary_stats.h"
#include "heap_tracker_test_common.h"

int
main() {
  HeapTracker heap_tracker;
  std::unique_ptr<AbstractHeapObserver> heap_observer =
      std::make_unique<SummaryStatsObserver>();
  heap_tracker.ObserverAttach(heap_observer.get());

  heap_tracker.StartTrackingAll();
  LeakyFunctionDetour();
  LeakyFunction();
  GoodFunction();
  heap_tracker.StopTrackingAll();

  SummaryStatsObserver const * const envelope_stats_observer =
      dynamic_cast<SummaryStatsObserver const * const>(heap_observer.get());
  assert(envelope_stats_observer);

  SummaryStats const & stats = envelope_stats_observer->GetStats();
  std::cout << "SummaryStats = " << stats.ToString() << "\n";
  assert(stats.OutstandingAllocCount() == num_leaked_count);
  assert(stats.OutstandingAllocBytes() == num_leaked_bytes);
}
