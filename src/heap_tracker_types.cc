// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <sstream>

#include "heap_tracker_types.h"

////////////////////////////////////////////////////////////////////////////////

CallStack::CallStack() {
  // REVISIT: Consider zero filling only the rest of the array.
  addresses.fill(0);
}


bool
CallStack::operator==(CallStack const & rhs) const {
  return addresses == rhs.addresses;
}


bool
CallStack::Empty() const {
  // Return true iff all elements are 0.
  return std::all_of(std::cbegin(addresses), std::cend(addresses),
                     [](uint64_t addr) { return addr == 0; });
}


std::string
CallStack::ToString() const {
  if (Empty()) {
    return "";
  }
  std::ostringstream oss;
  oss << std::hex << std::uppercase;
  oss << "callstack = { ";
  for (auto const & addr : addresses) {
    // The assumption is there cannot be valid address after 0x0.
    if (addr == 0) break;
    oss << "0x" << addr << " ";
  }
  oss << "} ";
  return oss.str();
}

////////////////////////////////////////////////////////////////////////////////

std::string
AllocCallbackInfo::ToString() const {
  auto const usecs_count = TimePointToUsecsCount(alloc_cb_timepoint);
  std::ostringstream oss;
  oss << "ptr = " << std::hex << std::uppercase << alloc_cb_ptr << std::dec
      << ", size = " << alloc_cb_size << ", timepoint = " << usecs_count;
  return oss.str() + " " + alloc_cb_callstack.ToString();
}

////////////////////////////////////////////////////////////////////////////////

void
InsertIntoPointerToCallbackInfoMap(
  PointerToCallbackInfoMap & pointer_to_cbinfo_map, HeapPointer const ptr,
  AllocCallbackInfo const & alloc_cb_info) {
  auto const result = pointer_to_cbinfo_map.insert({ptr, alloc_cb_info});
  assert(result.second);  // Insert is succesful.
  (void)result;
}


PointerToCallbackInfoMapConstIter
FindInPointerToCallbackInfoMap(
  PointerToCallbackInfoMap const & pointer_to_cbinfo_map,
  HeapPointer const ptr) {
  return pointer_to_cbinfo_map.find(ptr);
}


void
EraseFromPointerToCallbackInfoMap(
  PointerToCallbackInfoMap & pointer_to_cbinfo_map, HeapPointer const ptr) {
  pointer_to_cbinfo_map.erase(ptr);
}


std::string
ToStringPointerToCallbackInfoMap(
  PointerToCallbackInfoMap const & pointer_to_cbinfo_map) {
  std::ostringstream oss;
  oss << "PointerToCallbackInfoMap: {\n";
  for (auto const & kv : pointer_to_cbinfo_map) {
    oss << "ptr = " << std::hex << kv.first << std::dec
        << ", alloc_cb_info = " << kv.second.ToString() << '\n';
  }
  oss << "}\n";
  return oss.str();
}


////////////////////////////////////////////////////////////////////////////////


// Convert TimePoint to human readable string in "YYYY-MM-DD HH:MM:SS" format.
std::string
TimePointToString(const TimePoint & timepoint) {
  std::time_t const timepoint_t =
    std::chrono::system_clock::to_time_t(timepoint);
  struct tm * timeinfo = localtime(&timepoint_t);
  char buffer[80];
  strftime(buffer, sizeof buffer, "%Y-%m-%d %H:%M:%S.", timeinfo);
  std::chrono::microseconds const usecs_since_epoch =
    std::chrono::duration_cast<std::chrono::microseconds>(
      timepoint.time_since_epoch());
  int64_t const usecs_part = usecs_since_epoch.count() % 1000000;
  char usecs_buffer[7];
  snprintf(usecs_buffer, sizeof usecs_buffer, "%06ld", usecs_part);
  return std::string(buffer) + std::string(usecs_buffer);
}


int64_t
TimePointToUsecsCount(TimePoint const & time_point) {
  std::chrono::microseconds const usecs_since_epoch =
    std::chrono::duration_cast<std::chrono::microseconds>(
      time_point.time_since_epoch());
  return usecs_since_epoch.count();
}

////////////////////////////////////////////////////////////////////////////////

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


void
SummaryStats::OnAlloc(AllocCallbackInfo const & alloc_cb_info) {
  cumulative_alloc_count_ += 1;
  cumulative_alloc_bytes_ += alloc_cb_info.alloc_cb_size;

  auto const current_outstanding_alloc_count = OutstandingAllocCount();
  if (current_outstanding_alloc_count > peak_outstanding_alloc_count_) {
    peak_outstanding_alloc_count_         = current_outstanding_alloc_count;
    peak_outstanding_alloc_count_instant_ = alloc_cb_info.alloc_cb_timepoint;
  }

  auto const current_outstanding_alloc_bytes = OutstandingAllocBytes();
  if (current_outstanding_alloc_bytes > peak_outstanding_alloc_bytes_) {
    peak_outstanding_alloc_bytes_         = current_outstanding_alloc_bytes;
    peak_outstanding_alloc_bytes_instant_ = alloc_cb_info.alloc_cb_timepoint;
  }
}


void
SummaryStats::OnFree(FreeCallbackInfo const & free_cb_info) {
  cumulative_free_count_ += 1;
  cumulative_free_bytes_ += free_cb_info.free_cb_size;
}


void
SummaryStats::Reset() {
  cumulative_alloc_count_               = 0;
  cumulative_free_count_                = 0;
  cumulative_alloc_bytes_               = 0;
  cumulative_free_bytes_                = 0;
  peak_outstanding_alloc_count_         = 0;
  peak_outstanding_alloc_count_instant_ = TimePoint{};
  peak_outstanding_alloc_bytes_         = 0;
  peak_outstanding_alloc_bytes_instant_ = TimePoint{};
}


////////////////////////////////////////////////////////////////////////////////


OfflineEntry::OfflineEntry(AllocCallbackInfo const & alloc_cb_info)
    : offline_ptr{alloc_cb_info.alloc_cb_ptr},
      offline_size{alloc_cb_info.alloc_cb_size},
      offline_timepoint{alloc_cb_info.alloc_cb_timepoint},
      offline_callstack{alloc_cb_info.alloc_cb_callstack},
      offline_alloc{true} {}


OfflineEntry::OfflineEntry(FreeCallbackInfo const & free_cb_info)
    : offline_ptr{free_cb_info.free_cb_ptr},
      offline_size{free_cb_info.free_cb_size},
      offline_timepoint{free_cb_info.free_cb_timepoint},
      offline_alloc{false} {}


static void
TokenizeString(std::string const & str, char const delim,
               std::vector<std::string> * str_token_vec) {
  assert(str_token_vec);
  std::stringstream ss;
  ss.str(str);
  for (std::string token; std::getline(ss, token, delim);) {
    str_token_vec->push_back(token);
  }
}


static std::vector<std::string>
GetTokens(std::string const & str, char const delim, size_t const hint_size) {
  std::vector<std::string> str_token_vec;
  str_token_vec.reserve(hint_size);
  TokenizeString(str, delim, &str_token_vec);
  return str_token_vec;
}


// Sample lines:
// A 0x19df6a0 24 1498050096199965
// A 0x19df6a0 24 1498050096199965 callstack = { }
// A 0x19df8f0 16 1498050096199970 callstack = { 0x3002 0x2040 }
// D 0x19df6a0 0 1498050096199975 callstack = { }
OfflineEntry::OfflineEntry(std::string const & line) {
  if (line.empty()) return;    // Ignore empty lines.
  if (line[0] == '#') return;  // Ignore comment lines.

  size_t constexpr min_size = 4;
  std::vector<std::string> const token_vec =
    GetTokens(line, OfflineEntry::delim, min_size);
  assert(token_vec.size() >= min_size);

  std::string const & first_token = token_vec[0];
  assert(first_token.size() == 1);
  char const first_char = first_token[0];
  if (first_char == 'A') {
    offline_alloc = true;
  }
  else if (first_char == 'D') {
    offline_alloc = false;
  }
  else {
    assert(0);
  }

  std::string const & ptr_str = token_vec[1];
  static_assert(sizeof(size_t) == sizeof(HeapPointer), "");
  offline_ptr =
    reinterpret_cast<HeapPointer>(std::stoull(ptr_str, nullptr, 16));

  std::string const & size_str = token_vec[2];
  offline_size                 = std::stoull(size_str, nullptr, 10);

  std::string const & tp_str = token_vec[3];
  int64_t const tp_count     = std::stoll(tp_str, nullptr, 10);
  std::chrono::microseconds const usecs{tp_count};
  offline_timepoint = TimePoint{usecs};

  for (size_t idx = min_size, addr_idx = 0; idx < token_vec.size(); ++idx) {
    std::string const & token = token_vec[idx];
    if ((token == "callstack") || (token == "=") || (token == "{") ||
        (token == "}")) {
      continue;
    }
    size_t const address                    = std::stoull(token, nullptr, 16);
    offline_callstack.addresses[addr_idx++] = address;
  }

  // std::cout
  //   << "a/d = " << first_char << ", ptr = " << offline_ptr
  //   << ", size = " << offline_size
  //   << ", timepoint = " << tp_count
  //   << ", " << offline_callstack.ToString()
  //   << '\n';
}


std::ostream &
operator<<(std::ostream & os, OfflineEntry const & offline_entry) {
  auto const usecs_count =
    TimePointToUsecsCount(offline_entry.offline_timepoint);

  os << (offline_entry.offline_alloc ? 'A' : 'D') << OfflineEntry::delim
     << offline_entry.offline_ptr << OfflineEntry::delim
     << offline_entry.offline_size << OfflineEntry::delim << usecs_count
     << OfflineEntry::delim << offline_entry.offline_callstack.ToString()
     << '\n';
  return os;
}

////////////////////////////////////////////////////////////////////////////////

void
UpdatePointerToCallbackInfoMap(PointerToCallbackInfoMap & pointer_to_cbinfo_map,
                               OfflineEntry const & offline_entry) {
  if (offline_entry.offline_alloc) {
    AllocCallbackInfo alloc_cbinfo;
    alloc_cbinfo.alloc_cb_ptr       = offline_entry.offline_ptr;
    alloc_cbinfo.alloc_cb_size      = offline_entry.offline_size;
    alloc_cbinfo.alloc_cb_timepoint = offline_entry.offline_timepoint;
    InsertIntoPointerToCallbackInfoMap(pointer_to_cbinfo_map,
                                       offline_entry.offline_ptr, alloc_cbinfo);
  }
  else {
    EraseFromPointerToCallbackInfoMap(pointer_to_cbinfo_map,
                                      offline_entry.offline_ptr);
  }
}

////////////////////////////////////////////////////////////////////////////////


AllocCallbackInfo::AllocCallbackInfo(OfflineEntry const & offline_entry) {
  assert(offline_entry.offline_alloc);
  alloc_cb_ptr       = offline_entry.offline_ptr;
  alloc_cb_size      = offline_entry.offline_size;
  alloc_cb_timepoint = offline_entry.offline_timepoint;
  alloc_cb_callstack = offline_entry.offline_callstack;
}

////////////////////////////////////////////////////////////////////////////////


FreeCallbackInfo::FreeCallbackInfo(OfflineEntry const & offline_entry) {
  assert(!offline_entry.offline_alloc);
  free_cb_ptr       = offline_entry.offline_ptr;
  free_cb_size      = offline_entry.offline_size;
  free_cb_timepoint = offline_entry.offline_timepoint;
}

////////////////////////////////////////////////////////////////////////////////


OutstandingReport
PrepareOutstandingReport(
  PointerToCallbackInfoMap const & pointer_to_cbinfo_map) {
  CallStackAggMap callstack_map;

  for (auto const & ptr_cbinfo_kv : pointer_to_cbinfo_map) {
    // HeapPointer const ptr = ptr_cbinfo_kv.first;
    AllocCallbackInfo const & alloc_cb_info = ptr_cbinfo_kv.second;
    CallStack const & callstack             = alloc_cb_info.alloc_cb_callstack;
    auto it                                 = callstack_map.find(callstack);
    if (it == callstack_map.end()) {
      callstack_map.emplace(std::piecewise_construct,
                            std::forward_as_tuple(callstack),
                            std::forward_as_tuple(alloc_cb_info.alloc_cb_size));
    }
    else {
      CallStackAggValue & agg_value = it->second;
      agg_value.agg_count_ += 1;
      agg_value.agg_size_ += alloc_cb_info.alloc_cb_size;
    }
  }
  std::cout << ToStringCallStackAggMap(callstack_map);

  std::vector<CallStackInfo> callstack_vec;
  callstack_vec.reserve(callstack_map.size());
  for (auto const & kv : callstack_map) {
    callstack_vec.emplace_back(kv.first, kv.second);
  }

  std::sort(begin(callstack_vec), end(callstack_vec),
            CallStackDescSizeComparator{});

  OutstandingReport const outstanding_report{std::move(callstack_vec)};
  return outstanding_report;
}

////////////////////////////////////////////////////////////////////////////////

std::pair<SummaryStats, PointerToCallbackInfoMap>
DecodeOfflineEntries(std::vector<OfflineEntry> const & offline_entry_vec) {
  SummaryStats summary_stats;
  PointerToCallbackInfoMap pointer_to_cbinfo_map;
  for (auto const & entry : offline_entry_vec) {
    UpdateFromOfflineEntry(entry, summary_stats, pointer_to_cbinfo_map);
  }
  return {summary_stats, pointer_to_cbinfo_map};
}

////////////////////////////////////////////////////////////////////////////////

void
UpdateFromOfflineEntry(
  OfflineEntry const & entry,
  SummaryStats & summary_stats,
  PointerToCallbackInfoMap & pointer_to_cbinfo_map) {

  if (entry.offline_alloc) {
    AllocCallbackInfo const alloc_cb_info{entry};
    InsertIntoPointerToCallbackInfoMap(pointer_to_cbinfo_map,
                                       entry.offline_ptr, alloc_cb_info);
    summary_stats.OnAlloc(alloc_cb_info);
  }
  else {
    PointerToCallbackInfoMapConstIter const cit =
      FindInPointerToCallbackInfoMap(pointer_to_cbinfo_map,
                                     entry.offline_ptr);

    if (cit != pointer_to_cbinfo_map.cend()) {
      AllocCallbackInfo const & alloc_cb_info = cit->second;
      assert(cit->first == alloc_cb_info.alloc_cb_ptr);

      FreeCallbackInfo free_cb_info;
      free_cb_info.free_cb_ptr       = cit->first;
      free_cb_info.free_cb_size      = alloc_cb_info.alloc_cb_size;
      free_cb_info.free_cb_timepoint = entry.offline_timepoint;

      EraseFromPointerToCallbackInfoMap(pointer_to_cbinfo_map, cit->first);

      summary_stats.OnFree(free_cb_info);
    }
  }
}

////////////////////////////////////////////////////////////////////////////////

CallStackInfo::CallStackInfo(CallStack const & callstack,
                             CallStackAggValue const & callstack_agg_value)
    : callstack_{callstack}, info_{callstack_agg_value} {}


std::string
CallStackInfo::ToString() const {
  return info_.ToString() + ", " + callstack_.ToString();
}

////////////////////////////////////////////////////////////////////////////////


CallStackAggValue::CallStackAggValue(HeapSize const heap_size)
    : agg_count_{1}, agg_size_{heap_size} {}


std::string
CallStackAggValue::ToString() const {
  return std::string{"bytes = "} + std::to_string(agg_size_) +
         std::string{", count = "} + std::to_string(agg_count_);
}

////////////////////////////////////////////////////////////////////////////////

std::string
ToStringCallStackAggMap(CallStackAggMap const & callstack_map) {
  std::ostringstream oss;
  oss << "CallStackAggMap: {\n";
  for (auto const & kv : callstack_map) {
    oss << kv.first.ToString() << "; " << kv.second.ToString() << '\n';
  }
  oss << "}\n";
  return oss.str();
}

////////////////////////////////////////////////////////////////////////////////

std::string
OutstandingReport::ToString() const {
  std::ostringstream oss;
  for (auto const & callstack_info : callstack_info_vec_) {
    oss << callstack_info.ToString() << '\n';
  }
  return oss.str();
}
