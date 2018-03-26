// Copyright 2018, Arun Saha, <arunksaha@gmail.com>

#include <cassert>
#include <fstream>
#include <iostream>

#include "heap_tracker_types.h"

using std::cout;

int
main(int argc, char * argv[]) {
  if (argc < 2) {
    std::cerr << "usage: " << argv[0] << " <observer-output-file>\n";
    exit(1);
  }
  assert(argc == 2);

  std::string const filename{argv[1]};
  std::ifstream file{filename, std::ofstream::in};
  if (!file.is_open()) {
    std::cerr << __FILE__ << ":" << __LINE__
              << " Error opening file for reading: " << filename << '\n';
    exit(1);
  }

  std::vector<OfflineEntry> offline_entry_vec;
  for (std::string line; std::getline(file, line);) {
    offline_entry_vec.emplace_back(OfflineEntry{line});
  }

  SummaryStats summary_stats;
  PointerToCallbackInfoMap pointer_to_cbinfo_map;
  for (auto const & entry : offline_entry_vec) {
    UpdateFromOfflineEntry(entry, summary_stats, pointer_to_cbinfo_map);
    auto const usecs_since_epoch =
      TimePointToUsecsCount(entry.offline_timepoint);
    cout << usecs_since_epoch << " "
         << summary_stats.OutstandingAllocBytes() << '\n';
  }
}
