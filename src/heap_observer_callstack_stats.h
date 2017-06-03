// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_OBSEVER_CALLSTACK_STATS_H
#define HEAP_OBSEVER_CALLSTACK_STATS_H

#include <string>
#include <vector>
#include "heap_tracker.h"

/// Information aggregated per-callstack.
struct CallStackAggValue {
  int64_t agg_count_{0};
  HeapSize agg_size_{0};
  explicit CallStackAggValue(HeapSize heap_size);
  std::string ToString() const;
};

/// Callstack along with its aggregrate information.
struct CallStackInfo {
  CallStack callstack_;
  CallStackAggValue info_;
  CallStackInfo(CallStack const &, CallStackAggValue const &);
  std::string ToString() const;
};

/// Comparator for descending sort on agg_size; return true if a > b.
struct CallStackSizeComparator {
  bool
  operator()(CallStackInfo const & a, CallStackInfo const & b) const {
    return a.info_.agg_size_ > b.info_.agg_size_;
  }
};

/// Outstanding callstacks aggregated.
struct OutstandingReport {
  std::string ToString() const;
  std::vector<CallStackInfo> callstack_info_vec_;
};

class CallstackStatsObserver final : public AbstractHeapObserver {
 public:
  CallstackStatsObserver() = default;
  virtual ~CallstackStatsObserver() = default;
  virtual HeapTrackOptions GetHeapTrackOptions() const override;
  virtual void OnAlloc(AllocCallbackInfo const & alloc_cb_info) override;
  virtual void OnFree(FreeCallbackInfo const & free_cb_info) override;
  virtual void OnComplete() override;

  OutstandingReport const & GetOutstandingReport() const;

 private:
  void PrepareOutstandingReport();

 private:
  const HeapTrackOptions heap_track_options_{
      true,  // track_size
      true,  // track_timepoint
      true   // track_callstack
  };
  bool complete_{false};
  OutstandingReport outstanding_report_;
};

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions.

#endif
