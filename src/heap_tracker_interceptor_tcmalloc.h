// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_TRACKER_INTERCEPT_TCMALLOC_H
#define HEAP_TRACKER_INTERCEPT_TCMALLOC_H

#ifdef HEAP_TRACKER_INTERCEPT_TCMALLOC

#include <cstdlib>

class HeapTrackerInterceptorTcmalloc {
 public:
  HeapTrackerInterceptorTcmalloc();
  ~HeapTrackerInterceptorTcmalloc();

 private:
  friend void HeapTrackerInterceptorTcmallocNewHook(const void * ptr,
                                                    size_t size);
  friend void HeapTrackerInterceptorTcmallocDeleteHook(const void * ptr);

  void InstallNewDeleteHooks();
  void UninstallNewDeleteHooks();
  void UninstallNewHooks();
  void OnNewCallback(const void * ptr, size_t size);
  void OnDeleteCallback(const void * ptr);
};
#endif

#endif
