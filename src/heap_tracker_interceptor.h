// Copyright 2017, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_TRACKER_INTERCEPTOR_H
#define HEAP_TRACKER_INTERCEPTOR_H

#include <cassert>
#include <iostream>

#define LTRACE std::cout << '\n' << __PRETTY_FUNCTION__ << '\n'

// Exactly one of the following must be defined.
//  - HEAP_TRACKER_INTERCEPT_TCMALLOC
//  - HEAP_TRACKER_INTERCEPT_INTERPOSITION

#ifdef HEAP_TRACKER_INTERCEPT_TCMALLOC
#include "heap_tracker_interceptor_tcmalloc.h"
static const HeapTrackerInterceptorTcmalloc setup;
#elif defined HEAP_TRACKER_INTERCEPT_INTERPOSITION
#include "heap_tracker_interceptor_interposition.h"
#endif

// Save the Dispatcher pointer.
// The function is defined by each interceptor implementation.
class Dispatcher;
void SetInterceptorDispatcher(Dispatcher * dispatcher);


////////////////////////////////////////////////////////////////////////////////
// The following is just for safety check and is not necessary for
// functionality. It checks that exactly one macro is defined.
struct HeapTrackerInterceptor {
  HeapTrackerInterceptor() { Audit(); }
  void Audit() const;
};

inline void
HeapTrackerInterceptor::Audit() const {
  // LTRACE;
  int define_counter = 0;
#ifdef HEAP_TRACKER_INTERCEPT_TCMALLOC
  define_counter += 1;
#endif
#ifdef HEAP_TRACKER_INTERCEPT_INTERPOSITION
  define_counter += 1;
#endif
  // Exactly one intercept macro must be defined.
  assert(define_counter == 1);
}

static const HeapTrackerInterceptor heap_tracker_interceptor;
////////////////////////////////////////////////////////////////////////////////

#endif
