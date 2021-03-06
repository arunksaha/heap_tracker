#!/bin/bash

TestDir="../test"

TestInterpositionFiles="test_interposition_summary.sh \
  test_interposition_timeseries_file.sh"

TestTcmallocFiles="test_tcmalloc_summary.sh \
  test_tcmalloc_timeseries_file.sh"

PrintUsage() {
  printf "usage: $0 <interposition|tcmalloc>\n"
}

InterceptOption=$1

if [ "${InterceptOption}" == "interposition" ]; then
  TestFiles=${TestInterpositionFiles}
elif [ "${InterceptOption}" == "tcmalloc" ]; then
  TestFiles=${TestTcmallocFiles}
else 
  PrintUsage
  exit 1
fi

exit_status=0

for f in ${TestFiles}; do
  printf "\n$0: Running ${f}...\n"
  bash ${TestDir}/${f}
  rc=$?
  printf "${f} rc = ${rc}"
  if [ "${rc}" == "0" ]; then
    printf "    PASS\n"
  else
    printf "    FAIL\n"
    exit_status=1
  fi
done

run_binary() {
  Binary=$1
  printf "\n$0: Running ${Binary}...\n"
  export EXE=${Binary}
  ${Binary}
  rc=$?
  printf "${Binary} rc = ${rc}"
  if [ "${rc}" == "0" ]; then
    printf "    PASS\n"
  else
    printf "    FAIL\n"
    ls -ltr
    exit_status=1
  fi
}

if [ "${InterceptOption}" == "tcmalloc" ]; then
  run_binary test/heap_tracker_test2_observer_summary
  run_binary test/heap_tracker_test2_timeseries_file
  run_binary test/heap_tracker_test3_size
fi

exit ${exit_status}
