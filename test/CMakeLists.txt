add_executable(heap_tracker_test1
  heap_tracker_test_common.cc
  heap_tracker_test1.cc)

target_link_libraries(heap_tracker_test1
  heap_tracker_lib)
  
add_test(heap_tracker_tests1 heap_tracker_test1)


add_executable(heap_tracker_test2
  heap_tracker_test_common.cc
  heap_tracker_test2.cc)

target_link_libraries(heap_tracker_test2
  heap_tracker_lib)
  
add_test(heap_tracker_tests2 heap_tracker_test2)
