// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_TRACKER_H
#define HEAP_TRACKER_H

#include <array>
#include <cassert>
#include <cstdio>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "heap_tracker_utils.h"

// We make an assumption on the maximum stack depth.
// If this is not enough, then there would be an error message.
enum { StackMaxDepth = 16 };

typedef void const * HeapPointer;
typedef size_t HeapSize;

struct CallStack {
  CallStack() = default;
  void Dump() const;
  std::string ToString() const;
  bool operator==(CallStack const & rhs) const;

  // An array to store the addresses of a call stack.
  // The stack addresses, actually void*, are stored here as uint64_t
  // for ease of numeric operations.
  std::array<uint64_t, StackMaxDepth> addresses;
};

struct CallStackHasher {
  size_t
  operator()(CallStack const & callstack) const {
    size_t result = 0;
    for (auto const addr : callstack.addresses) {
      result ^= addr;
    }
    return result;
  }
};

/// Information provided on allocation callback, see Observer interface.
struct AllocCallbackInfo {
  HeapPointer alloc_cb_ptr{nullptr};
  HeapSize alloc_cb_size{0};
  TimePoint alloc_cb_timepoint;
  CallStack alloc_cb_callstack;
};

typedef std::unordered_map<HeapPointer, AllocCallbackInfo>
    PointerToCallbackInfoMap;
// TODO Add Pool Allocator.

/// Information provided on free callback, see Observer interface.
struct FreeCallbackInfo {
  HeapPointer free_cb_ptr{nullptr};
  HeapSize free_cb_size{0};
  TimePoint free_cb_timepoint;
};

////////////////////////////////////////////////////////////////////////////////

/// Amount of detail to track, caller specified.
struct HeapTrackOptions {
  bool track_size{false};
  bool track_timepoint{false};
  bool track_callstack{false};

  bool TrackingNecessary() const;

  HeapTrackOptions() = default;
  HeapTrackOptions(bool need_size, bool need_timepoint, bool need_callstack);
};

////////////////////////////////////////////////////////////////////////////////

/// Observer interface for caller. Caller may use one of the provided observers
/// or deploy a custom observer.
class HeapTracker;
class AbstractHeapObserver {
 public:
  virtual ~AbstractHeapObserver() = 0;
  virtual HeapTrackOptions GetHeapTrackOptions() const = 0;
  virtual void OnAlloc(AllocCallbackInfo const & alloc_cb_info) = 0;
  virtual void OnFree(FreeCallbackInfo const & free_cb_info) = 0;
  virtual void OnComplete() = 0;
  void SetHeapTracker(HeapTracker const * heap_tracker);

 protected:
  HeapTracker const * heap_tracker_{nullptr};
};

////////////////////////////////////////////////////////////////////////////////

/// Watcher interface for HeapTracker. This is how HeapTracker gets to know
/// about calls to alloc and free.
class AbstractHeapWatch {
 public:
  virtual void OnNewCallback(void const * ptr, size_t size) = 0;
  virtual void OnDeleteCallback(void const * ptr) = 0;
  virtual ~AbstractHeapWatch() = 0;
};

enum HeapWatchType { HeapWatchTypeTcmalloc = 0 };

////////////////////////////////////////////////////////////////////////////////

/// Heap Tracking Tool
class HeapTracker {
 public:
  explicit HeapTracker(
      HeapWatchType const heap_watch_type = HeapWatchTypeTcmalloc);
  ~HeapTracker() = default;
  HeapTracker(HeapTracker const &) = delete;
  HeapTracker & operator=(HeapTracker const &) = delete;

  /// Attach on observer; pointer only, no ownership.
  void ObserverAttach(AbstractHeapObserver * observer);
  /// Detach existing observer.
  void ObserverDetach();

  /// Start tracking alloc and free.
  void StartTrackingAll();
  /// Stop tracking alloc and free.
  void StopTrackingAll();
  /// Stop tracking alloc but keep tracking free (soft stop).
  void StopTrackingAllocations();

  /// Callback (from watcher) for alloc.
  void OnNewCallback(void const * ptr, size_t size);
  /// Callback (from watcher) for free.
  void OnDeleteCallback(void const * ptr);

  PointerToCallbackInfoMap const & GetPointerToCallbackInfoMap() const;

 private:
  /// Invoke OnAlloc() on attached observer.
  void ObserverNotifyOnAlloc(AllocCallbackInfo const & alloc_cb_info);
  /// Invoke OnFree() on attached observer.
  void ObserverNotifyOnFree(FreeCallbackInfo const & free_cb_info);
  /// Invoke OnStop() on attached observer.
  void ObserverNotifyOnStop();

  void InsertIntoPointerToCallbackInfoMap(
      HeapPointer ptr, AllocCallbackInfo const & alloc_cb_info);
  AllocCallbackInfo const & FindInPointerToCallbackInfoMap(
      HeapPointer ptr, bool must_exist = true) const;
  void EraseFromPointerToCallbackInfoMap(HeapPointer ptr);

 private:
  // Pointer only, no ownership.
  AbstractHeapObserver * observer_{nullptr};
  HeapTrackOptions options_;

  std::unique_ptr<AbstractHeapWatch> watcher_{nullptr};

  // Used iff options_.TrackingNecessary() is true.
  PointerToCallbackInfoMap pointer_to_cbinfo_map_;

  // General bookkeeping.
  bool tracking_{false};
  TimePoint start_timepoint_;
  TimePoint stop_timepoint_;
  TimePoint soft_stop_timepoint_;
};

////////////////////////////////////////////////////////////////////////////////
// Free Functions Declarations.

/// Create a concrete HeapWatch as specified by 'heap_watch_type'
/// that associates with HeapTracker 'heap_tracker'.
std::unique_ptr<AbstractHeapWatch> HeapWatchFactory(
    HeapWatchType const heap_watch_type, HeapTracker * heap_tracker);

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions.

inline AbstractHeapObserver::~AbstractHeapObserver() {}

inline AbstractHeapWatch::~AbstractHeapWatch() {}

inline HeapTrackOptions::HeapTrackOptions(bool need_size, bool need_timepoint,
                                          bool need_callstack)
    : track_size{need_size},
      track_timepoint{need_timepoint},
      track_callstack{need_callstack} {}

inline bool
HeapTrackOptions::TrackingNecessary() const {
  return track_size || track_callstack;
}

inline PointerToCallbackInfoMap const &
HeapTracker::GetPointerToCallbackInfoMap() const {
  return pointer_to_cbinfo_map_;
}

////////////////////////////////////////////////////////////////////////////////

#endif
