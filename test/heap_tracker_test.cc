// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_tracker_test_common.h"

// The following conditional compiling allows to use
// the same code to be used for two kinds of interceptions
// and two kinds of observers.
#ifdef HEAP_TRACKER_INTERCEPT_TCMALLOC_TEST
#ifdef TEST_OBSERVER_SUMMARY
#include "heap_tracker_observer_summary.h"
static const HeapObserverSummaryDriver driver;
#endif

#ifdef TEST_TIMESERIES_FILE
#include "heap_tracker_observer_timeseries_file.h"
static const HeapObserverTimeseriesFileDriver driver;
#endif
#endif

int
main() {
  LeakyFunctionDetour();
  LeakyFunction();
  GoodFunction();
}
