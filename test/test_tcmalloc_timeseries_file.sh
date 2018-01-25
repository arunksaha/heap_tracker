#!/bin/bash

SummaryOutputFilename="heap_tracker_observer_timeseries_file.output.txt"
ExpectedOutputFilename="../test/heap_tracker_observer_timeseries_file.output.decoded.expected.txt"
TestExe="test/heap_tracker_test_tcmalloc_timeseries_file"
DecoderExe="src/heap_tracker_observer_timeseries_file_decoder_tool"

rm ${SummaryOutputFilename} -f
[ -f ${TestExe} ] || exit 1
[ -f ${DecoderExe} ] || exit 1
${TestExe}
[ -f ${SummaryOutputFilename} ] || exit 1

decoder_output_filtered_actual=$(${DecoderExe} ${SummaryOutputFilename} | grep '^bytes =' | cut --delimiter ' ' --fields 1-6)
decoder_output_filtered_expected=$(cat ${ExpectedOutputFilename}        | grep '^bytes =' | cut --delimiter ' ' --fields 1-6)
if [ "${decoder_output_filtered_actual}" != "${decoder_output_filtered_expected}" ]; then
  printf "actual = ${decoder_output_filtered_actual}\n"
  printf "expected = ${decoder_output_filtered_expected}\n"
  exit 1
fi

exit 0
