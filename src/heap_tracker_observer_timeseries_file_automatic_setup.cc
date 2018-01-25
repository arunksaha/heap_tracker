// Copyright 2018, Arun Saha <arunksaha@gmail.com>

#include <string>

#include "heap_tracker_dispatcher.h"
#include "heap_tracker_interceptor.h"
#include "heap_tracker_observer_timeseries_file.h"

// The following portion of the file is like a "driver",
// it helps an application to use HeapObserverTimeseriesFile
// in a off-the-shelf ready-to-use way. Essentially, it
// creates few a static variables and therby initialize
// few data structures.

static std::string const output_filename{
  "heap_tracker_observer_timeseries_file.output.txt"};

// Step 1: Create observer.
static HeapObserverTimeseriesFile local_heap_observer{
  GetHeapObserverTimeseriesFileAutomaticTrackOptions(), output_filename};

// Step 2: Create dispatcher using the above observer.
static Dispatcher local_dispatcher{&local_heap_observer};

HeapObserverTimeseriesFileDriver::HeapObserverTimeseriesFileDriver() {
  SetInterceptorDispatcher(&local_dispatcher);
}

#ifdef HEAP_TRACKER_INTERCEPT_INTERPOSITION
// Step 3: Create the driver to setup interceptor dispatcher.
// For tcmalloc, this step is done by the test code.
static const HeapObserverTimeseriesFileDriver local_driver;
#endif
