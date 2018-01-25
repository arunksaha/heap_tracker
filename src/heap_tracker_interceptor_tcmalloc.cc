// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#include "heap_tracker_interceptor_tcmalloc.h"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include "heap_tracker_dispatcher.h"

#ifdef HEAP_TRACKER_INTERCEPT_TCMALLOC
#include <gperftools/malloc_hook.h>

////////////////////////////////////////////////////////////////////////////////
// For helping with routing outgoing callbacks to Dispatcher.
static Dispatcher * local_dispatcher_ptr = nullptr;
static bool local_inside_intercept       = false;

void
SetInterceptorDispatcher(Dispatcher * dispatcher) {
  TRACE;
  assert(dispatcher);
  assert(!local_dispatcher_ptr);
  local_dispatcher_ptr = dispatcher;
}

////////////////////////////////////////////////////////////////////////////////
// For helping with routing incoming callbacks from Tcmalloc to the
// class member functions.
static HeapTrackerInterceptorTcmalloc *
  local_heap_tracker_interceptor_tcmalloc = nullptr;

void
HeapTrackerInterceptorTcmallocNewHook(const void * ptr, size_t size) {
  local_heap_tracker_interceptor_tcmalloc->OnNewCallback(ptr, size);
}

void
HeapTrackerInterceptorTcmallocDeleteHook(const void * ptr) {
  local_heap_tracker_interceptor_tcmalloc->OnDeleteCallback(ptr);
}


////////////////////////////////////////////////////////////////////////////////
HeapTrackerInterceptorTcmalloc::HeapTrackerInterceptorTcmalloc() {
  TRACE;
  assert(!local_heap_tracker_interceptor_tcmalloc);
  local_heap_tracker_interceptor_tcmalloc = this;
  InstallNewDeleteHooks();
}


HeapTrackerInterceptorTcmalloc::~HeapTrackerInterceptorTcmalloc() {
  TRACE;
  UninstallNewDeleteHooks();
}


void
HeapTrackerInterceptorTcmalloc::InstallNewDeleteHooks() {
  const bool a = MallocHook::AddNewHook(HeapTrackerInterceptorTcmallocNewHook);
  assert(a);
  const bool b =
    MallocHook::AddDeleteHook(HeapTrackerInterceptorTcmallocDeleteHook);
  assert(b);
}


void
HeapTrackerInterceptorTcmalloc::UninstallNewDeleteHooks() {
  MallocHook::RemoveNewHook(HeapTrackerInterceptorTcmallocNewHook);
  MallocHook::RemoveDeleteHook(HeapTrackerInterceptorTcmallocDeleteHook);
}


void
HeapTrackerInterceptorTcmalloc::UninstallNewHooks() {
  MallocHook::RemoveNewHook(HeapTrackerInterceptorTcmallocNewHook);
}


void
HeapTrackerInterceptorTcmalloc::OnNewCallback(const void * ptr, size_t size) {
  if (!local_inside_intercept && local_dispatcher_ptr) {
    local_inside_intercept = true;
    local_dispatcher_ptr->OnNewCallback(ptr, size);
    local_inside_intercept = false;
  }
}

void
HeapTrackerInterceptorTcmalloc::OnDeleteCallback(const void * ptr) {
  if (!local_inside_intercept && local_dispatcher_ptr) {
    local_inside_intercept = true;
    local_dispatcher_ptr->OnDeleteCallback(ptr);
    local_inside_intercept = false;
  }
}

#endif
