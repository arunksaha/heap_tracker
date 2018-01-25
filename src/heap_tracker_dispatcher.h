// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_DISPATCHER_H
#define HEAP_DISPATCHER_H

#include <cassert>
#include <cstdio>
#include <cstring>
#include <memory>
#include <string>

#include "heap_tracker_observer_interface.h"
#include "heap_tracker_types.h"

////////////////////////////////////////////////////////////////////////////////

class Dispatcher {
 public:
  explicit Dispatcher(AbstractObserver * observer);
  ~Dispatcher();
  Dispatcher(Dispatcher const &) = delete;
  Dispatcher & operator=(Dispatcher const &) = delete;

  /// Callback (from watcher) for alloc.
  void OnNewCallback(void const * ptr, size_t size);
  /// Callback (from watcher) for free.
  void OnDeleteCallback(void const * ptr);

  /// Start tracking alloc and free.
  void StartTrackingAll();
  /// Stop tracking alloc and free.
  void StopTrackingAll();

  PointerToCallbackInfoMap const & GetPointerToCallbackInfoMap() const;

 private:
  /// Attach on observer; pointer only, no ownership.
  void ObserverAttach(AbstractObserver * observer);
  /// Invoke OnAlloc() on attached observer.
  void ObserverNotifyOnAlloc(AllocCallbackInfo const & alloc_cb_info);
  /// Invoke OnFree() on attached observer.
  void ObserverNotifyOnFree(FreeCallbackInfo const & free_cb_info);
  /// Invoke OnStop() on attached observer.
  void ObserverNotifyOnStop();

  /// Reset PointerToCallbackInfoMap and forget everything.
  void Reset();

 private:
  // Pointer only, no ownership.
  AbstractObserver * observer_{nullptr};

  // General bookkeeping.
  TimePoint start_timepoint_;
  TimePoint stop_timepoint_;
  TimePoint soft_stop_timepoint_;
  bool tracking_{false};

  // Used iff observer_.GetHeapTrackOptions().TrackingNecessary() is true.
  PointerToCallbackInfoMap pointer_to_cbinfo_map_;
};

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions.

inline PointerToCallbackInfoMap const &
Dispatcher::GetPointerToCallbackInfoMap() const {
  return pointer_to_cbinfo_map_;
}

////////////////////////////////////////////////////////////////////////////////

#endif
