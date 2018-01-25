// Copyright 2017, Arun Saha <arunksaha@gmail.com>

#include "heap_tracker_observer_timeseries_file.h"

HeapObserverTimeseriesFile::HeapObserverTimeseriesFile(
  HeapTrackOptions const heap_track_options, std::string const & filename)
    : heap_track_options_{heap_track_options},
      filename_{filename},
      out_{filename_, std::ofstream::out | std::ofstream::trunc} {
  TRACE;
}

HeapObserverTimeseriesFile::~HeapObserverTimeseriesFile() {
  TRACE;
  Flush();
  out_.close();

  char buffer[256] = {};
  snprintf(buffer, sizeof buffer, "Observer Timeseries saved to file %s\n",
           filename_.c_str());
  write(STDOUT_FILENO, buffer, sizeof buffer);
}

HeapTrackOptions
HeapObserverTimeseriesFile::GetHeapTrackOptions() const {
  return heap_track_options_;
}

void
HeapObserverTimeseriesFile::OnAlloc(AllocCallbackInfo const & alloc_cb_info) {
  TRACE;
  offline_entries_[size_++] = OfflineEntry{alloc_cb_info};
  FlushIfFull();
}

void
HeapObserverTimeseriesFile::OnFree(FreeCallbackInfo const & free_cb_info) {
  TRACE;
  offline_entries_[size_++] = OfflineEntry{free_cb_info};
  FlushIfFull();
}

void
HeapObserverTimeseriesFile::OnComplete() {
  TRACE;
  Flush();
}

void
HeapObserverTimeseriesFile::Dump() const {}

void
HeapObserverTimeseriesFile::Reset() {
  // Not sure what is the best thing to do here.
  assert(0);
}

void
HeapObserverTimeseriesFile::Flush() {
  TRACE;
  for (size_t i = 0; i != size_; ++i) {
    out_ << offline_entries_[i];
  }
  size_ = 0;
  out_.flush();
}

void
HeapObserverTimeseriesFile::FlushIfFull() {
  if (size_ == kMaxBufferCount) {
    Flush();
  }
}

#if 0
void
HeapObserverTimeseriesFile::OpenIfNot() {
  if (!out_.is_open()) {
    out_.open(filename_, std::ofstream::out | std::ofstream::trunc);
    if (!out_.is_open()) {
      std::cerr << "Error: Could not open file = " << filename_
        << "; error = " << strerror(errno) << '\n';
      exit(1);
    }
  }
}
#endif

////////////////////////////////////////////////////////////////////////////////
