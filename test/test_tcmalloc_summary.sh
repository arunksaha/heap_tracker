#!/bin/bash

SummaryOutputFilename="heap_tracker_observer_summary.output.txt"
ExpectedOutputFilename="../test/heap_tracker_observer_summary.output.expected.txt"
TestExe="test/heap_tracker_test_tcmalloc_summary"

rm ${SummaryOutputFilename} -f
[ -f ${TestSo} ] || exit 1
[ -f ${TestExe} ] || exit 1
${TestExe}
[ -f ${SummaryOutputFilename} ] || exit 1

expected_output_sans_instant=$(grep --invert-match instant --text ${ExpectedOutputFilename})
actual_output_sans_instant=$(grep --invert-match instant --text ${SummaryOutputFilename})
[ "${expected_output_sans_instant}" == "${actual_output_sans_instant}" ] || exit 1

exit 0
