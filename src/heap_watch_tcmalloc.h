// Copyright 2016, Arun Saha <arunksaha@gmail.com>

#ifndef HEAP_WATCH_TCMALLOC_H
#define HEAP_WATCH_TCMALLOC_H

#include <gperftools/malloc_hook.h>
#include "heap_tracker.h"

class HeapTracker;

void InstallNewDeleteHooks();
void UninstallNewDeleteHooks();
void UninstallNewHooks();

class HeapWatchTcmalloc final : public AbstractHeapWatch {
 public:
  explicit HeapWatchTcmalloc(HeapTracker * heap_tracker);

  virtual void OnNewCallback(const void * ptr, size_t size) override;
  virtual void OnDeleteCallback(const void * ptr) override;

 private:
  // Pointer only, no ownership.
  HeapTracker * heap_tracker_{nullptr};
};

// On construction disable heap watch and disable on destruction.
class HeapWatchGuard {
 public:
  HeapWatchGuard();
  ~HeapWatchGuard();
};

#endif
