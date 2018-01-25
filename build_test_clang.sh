#!/bin/bash
# Run this in 'build' directory as:
#   build> bash ../build_test_clang.sh

set -x
rm ./* -rf; cmake -G "Unix Makefiles" -DBUILD_INTERPOSITION=ON -DBUILD_CLANG=ON .. && make $@
bash ../test/run_all_tests_for_specific_interception.sh interposition
rm ./* -rf; cmake -G "Unix Makefiles" -DBUILD_TCMALLOC=ON -DBUILD_CLANG=ON .. && make $@
bash ../test/run_all_tests_for_specific_interception.sh tcmalloc
