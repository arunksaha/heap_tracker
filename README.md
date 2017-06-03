# heaptracker: A customizable library for tracking heap activity

This is a library to gain insight into the heap memory usage behavior.

heaptracker is distributed under the terms of the BSD License.

It consists of three main components:
  - HeapTracker
  - Heap Observer
  - Heap Watcher

HeapTracker is the main class applications interact with. It offers APIs
to enable and disable heap tracking. It also offers APIs to attach an
observer.

All the observers follow a specific interface as offered by
AbstractHeapObserver. There can be different kind of observers based on
what aspect of the heap needs to be tracked. The library offers the
following observers out-of-the-box:
  - SummaryStatsObserver
  - CallStackStatsObserver

SummaryStatsObserver tracks the allocation and deallocation volume, the
time instant when allocation hits peak etc. Here is an example output
from test/heap_tracker_test1.cc

    $ test/heap_tracker_test1
    SummaryStats = {
      cumulative_alloc_count: 35,
      cumulative_free_count: 33,
      cumulative_alloc_bytes: 166726292,
      cumulative_free_bytes: 166724292,
      peak_outstanding_alloc_count: 35,
      peak_outstanding_alloc_count_instant: 2017-06-03 11:25:58.269747,
      peak_outstanding_alloc_bytes: 166726292,
      peak_outstanding_alloc_bytes_instant: 2017-06-03 11:25:58.269747,
      outstanding_alloc_count: 2,
      outstanding_alloc_bytes: 2000,
    }

Along other things, it shows that, during the time heap tracking was
enabled, there are 2 outstanding allocations which sum up to 2000 bytes.

CallStackStatsObserver tracks the callstacks of the live allocations. At 
the end, it reports the outstanding allocations with the corresponding
callstack. Here is an example output from test/heap_tracker_test2.cc

    bytes = 2000, count = 2, callstack = {0x41CB14 0x41CAF9 0x41CAE9 0x420FD3 0x7F9E8E9E5830 0x41CA09 } 
    bytes = 1000, count = 1, callstack = {0x41CB14 0x420FA8 0x7F9E8E9E5830 0x41CA09 } 
    bytes = 1000, count = 1, callstack = {0x41CB14 0x41CAF9 0x41CAE9 0x420F9E 0x7F9E8E9E5830 0x41CA09 }

Heap Watcher is the mechanism to intercept the allocations and deallocations
while heap tracking is active. At present, the only supported mechanism is
tcmalloc. Few other mechanisms are in the roadmap.

# Tidbits

The library uses standard compliant and portable C++.

It is compiled with the following settings

    -std=c++14 -pedantic

and the following warning flags

    -Wall -Wextra -Wshadow -Wnon-virtual-dtor -Wold-style-cast -Wcast-align
    -Wunused -Woverloaded-virtual -Wconversion 
    -Werror

It is tested to compile with clang++ 3.8 and g++ 6.2.

The coding style is based on Google style guide with minor
changes, it is specified in the .clang-format. Run make_style.sh
to format the code as per this style.
