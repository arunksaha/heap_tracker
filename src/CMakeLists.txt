add_library(heap_tracker_lib
  heap_tracker.cc
  heap_tracker_utils.cc
  heap_watch_tcmalloc.cc
  heap_observer_summary_stats.cc
  heap_observer_callstack_stats.cc)

target_link_libraries(heap_tracker_lib tcmalloc)

