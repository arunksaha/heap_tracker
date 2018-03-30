#!/bin/bash
# Run this in 'build' directory as:
#   build> bash ../run_all_tests_for_all_interceptions.sh

TestDir="../test"

set -x
pwd

rm ./* -rf ; ../cmake_interposition.sh
bash ../test/run_all_tests_for_specific_interception.sh interposition
rcInterposition=$?

rm ./* -rf ; ../cmake_tcmalloc.sh
bash ../test/run_all_tests_for_specific_interception.sh tcmalloc
rcTcmalloc=$?

rcFinal=0
if [[ "${rcInterposition}" == 1 || "${rcTcmalloc}" == 1 ]]; then
  rcFinal=1
fi

printf "../test/run_all_tests_for_specific_interception.sh interposition rc = ${rcInterposition}\n"
printf "../test/run_all_tests_for_specific_interception.sh tcmalloc rc = ${rcTcmalloc}\n"
printf "rcFinal = ${rcFinal}\n"

exit ${rcFinal}
