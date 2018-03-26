# Copyright 2017, Arun Saha <arunksaha@gmail.com>

###############################################################################
set(heap_tracker_common_sources
  heap_tracker_types.cc
  heap_tracker_dispatcher.cc
)

add_library(heap_tracker_common SHARED
  ${heap_tracker_common_sources}
)

target_link_libraries(heap_tracker_common dl)

###############################################################################
add_executable(heap_tracker_observer_timeseries_file_decoder_tool
  heap_tracker_observer_timeseries_file_decoder_tool.cc
)

target_link_libraries(heap_tracker_observer_timeseries_file_decoder_tool
  heap_tracker_common
)

###############################################################################
add_executable(heap_tracker_observer_timeseries_file_graph_input_tool
  heap_tracker_observer_timeseries_file_graph_input_tool.cc
)

target_link_libraries(heap_tracker_observer_timeseries_file_graph_input_tool
  heap_tracker_common
)

###############################################################################
set(heap_tracker_observer_summary_sources_automatic
  heap_tracker_observer_summary.cc
  heap_tracker_observer_summary_automatic_setup.cc
  ${heap_tracker_common_sources}
)

set(heap_tracker_observer_summary_sources_nonautomatic
  heap_tracker_observer_summary.cc
  ${heap_tracker_common_sources}
)

###############################################################################
set(heap_tracker_observer_summary_interposition_sources
  heap_tracker_interceptor_interposition.cc
  ${heap_tracker_observer_summary_sources_automatic}
)

add_library(heap_tracker_observer_summary_interposition SHARED
  ${heap_tracker_observer_summary_interposition_sources}
)

target_link_libraries(heap_tracker_observer_summary_interposition
  dl
)

###############################################################################
set(heap_tracker_observer_summary_tcmalloc_sources_automatic
  heap_tracker_interceptor_tcmalloc.cc
  ${heap_tracker_observer_summary_sources_automatic}
)

add_library(heap_tracker_observer_summary_tcmalloc_automatic SHARED
  ${heap_tracker_observer_summary_tcmalloc_sources_automatic}
)

target_link_libraries(heap_tracker_observer_summary_tcmalloc_automatic
  tcmalloc
)


set(heap_tracker_observer_summary_tcmalloc_sources_nonautomatic
  heap_tracker_interceptor_tcmalloc.cc
  ${heap_tracker_observer_summary_sources_nonautomatic}
)

add_library(heap_tracker_observer_summary_tcmalloc_nonautomatic SHARED
  ${heap_tracker_observer_summary_tcmalloc_sources_nonautomatic}
)

target_link_libraries(heap_tracker_observer_summary_tcmalloc_nonautomatic
  tcmalloc
)

###############################################################################
## The following does not work, it does produce correct results.
## add_library(heap_tracker_observer_timeseries_file SHARED
##   heap_tracker_observer_timeseries_file_driver.cc
##   heap_tracker_observer_timeseries_file.cc
## )
##
## target_link_libraries(heap_tracker_observer_timeseries_file heap_tracker_common)

###############################################################################
set(heap_tracker_observer_timeseries_file_sources_automatic
  heap_tracker_observer_timeseries_file.cc
  heap_tracker_observer_timeseries_file_automatic_setup.cc
  ${heap_tracker_common_sources}
)

set(heap_tracker_observer_timeseries_file_sources_nonautomatic
  heap_tracker_observer_timeseries_file.cc
  ${heap_tracker_common_sources}
)

###############################################################################
set(heap_tracker_observer_timeseries_file_sources_interposition
  heap_tracker_interceptor_interposition.cc
  ${heap_tracker_observer_timeseries_file_sources_automatic}
)

add_library(heap_tracker_observer_timeseries_file_interposition SHARED
  ${heap_tracker_observer_timeseries_file_sources_interposition}
)

target_link_libraries(heap_tracker_observer_timeseries_file_interposition
  dl
)

###############################################################################
set(heap_tracker_observer_timeseries_file_sources_tcmalloc_automatic
  heap_tracker_interceptor_tcmalloc.cc
  ${heap_tracker_observer_timeseries_file_sources_automatic}
)

add_library(heap_tracker_observer_timeseries_file_tcmalloc_automatic SHARED
  ${heap_tracker_observer_timeseries_file_sources_tcmalloc_automatic}
)

target_link_libraries(heap_tracker_observer_timeseries_file_tcmalloc_automatic
  tcmalloc
)

set(heap_tracker_observer_timeseries_file_sources_tcmalloc_nonautomatic
  heap_tracker_interceptor_tcmalloc.cc
  ${heap_tracker_observer_timeseries_file_sources_nonautomatic}
)

add_library(heap_tracker_observer_timeseries_file_tcmalloc_nonautomatic SHARED
  ${heap_tracker_observer_timeseries_file_sources_tcmalloc_nonautomatic}
)

target_link_libraries(heap_tracker_observer_timeseries_file_tcmalloc_nonautomatic
  tcmalloc
)

