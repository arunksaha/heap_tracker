// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <iostream>
#include <sstream>

#include "heap_tracker_dispatcher.h"
#include "heap_tracker_interceptor.h"
#include "heap_tracker_observer_summary.h"

// The following portion of the file is like a "driver",
// it helps an application to use HeapObserverSummary
// in a off-the-shelf ready-to-use way. Essentially, it
// creates few a static variables and therby initialize
// few data structures.

static char const * const output_filename =
  "heap_tracker_observer_summary.output.txt";
static char const stdout_mesg[] =
  "Observer SummaryStats saved to file "
  "heap_tracker_observer_summary.output.txt\n";

// Step 1: Create observer.
static HeapObserverSummary local_heap_observer{
  GetHeapObserverSummaryAutomaticTrackOptions()};

// Step 2: Create dispatcher using the above observer.
static Dispatcher local_dispatcher{&local_heap_observer};

static void
DumpObserverOutputToFile() {
  int const fd_output =
    open(output_filename, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR);
  assert(fd_output != -1);

  std::string const observer_stats_string =
    local_heap_observer.GetStats().ToString();
  char const * const data = observer_stats_string.data();
  size_t const len        = observer_stats_string.size();

  char const mesg[] = "SummaryStats = ";
  ssize_t rc        = write(fd_output, mesg, sizeof mesg);
  rc                = write(fd_output, data, len);
  rc                = write(fd_output, "\n", 1);
  (void)rc;

  close(fd_output);

  write(STDOUT_FILENO, stdout_mesg, sizeof stdout_mesg);
}

HeapObserverSummaryDriver::HeapObserverSummaryDriver() {
  SetInterceptorDispatcher(&local_dispatcher);
}

HeapObserverSummaryDriver::~HeapObserverSummaryDriver() {
  DumpObserverOutputToFile();
}

#ifdef HEAP_TRACKER_INTERCEPT_INTERPOSITION
// Step 3: Create the driver to setup interceptor dispatcher.
// For tcmalloc, this step is done by the test code.
static const HeapObserverSummaryDriver local_driver;
#endif
