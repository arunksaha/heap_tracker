#!/bin/bash
# Run this in 'build' directory as:
#   build> bash ../run_all_tests_for_all_interceptions.sh

TestDir="../test"

set -x
pwd
rm ./* -rf ; ../cmake_interposition.sh
bash ../test/run_all_tests_for_specific_interception.sh interposition
rm ./* -rf ; ../cmake_tcmalloc.sh
bash ../test/run_all_tests_for_specific_interception.sh tcmalloc
