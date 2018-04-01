// Copyright 2017, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_OBSERVER_TIMESERIES_FILE_H
#define HEAP_OBSERVER_TIMESERIES_FILE_H

#include <fstream>
#include <string>

#include "heap_tracker_observer_interface.h"

class HeapObserverTimeseriesFile final : public AbstractObserver {
 public:
  HeapObserverTimeseriesFile(HeapTrackOptions heap_track_options,
                             char const * const filename);
  ~HeapObserverTimeseriesFile();
  virtual HeapTrackOptions GetHeapTrackOptions() const override;
  virtual void OnAlloc(AllocCallbackInfo const & alloc_cb_info) override;
  virtual void OnFree(FreeCallbackInfo const & free_cb_info) override;
  virtual void OnComplete() override;
  virtual void Dump() const override;
  virtual void Reset() override;

 private:
  // Flush all entries from 'offline_entries_' to 'filename_'.
  void Flush();
  // Flush if 'offline_entries_' is full.
  void FlushIfFull();

 private:
  const HeapTrackOptions heap_track_options_;
  // Name of output file.
  char const * const filename_{nullptr};
  // Entries are buffered before flushed to the file.
  enum { kMaxBufferCount = 128 * 1024 };
  std::array<OfflineEntry, kMaxBufferCount> offline_entries_;
  // Current number of entries in 'offline_entries_'.
  size_t size_{0};
  std::ofstream out_;
};

////////////////////////////////////////////////////////////////////////////////

inline HeapTrackOptions
GetHeapObserverTimeseriesFileAutomaticTrackOptions() {
  HeapTrackOptions options;
  options.track_size            = true;
  options.track_callstack       = true;
  options.track_timepoint       = true;
  options.start_at_construction = true;
  options.finish_at_destruction = true;
  return options;
}

////////////////////////////////////////////////////////////////////////////////
// A class to automatically setup the the dispatcher (in constructor).
struct HeapObserverTimeseriesFileDriver {
  HeapObserverTimeseriesFileDriver();
  ~HeapObserverTimeseriesFileDriver() = default;
};

#endif
