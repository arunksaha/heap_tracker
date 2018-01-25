// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_TRACKER_TYPES_H
#define HEAP_TRACKER_TYPES_H

#include <unistd.h>
#include <array>
#include <chrono>
#include <cstring>
#include <string>
#include <unordered_map>
#include <vector>

////////////////////////////////////////////////////////////////////////////////

// #define TRACE_ENABLED

#ifdef TRACE_ENABLED
#define TRACE                                                           \
  do {                                                                  \
    int const dest_fd = STDOUT_FILENO;                                  \
    ssize_t const rc1 = write(dest_fd, "\n", 1);                        \
    (void)rc1;                                                          \
    ssize_t const rc2 =                                                 \
      write(dest_fd, __PRETTY_FUNCTION__, strlen(__PRETTY_FUNCTION__)); \
    (void)rc2;                                                          \
  } while (0)
#else
#define TRACE \
  do {        \
  } while (0)
#endif

////////////////////////////////////////////////////////////////////////////////

// We make an assumption on the maximum stack depth.
// If this is not enough, then there would be an error message.
enum { StackMaxDepth = 24 };

typedef void const * HeapPointer;
typedef size_t HeapSize;
typedef std::chrono::time_point<std::chrono::system_clock> TimePoint;

/// A type to capture a function callstack.
struct CallStack {
  CallStack();
  bool operator==(CallStack const & rhs) const;
  bool Empty() const;
  std::string ToString() const;

  // An array to store the addresses of a call stack.
  // The stack addresses, actually void*, are stored here as uint64_t
  // for ease of numeric operations.
  std::array<uint64_t, StackMaxDepth> addresses;
};

/// A hash functor for CallStack.
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
struct OfflineEntry;
struct AllocCallbackInfo {
  HeapPointer alloc_cb_ptr{nullptr};
  HeapSize alloc_cb_size{0};
  TimePoint alloc_cb_timepoint;
  CallStack alloc_cb_callstack;

  explicit AllocCallbackInfo(OfflineEntry const &);
  AllocCallbackInfo() = default;
  std::string ToString() const;
};

////////////////////////////////////////////////////////////////////////////////

/// A map to map from an allocated pointer to its allocation information.
typedef std::unordered_map<HeapPointer, AllocCallbackInfo>
  PointerToCallbackInfoMap;
// TODO Add Pool Allocator.
typedef PointerToCallbackInfoMap::iterator PointerToCallbackInfoMapIter;
typedef PointerToCallbackInfoMap::const_iterator
  PointerToCallbackInfoMapConstIter;

// Insert '(ptr, alloc_cb_info)' into 'pointer_to_cbinfo_map'
void InsertIntoPointerToCallbackInfoMap(
  PointerToCallbackInfoMap & pointer_to_cbinfo_map, HeapPointer ptr,
  AllocCallbackInfo const & alloc_cb_info);

// Find 'ptr' in 'pointer_to_cbinfo_map' and return const iterator.
PointerToCallbackInfoMapConstIter FindInPointerToCallbackInfoMap(
  PointerToCallbackInfoMap const & pointer_to_cbinfo_map, HeapPointer ptr);

// Erase 'ptr' from 'pointer_to_cbinfo_map'.
void EraseFromPointerToCallbackInfoMap(
  PointerToCallbackInfoMap & pointer_to_cbinfo_map, HeapPointer ptr);

std::string ToStringPointerToCallbackInfoMap(PointerToCallbackInfoMap const &);

////////////////////////////////////////////////////////////////////////////////

/// Information provided on free callback, see Observer interface.
struct FreeCallbackInfo {
  HeapPointer free_cb_ptr{nullptr};
  HeapSize free_cb_size{0};
  TimePoint free_cb_timepoint;

  explicit FreeCallbackInfo(OfflineEntry const &);
  FreeCallbackInfo() = default;
};

////////////////////////////////////////////////////////////////////////////////

struct OfflineEntry {
  HeapPointer offline_ptr{nullptr};
  HeapSize offline_size{0};
  TimePoint offline_timepoint;
  CallStack offline_callstack;
  bool offline_alloc{true};  /// true := alloc; false := dealloc

  OfflineEntry() = default;
  explicit OfflineEntry(AllocCallbackInfo const &);
  explicit OfflineEntry(FreeCallbackInfo const &);
  explicit OfflineEntry(std::string const &);

  static char constexpr delim = ' ';
};

std::ostream & operator<<(std::ostream & os,
                          OfflineEntry const & offline_entry);

void UpdatePointerToCallbackInfoMap(
  PointerToCallbackInfoMap & pointer_to_cbinfo_map,
  OfflineEntry const & offline_entry);

////////////////////////////////////////////////////////////////////////////////

/// Amount of detail to track, chosen by caller and passed to tracker.
struct HeapTrackOptions {
  bool track_size{false};
  bool track_callstack{false};
  bool track_timepoint{false};
  bool start_at_construction{false};
  bool finish_at_destruction{false};

  bool TrackingNecessary() const;
};

////////////////////////////////////////////////////////////////////////////////

// Return the TimePoint of the instant.
TimePoint GetTimePointNow();

// Convert TimePoint to human readable string in "YYYY-MM-DD HH:MM:SS" format.
std::string TimePointToString(TimePoint const & time_point);

// Convert TimePoint to number of microseconds since epoch.
int64_t TimePointToUsecsCount(TimePoint const & time_point);

////////////////////////////////////////////////////////////////////////////////

// Basic stats. Note all cumulative numbers are always increasing.
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

  // Return number of allocations outstanding.
  int64_t OutstandingAllocCount() const;

  // Return number of bytes outstanding.
  int64_t OutstandingAllocBytes() const;

  // Representation as JSON.
  std::string ToString() const;

  // Update with allocation information.
  void OnAlloc(AllocCallbackInfo const & alloc_cb_info);

  // Update with deallocation information.
  void OnFree(FreeCallbackInfo const & free_cb_info);

  // Reset *all* counters to zero (and forget all history).
  void Reset();
};

////////////////////////////////////////////////////////////////////////////////

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
struct CallStackDescSizeComparator {
  bool
  operator()(CallStackInfo const & a, CallStackInfo const & b) const {
    return a.info_.agg_size_ > b.info_.agg_size_;
  }
};

/// Callstack aggregation map aggregates information on per-callstack basis.
typedef std::unordered_map<CallStack, CallStackAggValue, CallStackHasher>
  CallStackAggMap;

/// Return human readable string representation of a CallStackAggMap.
std::string ToStringCallStackAggMap(CallStackAggMap const &);

/// Outstanding callstacks aggregated.
struct OutstandingReport {
  explicit OutstandingReport(std::vector<CallStackInfo> && callstack_info_vec)
      : callstack_info_vec_{std::move(callstack_info_vec)} {}
  std::string ToString() const;
  std::vector<CallStackInfo> const callstack_info_vec_;
};

OutstandingReport PrepareOutstandingReport(PointerToCallbackInfoMap const &);

std::pair<SummaryStats, PointerToCallbackInfoMap> DecodeOfflineEntries(
  std::vector<OfflineEntry> const &);

////////////////////////////////////////////////////////////////////////////////
// Inline Definitions.

inline bool
HeapTrackOptions::TrackingNecessary() const {
  return track_size || track_callstack;
}

inline TimePoint
GetTimePointNow() {
  return std::chrono::system_clock::now();
}

inline int64_t
SummaryStats::OutstandingAllocCount() const {
  return cumulative_alloc_count_ - cumulative_free_count_;
}

inline int64_t
SummaryStats::OutstandingAllocBytes() const {
  return cumulative_alloc_bytes_ - cumulative_free_bytes_;
}

#endif
