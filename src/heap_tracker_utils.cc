// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include <heap_tracker_utils.h>

// Convert TimePoint to human readable string in "YYYY-MM-DD HH:MM:SS" format.
std::string
TimePointToString(const TimePoint & timepoint) {
  std::time_t const timepoint_t =
      std::chrono::system_clock::to_time_t(timepoint);
  struct tm * timeinfo = localtime(&timepoint_t);
  char buffer[80];
  strftime(buffer, sizeof buffer, "%Y-%m-%d %H:%M:%S.", timeinfo);
  std::chrono::microseconds const usecs_since_epoch =
      std::chrono::duration_cast<std::chrono::microseconds>(
          timepoint.time_since_epoch());
  int64_t const usecs_part = usecs_since_epoch.count() % 1000000;
  char usecs_buffer[7];
  snprintf(usecs_buffer, sizeof usecs_buffer, "%06ld", usecs_part);
  return std::string(buffer) + std::string(usecs_buffer);
}
