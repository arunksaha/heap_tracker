// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_OBSEVER_SUMMARY_STATS_H
#define HEAP_OBSEVER_SUMMARY_STATS_H

#include <string>
#include "heap_tracker.h"

// Basic stats. Note all cumulative numbers are always increasing.
// TimePoint updates are optional.
struct SummaryStats {
  // Cumulative number of allocations.
  int64_t cumulative_alloc_count_{0};

  // Cumulative number of frees.
  int64_t cumulative_free_count_{0};

  // Cumulative number of allocated bytes.
  int64_t cumulative_alloc_bytes_{0};

  // Cumulative number of freed bytes.
  int64_t cumulative_free_bytes_{0};

  // Peak value of outstanding allocation count.
  int64_t peak_outstanding_alloc_count_{0};
  TimePoint peak_outstanding_alloc_count_instant_;

  // Peak size of outstanding allocated bytes.
  int64_t peak_outstanding_alloc_bytes_{0};
  TimePoint peak_outstanding_alloc_bytes_instant_;

  int64_t
  OutstandingAllocCount() const {
    return cumulative_alloc_count_ - cumulative_free_count_;
  }

  int64_t
  OutstandingAllocBytes() const {
    return cumulative_alloc_bytes_ - cumulative_free_bytes_;
  }

  // Representation as JSON.
  std::string ToString() const;
};

class SummaryStatsObserver final : public AbstractHeapObserver {
 public:
  SummaryStatsObserver() = default;
  virtual ~SummaryStatsObserver() = default;
  virtual HeapTrackOptions GetHeapTrackOptions() const override;
  virtual void OnAlloc(AllocCallbackInfo const & alloc_cb_info) override;
  virtual void OnFree(FreeCallbackInfo const & free_cb_info) override;
  virtual void OnComplete() override;

  SummaryStats GetStats() const;

 private:
  SummaryStats stats_;
  const HeapTrackOptions heap_track_options_{
      true,  // track_size
      true,  // track_timepoint
      false  // track_callstack
  };
};

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions.

inline SummaryStats
SummaryStatsObserver::GetStats() const {
  return stats_;
}

#endif
