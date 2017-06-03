// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_watch_tcmalloc.h"
#include "heap_tracker.h"

////////////////////////////////////////////////////////////////////////////////
// Setup for callback from tcmalloc.

static HeapWatchTcmalloc * global_heap_watch_tcmalloc_ptr = nullptr;

void
HeapWatchTcmallocNewHook(const void * ptr, size_t size) {
  global_heap_watch_tcmalloc_ptr->OnNewCallback(ptr, size);
}

void
HeapWatchTcmallocDeleteHook(const void * ptr) {
  global_heap_watch_tcmalloc_ptr->OnDeleteCallback(ptr);
}

////////////////////////////////////////////////////////////////////////////////

void
InstallNewDeleteHooks() {
  const bool a = MallocHook::AddNewHook(HeapWatchTcmallocNewHook);
  assert(a);
  const bool b = MallocHook::AddDeleteHook(HeapWatchTcmallocDeleteHook);
  assert(b);
}

void
UninstallNewDeleteHooks() {
  MallocHook::RemoveNewHook(HeapWatchTcmallocNewHook);
  MallocHook::RemoveDeleteHook(HeapWatchTcmallocDeleteHook);
}

void
UninstallNewHooks() {
  MallocHook::RemoveNewHook(HeapWatchTcmallocNewHook);
}

////////////////////////////////////////////////////////////////////////////////

HeapWatchTcmalloc::HeapWatchTcmalloc(HeapTracker * heap_tracker)
    : heap_tracker_{heap_tracker} {
  assert(heap_tracker);
  assert(!global_heap_watch_tcmalloc_ptr);
  global_heap_watch_tcmalloc_ptr = this;
}

void
HeapWatchTcmalloc::OnNewCallback(const void * ptr, size_t size) {
  heap_tracker_->OnNewCallback(ptr, size);
}

void
HeapWatchTcmalloc::OnDeleteCallback(const void * ptr) {
  heap_tracker_->OnDeleteCallback(ptr);
}

////////////////////////////////////////////////////////////////////////////////

HeapWatchGuard::HeapWatchGuard() {
  UninstallNewDeleteHooks();
}

HeapWatchGuard::~HeapWatchGuard() {
  InstallNewDeleteHooks();
}
