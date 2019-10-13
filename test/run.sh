#!/bin/bash
# Copyright (c) 2019 Daniel Lovasko
# All Rights Reserved
#
# Distributed under the terms of the 2-clause BSD License. The full
# license is in the file LICENSE, distributed as part of this software.

# Ensure that all relative paths remain correct.
cd $(dirname $0)

# C compiler.
CC=c99

# The first part of the testing is to execute the test assuming full IEEE
# floating-point operations compliance. If any of the following steps were to
# fail, we assume that the test suite has not passed.
set -e
${CC} -DAGG_DBL -o dbl_test -std=c99 test.c ../impl/agg.c -lm
./dbl_test

${CC} -DAGG_FLT -o flt_test -std=c99 test.c ../impl/agg.c -lm
./flt_test
set +e

# The second part is mostly informational as to whether increased optimizations
# and the fast math mode that disables full IEEE compliance, errno-setting, and
# assumes all math is finite, still produces valid results within the expected
# margin of error. To ensure that the test suite passes nonetheless, we exit
# with successful execution.
${CC} -DAGG_DBL -o dbl_test_fast -std=c99 -Ofast test.c ../impl/agg.c -lm
./dbl_test_fast

${CC} -DAGG_FLT -o flt_test_fast -std=c99 -Ofast test.c ../impl/agg.c -lm 
./flt_test_fast
true
