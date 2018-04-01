#!/bin/bash

SummaryOutputFilename="heap_tracker_observer_summary.output.txt"
ExpectedOutputFilename="../test/heap_tracker_observer_summary.output.expected.txt"
TestExe="test/heap_tracker_test_tcmalloc_summary"

rm ${SummaryOutputFilename} -f
[ -f ${TestSo} ] || exit 1
[ -f ${TestExe} ] || exit 1
set -x
${TestExe}
set +x
[ -f ${SummaryOutputFilename} ] || exit 1

# In clang builds, the numbers are not exactly matching, so checking only few stats. REVISIT.
expected_output_content=$(grep --text 'outstanding_alloc_count:\|outstanding_alloc_bytes:' ${ExpectedOutputFilename} | grep -v peak)
actual_output_content=$(grep   --text 'outstanding_alloc_count:\|outstanding_alloc_bytes:' ${SummaryOutputFilename}  | grep -v peak)
if [ "${expected_output_content}" != "${actual_output_content}" ]; then
  printf "$0 expected:\n"
  cat ${ExpectedOutputFilename}
  printf "$0 actual:\n"
  cat ${SummaryOutputFilename}
  diff  <(echo "${expected_output_content}") <(echo "${actual_output_content}")
  exit 1
fi

exit 0
