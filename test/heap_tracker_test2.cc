// Copyright 2018, Arun Saha <arunksaha@gmail.com>

#include <iostream>
#include <memory>

#include "heap_tracker_dispatcher.h"
#include "heap_tracker_interceptor.h"
#include "heap_tracker_test_common.h"

#define TEST(x) assert(x)

// This file has few ifdef's which may appear daunting,
// but the idea is simple. The outer macro
// HEAP_TRACKER_INTERCEPT_TCMALLOC_TEST is to make sure
// that the contents compile only in the tcmalloc mode.
// The inner macro SUMMARY vs. TIMESERIES is used to
// demonstrate that almost identical code can be written
// for both cases.

#ifdef HEAP_TRACKER_INTERCEPT_TCMALLOC_TEST

#ifdef TEST_OBSERVER_SUMMARY
#include "heap_tracker_observer_summary.h"

static HeapTrackOptions
MyHeapTrackSummaryOptions() {
  HeapTrackOptions options;
  options.track_size = true;
  return options;
}
#endif

#ifdef TEST_TIMESERIES_FILE
#include "heap_tracker_observer_timeseries_file.h"

static HeapTrackOptions
MyHeapTrackTimeseriesFileOptions() {
  HeapTrackOptions options;
  options.track_size      = true;
  options.track_callstack = true;
  options.track_timepoint = true;
  return options;
}

static char const * const output_filename =
  "heap_tracker_observer_timeseries_file.output.txt";
#endif

int
main() {
  std::unique_ptr<AbstractObserver> heap_observer{
#ifdef TEST_OBSERVER_SUMMARY
    new HeapObserverSummary{MyHeapTrackSummaryOptions()}
#endif
#ifdef TEST_TIMESERIES_FILE
    new HeapObserverTimeseriesFile{MyHeapTrackTimeseriesFileOptions(),
                                   output_filename}
#endif
  };

  Dispatcher dispatcher{heap_observer.get()};
  SetInterceptorDispatcher(&dispatcher);

  dispatcher.StartTrackingAll();
  LeakyFunctionDetour();
  LeakyFunction();
  GoodFunction();
  dispatcher.StopTrackingAll();

#ifdef TEST_OBSERVER_SUMMARY
  HeapObserverSummary const * const heap_observer_summary =
    dynamic_cast<HeapObserverSummary const * const>(heap_observer.get());
  assert(heap_observer_summary);

  // Retrieve results, log, verify.
  SummaryStats const stats = heap_observer_summary->GetStats();
  std::cout << "SummaryStats = " << stats.ToString() << "\n";
  TEST(stats.OutstandingAllocCount() == num_leaked_count);
  TEST(stats.OutstandingAllocBytes() == num_leaked_bytes);
#endif
#ifdef TEST_TIMESERIES_FILE
// The results are saved to the specified file.
// Nothing much to do here.
#endif
}

#endif
