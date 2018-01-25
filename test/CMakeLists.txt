add_library(heap_tracker_test_common_lib
  heap_tracker_test_common.cc)


if(BUILD_INTERPOSITION)
  add_executable(heap_tracker_test_interposition
    heap_tracker_test.cc)
  
  target_link_libraries(heap_tracker_test_interposition
    heap_tracker_test_common_lib)
endif(BUILD_INTERPOSITION)

if(BUILD_TCMALLOC)

  add_definitions(-DHEAP_TRACKER_INTERCEPT_TCMALLOC_TEST)

  # ------------------------------------------------------------------
  add_executable(heap_tracker_test_tcmalloc_summary
    heap_tracker_test.cc)
  
  target_link_libraries(heap_tracker_test_tcmalloc_summary
    heap_tracker_test_common_lib
    heap_tracker_observer_summary_tcmalloc_automatic
  )

  target_compile_definitions(heap_tracker_test_tcmalloc_summary
    PUBLIC TEST_OBSERVER_SUMMARY=1)

  # ------------------------------------------------------------------
  add_executable(heap_tracker_test_tcmalloc_timeseries_file
    heap_tracker_test.cc)
  
  target_link_libraries(heap_tracker_test_tcmalloc_timeseries_file
    heap_tracker_test_common_lib
    heap_tracker_observer_timeseries_file_tcmalloc_automatic
  )

  target_compile_definitions(heap_tracker_test_tcmalloc_timeseries_file
    PUBLIC TEST_TIMESERIES_FILE=1)

  # ------------------------------------------------------------------
  add_executable(heap_tracker_test2_observer_summary
    heap_tracker_test2.cc)

  target_link_libraries(heap_tracker_test2_observer_summary
    heap_tracker_test_common_lib
    heap_tracker_observer_summary_tcmalloc_nonautomatic
  )

  target_compile_definitions(heap_tracker_test2_observer_summary
    PUBLIC TEST_OBSERVER_SUMMARY=1)

  # ------------------------------------------------------------------
  add_executable(heap_tracker_test2_timeseries_file
    heap_tracker_test2.cc)

  target_link_libraries(heap_tracker_test2_timeseries_file
    heap_tracker_test_common_lib
    heap_tracker_observer_timeseries_file_tcmalloc_nonautomatic
  )

  target_compile_definitions(heap_tracker_test2_timeseries_file
    PUBLIC TEST_TIMESERIES_FILE=1)

  # ------------------------------------------------------------------
  add_executable(heap_tracker_test3_size
    heap_tracker_test3.cc)

  target_link_libraries(heap_tracker_test3_size
    heap_tracker_test_common_lib
    heap_tracker_observer_summary_tcmalloc_nonautomatic
  )

endif(BUILD_TCMALLOC)

