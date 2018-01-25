// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include <iostream>

#include "execinfo.h"
#include "heap_tracker_dispatcher.h"

////////////////////////////////////////////////////////////////////////////////

Dispatcher::Dispatcher(AbstractObserver * const observer) {
  TRACE;

  assert(observer);
  ObserverAttach(observer);

  if (observer_->GetHeapTrackOptions().start_at_construction) {
    StartTrackingAll();
  }
}

Dispatcher::~Dispatcher() {
  TRACE;
  if (observer_->GetHeapTrackOptions().finish_at_destruction) {
    StopTrackingAll();
  }
}

void
Dispatcher::OnNewCallback(const void * ptr, size_t size) {
  if (!ptr) return;
  if (size == 0) return;
  if (!tracking_) return;
  TRACE;
  AllocCallbackInfo cbinfo;
  cbinfo.alloc_cb_ptr  = ptr;
  cbinfo.alloc_cb_size = size;
  if (observer_->GetHeapTrackOptions().track_timepoint) {
    cbinfo.alloc_cb_timepoint = GetTimePointNow();
  }
  if (observer_->GetHeapTrackOptions().track_callstack) {
    const int depth = backtrace(
      reinterpret_cast<void **>(&cbinfo.alloc_cb_callstack.addresses[0]),
      StackMaxDepth);
    if (depth == StackMaxDepth) {
      std::cerr << __FILE__ << ":" << __LINE__ << " Depth = " << depth
                << " is larger than current StackMaxDepth = " << StackMaxDepth
                << ", please increase the value of StackMaxDepth. Exiting.\n";
      exit(1);
    }
  }

  if (observer_->GetHeapTrackOptions().TrackingNecessary()) {
    InsertIntoPointerToCallbackInfoMap(pointer_to_cbinfo_map_, ptr, cbinfo);
  }

  ObserverNotifyOnAlloc(cbinfo);
}

void
Dispatcher::OnDeleteCallback(const void * ptr) {
  if (!ptr) return;
  if (!tracking_) return;
  TRACE;
  FreeCallbackInfo cbinfo;
  cbinfo.free_cb_ptr = ptr;
  if (observer_->GetHeapTrackOptions().track_size) {
    PointerToCallbackInfoMapConstIter cit =
      FindInPointerToCallbackInfoMap(pointer_to_cbinfo_map_, ptr);
    // REVISIT Should this be an assert?
    if (cit != pointer_to_cbinfo_map_.end()) {
      AllocCallbackInfo const & alloc_cb_info = cit->second;
      cbinfo.free_cb_size                     = alloc_cb_info.alloc_cb_size;
      EraseFromPointerToCallbackInfoMap(pointer_to_cbinfo_map_, ptr);
    }
  }
  if (observer_->GetHeapTrackOptions().track_timepoint) {
    cbinfo.free_cb_timepoint = GetTimePointNow();
  }

  ObserverNotifyOnFree(cbinfo);
}

void
Dispatcher::ObserverAttach(AbstractObserver * observer) {
  assert(!observer_ && "Maximum one Observer at a time, for now");
  assert(observer);
  observer_ = observer;
  observer_->SetInterceptorDispatcher(this);
}

void
Dispatcher::ObserverNotifyOnAlloc(AllocCallbackInfo const & alloc_cb_info) {
  assert(observer_);
  observer_->OnAlloc(alloc_cb_info);
}

void
Dispatcher::ObserverNotifyOnFree(FreeCallbackInfo const & free_cb_info) {
  assert(observer_);
  observer_->OnFree(free_cb_info);
}

void
Dispatcher::StartTrackingAll() {
  assert(!tracking_);
  tracking_        = true;
  start_timepoint_ = GetTimePointNow();
}

void
Dispatcher::StopTrackingAll() {
  assert(tracking_);
  tracking_       = false;
  stop_timepoint_ = GetTimePointNow();
  observer_->OnComplete();
}

void
Dispatcher::Reset() {
  assert(!tracking_);
  observer_->Reset();
  start_timepoint_     = TimePoint{};
  stop_timepoint_      = TimePoint{};
  soft_stop_timepoint_ = TimePoint{};
  pointer_to_cbinfo_map_.clear();
}
