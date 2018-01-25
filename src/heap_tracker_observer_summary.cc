// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include <iostream>

#include "heap_tracker_observer_summary.h"

HeapObserverSummary::HeapObserverSummary(
  HeapTrackOptions const heap_track_options)
    : heap_track_options_{heap_track_options} {
  TRACE;
}

HeapObserverSummary::~HeapObserverSummary() {
  TRACE;
}

void
HeapObserverSummary::OnAlloc(AllocCallbackInfo const & alloc_cb_info) {
  TRACE;
  stats_.OnAlloc(alloc_cb_info);
}

void
HeapObserverSummary::OnFree(FreeCallbackInfo const & free_cb_info) {
  TRACE;
  stats_.OnFree(free_cb_info);
}

void
HeapObserverSummary::OnComplete() {
  TRACE;
}

void
HeapObserverSummary::Dump() const {
  std::cout << "SummaryStats = " << stats_.ToString() << "\n";
}

void
HeapObserverSummary::Reset() {
  stats_.Reset();
}
