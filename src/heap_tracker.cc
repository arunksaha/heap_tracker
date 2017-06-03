// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_tracker.h"
#include <algorithm>
#include <cassert>
#include <iostream>
#include <memory>
#include <sstream>
#include "heap_watch_tcmalloc.h"

////////////////////////////////////////////////////////////////////////////////

std::unique_ptr<AbstractHeapWatch>
HeapWatchFactory(HeapWatchType const heap_watch_type,
                 HeapTracker * heap_tracker) {
  switch (heap_watch_type) {
    case HeapWatchTypeTcmalloc:
      return std::make_unique<HeapWatchTcmalloc>(heap_tracker);
  }
  return nullptr;
}

////////////////////////////////////////////////////////////////////////////////

void
AbstractHeapObserver::SetHeapTracker(HeapTracker const * heap_tracker) {
  assert(heap_tracker);
  assert(!heap_tracker_);
  heap_tracker_ = heap_tracker;
}

////////////////////////////////////////////////////////////////////////////////

HeapTracker::HeapTracker(HeapWatchType const heap_watch_type)
    : watcher_{HeapWatchFactory(heap_watch_type, this)} {}

void
HeapTracker::ObserverAttach(AbstractHeapObserver * observer) {
  assert(!observer_ && "Maximum one Observer at a time, for now");
  assert(observer);
  observer_ = observer;
  observer->SetHeapTracker(this);
  options_ = observer->GetHeapTrackOptions();
}

void
HeapTracker::ObserverDetach() {
  assert(observer_);
  observer_ = nullptr;
  options_ = HeapTrackOptions{};
}

void
HeapTracker::ObserverNotifyOnAlloc(AllocCallbackInfo const & alloc_cb_info) {
  assert(observer_);
  observer_->OnAlloc(alloc_cb_info);
}

void
HeapTracker::ObserverNotifyOnFree(FreeCallbackInfo const & free_cb_info) {
  assert(observer_);
  observer_->OnFree(free_cb_info);
}

void
HeapTracker::StartTrackingAll() {
  assert(!tracking_);
  tracking_ = true;
  start_timepoint_ = GetTimePointNow();
  InstallNewDeleteHooks();
}

void
HeapTracker::StopTrackingAll() {
  assert(tracking_);
  tracking_ = false;
  stop_timepoint_ = GetTimePointNow();
  UninstallNewDeleteHooks();
  observer_->OnComplete();
}

void
HeapTracker::StopTrackingAllocations() {
  assert(tracking_);
  soft_stop_timepoint_ = GetTimePointNow();
  UninstallNewHooks();
}

void
HeapTracker::OnNewCallback(const void * ptr, size_t size) {
  if (!ptr) return;
  if (size == 0) return;
  HeapWatchGuard heap_watch_guard;
  AllocCallbackInfo cbinfo;
  cbinfo.alloc_cb_ptr = ptr;
  cbinfo.alloc_cb_size = size;
  if (options_.track_timepoint) {
    cbinfo.alloc_cb_timepoint = GetTimePointNow();
  }
  if (options_.track_callstack) {
    const int skip_count_hint = 0;
    cbinfo.alloc_cb_callstack.addresses.fill(0);
    const int depth = MallocHook::GetCallerStackTrace(
        reinterpret_cast<void **>(&cbinfo.alloc_cb_callstack.addresses[0]),
        StackMaxDepth, skip_count_hint);
    if (depth > StackMaxDepth) {
      std::cerr << __FILE__ << ":" << __LINE__ << " Depth = " << depth
                << " is larger than current StackMaxDepth = " << StackMaxDepth
                << ", please increase the value of StackMaxDepth. Exiting.\n";
      exit(1);
    }
  }

  if (options_.TrackingNecessary()) {
    InsertIntoPointerToCallbackInfoMap(ptr, cbinfo);
  }

  ObserverNotifyOnAlloc(cbinfo);
}

void
HeapTracker::OnDeleteCallback(const void * ptr) {
  if (!ptr) return;
  HeapWatchGuard heap_watch_guard;
  FreeCallbackInfo cbinfo;
  cbinfo.free_cb_ptr = ptr;
  if (options_.TrackingNecessary()) {
    AllocCallbackInfo const & alloc_cb_info =
        FindInPointerToCallbackInfoMap(ptr, true);
    cbinfo.free_cb_size = alloc_cb_info.alloc_cb_size;
    EraseFromPointerToCallbackInfoMap(ptr);
  }
  if (options_.track_timepoint) {
    cbinfo.free_cb_timepoint = GetTimePointNow();
  }
  ObserverNotifyOnFree(cbinfo);
}

void
HeapTracker::InsertIntoPointerToCallbackInfoMap(
    HeapPointer const ptr, AllocCallbackInfo const & alloc_cb_info) {
  auto const result = pointer_to_cbinfo_map_.insert({ptr, alloc_cb_info});
  assert(result.second);
}

AllocCallbackInfo const &
HeapTracker::FindInPointerToCallbackInfoMap(HeapPointer const ptr,
                                            bool const must_exist) const {
  auto const iter = pointer_to_cbinfo_map_.find(ptr);
  bool const exist = iter != pointer_to_cbinfo_map_.end();
  assert(!must_exist || (must_exist && exist));
  return iter->second;
}

void
HeapTracker::EraseFromPointerToCallbackInfoMap(HeapPointer const ptr) {
  pointer_to_cbinfo_map_.erase(ptr);
}

bool
CallStack::operator==(CallStack const & rhs) const {
  return addresses == rhs.addresses;
}

std::string
CallStack::ToString() const {
  std::ostringstream oss;
  oss << std::hex << std::uppercase;
  oss << "callstack = {";
  for (auto const & addr : addresses) {
    if (addr == 0) break;
    oss << "0x" << addr << " ";
  }
  oss << "} ";
  return oss.str();
}
