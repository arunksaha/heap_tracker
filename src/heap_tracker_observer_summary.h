// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_OBSERVER_SUMMARY_H
#define HEAP_OBSERVER_SUMMARY_H

#include "heap_tracker_observer_interface.h"

/// A class to track summary stats online.
class HeapObserverSummary final : public AbstractObserver {
 public:
  explicit HeapObserverSummary(HeapTrackOptions heap_track_options);
  ~HeapObserverSummary();
  virtual HeapTrackOptions GetHeapTrackOptions() const override;
  virtual void OnAlloc(AllocCallbackInfo const & alloc_cb_info) override;
  virtual void OnFree(FreeCallbackInfo const & free_cb_info) override;
  virtual void OnComplete() override;
  virtual void Dump() const override;
  virtual void Reset() override;

  SummaryStats GetStats() const;

 private:
  HeapTrackOptions const heap_track_options_;
  SummaryStats stats_;
};

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions.

inline HeapTrackOptions
HeapObserverSummary::GetHeapTrackOptions() const {
  return heap_track_options_;
}

inline SummaryStats
HeapObserverSummary::GetStats() const {
  return stats_;
}

inline HeapTrackOptions
GetHeapObserverSummaryAutomaticTrackOptions() {
  HeapTrackOptions options;
  options.track_size            = true;
  options.track_timepoint       = true;
  options.start_at_construction = true;
  options.finish_at_destruction = true;
  return options;
}

////////////////////////////////////////////////////////////////////////////////
// A class to automatically setup the the dispatcher (in constructor)
// and automatically dump the SummaryStats in a file (in destructor).
struct HeapObserverSummaryDriver {
  HeapObserverSummaryDriver();
  ~HeapObserverSummaryDriver();
};

#endif
