// Copyright 2017, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_OBSERVER_INTERFACE_H
#define HEAP_OBSERVER_INTERFACE_H

#include <cassert>
#include "heap_tracker_types.h"

////////////////////////////////////////////////////////////////////////////////

/// The observer interface.
/// Applications can use one of the provided observers or deploy a custom one.
class Dispatcher;
class AbstractObserver {
 public:
  virtual ~AbstractObserver()                                   = 0;
  virtual HeapTrackOptions GetHeapTrackOptions() const          = 0;
  virtual void OnAlloc(AllocCallbackInfo const & alloc_cb_info) = 0;
  virtual void OnFree(FreeCallbackInfo const & free_cb_info)    = 0;
  virtual void OnComplete()                                     = 0;
  virtual void Reset()                                          = 0;
  virtual void Dump() const                                     = 0;
  void SetInterceptorDispatcher(Dispatcher const * dispatcher);

 protected:
  Dispatcher const * dispatcher_{nullptr};
};

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions.

inline AbstractObserver::~AbstractObserver() {}

inline void
AbstractObserver::SetInterceptorDispatcher(
  Dispatcher const * const dispatcher) {
  TRACE;
  assert(dispatcher);
  assert(!dispatcher_);
  dispatcher_ = dispatcher;
}

////////////////////////////////////////////////////////////////////////////////

#endif
