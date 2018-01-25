// Copyright 2018, Arun Saha <arunksaha@gmail.com>

#include <forward_list>
#include <iostream>
#include <list>
#include <memory>
#include <vector>

#include "heap_tracker_dispatcher.h"
#include "heap_tracker_interceptor.h"

#define TEST(x) assert(x)

#ifdef HEAP_TRACKER_INTERCEPT_TCMALLOC_TEST

#include "heap_tracker_observer_summary.h"

////////////////////////////////////////////////////////////////////////////////
// Return the (B, N) for 'object' where:
//  - B is the number of blocks of memory allocated by 'object'
//  - N is the total number of bytes allocated by 'object' (all blocks combined)
// Require T to be copy constructible.
template <typename T>
std::pair<int64_t, int64_t>
GetHeapConsumption(HeapObserverSummary * heap_observer_summary,
                   T const & object) {
  SummaryStats const stats_before = heap_observer_summary->GetStats();
  T const copy                    = object;
  (void)copy;
  SummaryStats const stats_after = heap_observer_summary->GetStats();

  auto const num_blocks =
    stats_after.OutstandingAllocCount() - stats_before.OutstandingAllocCount();
  auto const num_bytes =
    stats_after.OutstandingAllocBytes() - stats_before.OutstandingAllocBytes();
  return {num_blocks, num_bytes};
}

////////////////////////////////////////////////////////////////////////////////
static HeapTrackOptions
MyHeapTrackSummaryOptions() {
  HeapTrackOptions options;
  options.track_size = true;
  return options;
}

////////////////////////////////////////////////////////////////////////////////
struct Foo {
  int x;
  char y;
  int z[2];
};

static void
TestHeapConsumptionList(HeapObserverSummary * heap_observer_summary,
                        int64_t const n) {
  // Double linked list.
  std::list<Foo> foo_list;
  for (int i = 0; i != n; ++i) {
    foo_list.push_back(Foo{});
  }

  int64_t const foo_list_memory_expected =
    n * static_cast<int64_t>(sizeof(Foo) + 2 * sizeof(void *));
  auto const foo_list_memory_actual =
    GetHeapConsumption(heap_observer_summary, foo_list);
  TEST(foo_list_memory_actual.first == n);
  TEST(foo_list_memory_actual.second == foo_list_memory_expected);
}

static void
TestHeapConsumptionForwardList(HeapObserverSummary * heap_observer_summary,
                               int64_t const n) {
  // Single linked list.
  std::forward_list<Foo> foo_forward_list;
  for (int i = 0; i != n; ++i) {
    foo_forward_list.push_front(Foo{});
  }

  int64_t const foo_forward_list_memory_expected =
    n * static_cast<int64_t>(sizeof(Foo) + sizeof(void *));
  auto const foo_forward_list_memory_actual =
    GetHeapConsumption(heap_observer_summary, foo_forward_list);
  TEST(foo_forward_list_memory_actual.first == n);
  TEST(foo_forward_list_memory_actual.second ==
       foo_forward_list_memory_expected);
}

static void
TestHeapConsumptionVector(HeapObserverSummary * heap_observer_summary,
                          int64_t const n) {
  // Vector.
  std::vector<Foo> foo_vector;
  for (int i = 0; i != n; ++i) {
    foo_vector.push_back(Foo{});
  }

  int64_t const foo_vector_memory_count_expected = n == 0 ? 0 : 1;
  int64_t const foo_vector_memory_bytes_expected =
    n * static_cast<int64_t>(sizeof(Foo));
  auto const foo_vector_memory_actual =
    GetHeapConsumption(heap_observer_summary, foo_vector);
  TEST(foo_vector_memory_actual.first == foo_vector_memory_count_expected);
  TEST(foo_vector_memory_actual.second == foo_vector_memory_bytes_expected);
}

static void
TestHeapConsumption(HeapObserverSummary * heap_observer_summary,
                    Dispatcher * dispatcher, int64_t const n) {
  dispatcher->StartTrackingAll();

  auto const foo_memory_actual =
    GetHeapConsumption(heap_observer_summary, Foo{});
  TEST(foo_memory_actual.first == 0);
  TEST(foo_memory_actual.second == 0);

  TestHeapConsumptionList(heap_observer_summary, n);
  TestHeapConsumptionForwardList(heap_observer_summary, n);
  TestHeapConsumptionVector(heap_observer_summary, n);

  dispatcher->StopTrackingAll();
}

int
main() {
  std::unique_ptr<AbstractObserver> heap_observer{
    new HeapObserverSummary{MyHeapTrackSummaryOptions()}};

  Dispatcher dispatcher{heap_observer.get()};
  SetInterceptorDispatcher(&dispatcher);

  HeapObserverSummary * heap_observer_summary =
    dynamic_cast<HeapObserverSummary *>(heap_observer.get());
  assert(heap_observer_summary);

  TestHeapConsumption(heap_observer_summary, &dispatcher, 0);
  TestHeapConsumption(heap_observer_summary, &dispatcher, 1);
  TestHeapConsumption(heap_observer_summary, &dispatcher, 2);
  TestHeapConsumption(heap_observer_summary, &dispatcher, 3);
  TestHeapConsumption(heap_observer_summary, &dispatcher, 10);
}

#endif
