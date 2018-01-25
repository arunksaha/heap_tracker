// Copyright 2017, Arun Saha <arunksaha@gmail.com>

#include "heap_tracker_interceptor_interposition.h"
#include <dlfcn.h>
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include "heap_tracker_dispatcher.h"

#ifdef HEAP_TRACKER_INTERCEPT_INTERPOSITION

////////////////////////////////////////////////////////////////////////////////
// Declaration of memory management function types to be interpositioned.
using MallocFunc  = void * (*)(size_t size);
using FreeFunc    = void (*)(void * ptr);
using CallocFunc  = void * (*)(size_t nelem, size_t elsize);
using ReallocFunc = void * (*)(void * ptr, size_t size);

////////////////////////////////////////////////////////////////////////////////
// For helping with routing outgoing callbacks to Dispatcher.
static Dispatcher * local_dispatcher_ptr = nullptr;
// A flag to track if we are inside intercept and prevent infinite loop.
static bool local_inside_intercept = false;

void
SetInterceptorDispatcher(Dispatcher * const dispatcher) {
  TRACE;
  assert(dispatcher);
  assert(!local_dispatcher_ptr);
  local_dispatcher_ptr = dispatcher;
}

////////////////////////////////////////////////////////////////////////////////
void *
malloc(size_t const size) {
  TRACE;
  static MallocFunc real_malloc = nullptr;

  if (!real_malloc) {
    real_malloc = reinterpret_cast<MallocFunc>(dlsym(RTLD_NEXT, "malloc"));
    char const * const error = dlerror();
    if (error) {
      fputs(error, stderr);
      exit(1);
    }
  }

  assert(real_malloc);
  void * const ptr = real_malloc(size);

  if (!local_inside_intercept && local_dispatcher_ptr) {
    local_inside_intercept = true;
    local_dispatcher_ptr->OnNewCallback(ptr, size);
    local_inside_intercept = false;
  }

  return ptr;
}

////////////////////////////////////////////////////////////////////////////////
void
free(void * const ptr) {
  TRACE;
  static FreeFunc real_free = nullptr;

  if (!real_free) {
    real_free = reinterpret_cast<FreeFunc>(dlsym(RTLD_NEXT, "free"));
    char const * const error = dlerror();
    if (error) {
      fputs(error, stderr);
      exit(1);
    }
  }

  assert(real_free);
  real_free(ptr);

  if (!local_inside_intercept && local_dispatcher_ptr) {
    local_inside_intercept = true;
    local_dispatcher_ptr->OnDeleteCallback(ptr);
    local_inside_intercept = false;
  }
}

////////////////////////////////////////////////////////////////////////////////
static void * temporary_calloc(size_t, size_t) {
  TRACE;
  return nullptr;
}

void *
calloc(size_t const nelem, size_t const elsize) {
  TRACE;
  static CallocFunc real_calloc = nullptr;

  // // HACK from https://sourceware.org/ml/libc-help/2008-11/msg00008.html
  // static int calloc_counter = 0;
  // ++calloc_counter;
  // if (calloc_counter == 2) {
  //   enum {N = 32};
  //   static char calloc_buffer[N] = {};
  //   if (elsize > N) {
  //     fprintf(stderr, "ERROR: calloc static buffer issue, "
  //       "nelem = %zd, elsize = %zd\n", nelem, elsize);
  //     abort();
  //   }
  //   return static_cast<void *>(calloc_buffer);
  // }

  if (!real_calloc) {
    // HACK from  http://blog.bigpixel.ro/2010/09/interposing-calloc-on-linux/
    real_calloc = temporary_calloc;

    real_calloc = reinterpret_cast<CallocFunc>(dlsym(RTLD_NEXT, "calloc"));
    char const * const error = dlerror();
    if (error) {
      fputs(error, stderr);
      exit(1);
    }
  }

  assert(real_calloc);
  void * const ptr = real_calloc(nelem, elsize);

  if (!local_inside_intercept && local_dispatcher_ptr) {
    local_inside_intercept  = true;
    size_t const total_size = nelem * elsize;
    local_dispatcher_ptr->OnNewCallback(ptr, total_size);
    local_inside_intercept = false;
  }

  return ptr;
}

////////////////////////////////////////////////////////////////////////////////
void *
realloc(void * const old_ptr, size_t const size) {
  TRACE;
  static ReallocFunc real_realloc = nullptr;

  if (!real_realloc) {
    real_realloc = reinterpret_cast<ReallocFunc>(dlsym(RTLD_NEXT, "realloc"));
    char const * const error = dlerror();
    if (error) {
      fputs(error, stderr);
      exit(1);
    }
  }

  assert(real_realloc);
  void * const new_ptr = real_realloc(old_ptr, size);

  if (!local_inside_intercept && local_dispatcher_ptr) {
    local_inside_intercept = true;
    local_dispatcher_ptr->OnDeleteCallback(old_ptr);
    local_dispatcher_ptr->OnNewCallback(new_ptr, size);
    local_inside_intercept = false;
  }

  return new_ptr;
}

#endif
