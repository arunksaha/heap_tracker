// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_observer_summary_stats.h"
#include "heap_tracker_utils.h"

#include <sstream>

std::string
SummaryStats::ToString() const {
  std::ostringstream oss;
  oss << "{\n";
  oss << "  cumulative_alloc_count: " << cumulative_alloc_count_ << ",\n";
  oss << "  cumulative_free_count: " << cumulative_free_count_ << ",\n";
  oss << "  cumulative_alloc_bytes: " << cumulative_alloc_bytes_ << ",\n";
  oss << "  cumulative_free_bytes: " << cumulative_free_bytes_ << ",\n";
  oss << "  peak_outstanding_alloc_count: " << peak_outstanding_alloc_count_
      << ",\n";
  oss << "  peak_outstanding_alloc_count_instant: "
      << TimePointToString(peak_outstanding_alloc_count_instant_) << ",\n";
  oss << "  peak_outstanding_alloc_bytes: " << peak_outstanding_alloc_bytes_
      << ",\n";
  oss << "  peak_outstanding_alloc_bytes_instant: "
      << TimePointToString(peak_outstanding_alloc_bytes_instant_) << ",\n";
  oss << "  outstanding_alloc_count: " << OutstandingAllocCount() << ",\n";
  oss << "  outstanding_alloc_bytes: " << OutstandingAllocBytes() << ",\n";
  oss << "}";
  return oss.str();
}

HeapTrackOptions
SummaryStatsObserver::GetHeapTrackOptions() const {
  return heap_track_options_;
}

void
SummaryStatsObserver::OnAlloc(AllocCallbackInfo const & alloc_cb_info) {
  stats_.cumulative_alloc_count_ += 1;
  stats_.cumulative_alloc_bytes_ += alloc_cb_info.alloc_cb_size;

  if (stats_.cumulative_alloc_count_ > stats_.peak_outstanding_alloc_count_) {
    stats_.peak_outstanding_alloc_count_ = stats_.cumulative_alloc_count_;
    stats_.peak_outstanding_alloc_count_instant_ =
        alloc_cb_info.alloc_cb_timepoint;
  }

  if (stats_.cumulative_alloc_bytes_ > stats_.peak_outstanding_alloc_bytes_) {
    stats_.peak_outstanding_alloc_bytes_ = stats_.cumulative_alloc_bytes_;
    stats_.peak_outstanding_alloc_bytes_instant_ =
        alloc_cb_info.alloc_cb_timepoint;
  }
}

void
SummaryStatsObserver::OnFree(FreeCallbackInfo const & free_cb_info) {
  stats_.cumulative_free_count_ += 1;
  stats_.cumulative_free_bytes_ += free_cb_info.free_cb_size;
}

void
SummaryStatsObserver::OnComplete() {}
