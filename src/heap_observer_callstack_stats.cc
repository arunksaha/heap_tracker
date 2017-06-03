// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_observer_callstack_stats.h"
#include <algorithm>
#include <sstream>

HeapTrackOptions
CallstackStatsObserver::GetHeapTrackOptions() const {
  return heap_track_options_;
}

void
CallstackStatsObserver::OnAlloc(AllocCallbackInfo const &) {}

void
CallstackStatsObserver::OnFree(FreeCallbackInfo const &) {}

void
CallstackStatsObserver::OnComplete() {
  assert(!complete_);
  PrepareOutstandingReport();
  complete_ = true;
}

void
CallstackStatsObserver::PrepareOutstandingReport() {
  std::unordered_map<CallStack, CallStackAggValue, CallStackHasher>
      callstack_map;

  PointerToCallbackInfoMap const & pointer_to_cbinfo_map =
      heap_tracker_->GetPointerToCallbackInfoMap();

  for (auto const & ptr_cbinfo_kv : pointer_to_cbinfo_map) {
    CallStack const & callstack = ptr_cbinfo_kv.second.alloc_cb_callstack;
    auto it = callstack_map.find(callstack);
    if (it == callstack_map.end()) {
      callstack_map.emplace(
          std::piecewise_construct, std::forward_as_tuple(callstack),
          std::forward_as_tuple(ptr_cbinfo_kv.second.alloc_cb_size));
    }
    else {
      CallStackAggValue & agg_value = it->second;
      agg_value.agg_count_ += 1;
      agg_value.agg_size_ += ptr_cbinfo_kv.second.alloc_cb_size;
    }
  }

  std::vector<CallStackInfo> callstack_vec;
  callstack_vec.reserve(callstack_map.size());
  for (auto const & kv : callstack_map) {
    callstack_vec.emplace_back(kv.first, kv.second);
  }

  std::sort(begin(callstack_vec), end(callstack_vec),
            CallStackSizeComparator{});

  outstanding_report_.callstack_info_vec_ = std::move(callstack_vec);
}

OutstandingReport const &
CallstackStatsObserver::GetOutstandingReport() const {
  assert(complete_);
  return outstanding_report_;
}

std::string
CallStackInfo::ToString() const {
  return info_.ToString() + ", " + callstack_.ToString();
}

std::string
CallStackAggValue::ToString() const {
  return std::string{"bytes = "} + std::to_string(agg_size_) +
         std::string{", count = "} + std::to_string(agg_count_);
}

CallStackAggValue::CallStackAggValue(HeapSize const heap_size)
    : agg_count_{1}, agg_size_{heap_size} {}

CallStackInfo::CallStackInfo(CallStack const & callstack,
                             CallStackAggValue const & callstack_agg_value)
    : callstack_{callstack}, info_{callstack_agg_value} {}

std::string
OutstandingReport::ToString() const {
  std::ostringstream oss;
  for (auto const & callstack_info : callstack_info_vec_) {
    oss << callstack_info.ToString() << '\n';
  }
  return oss.str();
}
