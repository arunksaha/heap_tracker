// Copyright 2017, Arun Saha, <arunksaha@gmail.com>

#include <cassert>
#include <fstream>
#include <iostream>

#include "heap_tracker_types.h"

int
main(int argc, char * argv[]) {
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

  auto result                        = DecodeOfflineEntries(offline_entry_vec);
  SummaryStats const & summary_stats = result.first;
  PointerToCallbackInfoMap const & pointer_to_cbinfo_map = result.second;

  std::cout << "Summary Stats = " << summary_stats.ToString() << '\n';

  std::cout << ToStringPointerToCallbackInfoMap(pointer_to_cbinfo_map);

  OutstandingReport const outstanding_report =
    PrepareOutstandingReport(pointer_to_cbinfo_map);
  std::cout << "Outstanding Report = {\n"
            << outstanding_report.ToString() << "}\n";
}
