// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_TRACKER_UTILS_H
#define HEAP_TRACKER_UTILS_H
#include <chrono>
#include <string>

typedef std::chrono::system_clock::time_point TimePoint;

// Return the TimePoint of the instant.
TimePoint GetTimePointNow();

// Convert TimePoint to human readable string in "YYYY-MM-DD HH:MM:SS" format.
std::string TimePointToString(TimePoint const & time_point);

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions.

inline TimePoint
GetTimePointNow() {
  return std::chrono::system_clock::now();
}

#endif
