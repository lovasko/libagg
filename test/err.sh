#!/bin/bash
# Copyright (c) 2019 Daniel Lovasko
# All Rights Reserved
#
# Distributed under the terms of the 2-clause BSD License. The full
# license is in the file LICENSE, distributed as part of this software.

# Ensure that all relative paths remain correct.
cd $(dirname $0)

# C compiler.
CC=cc

# The first part of the testing is to execute the test assuming full IEEE
# floating-point operations compliance. If any of the following steps were to
# fail, we assume that the test suite has not passed.
set -e
echo "#################"
echo "# DOUBLE        #"
echo "#################"
${CC} -DAGG_DBL -o err_dbl -std=c99 -flto err.c ../src/*.c -lm
./err_dbl

echo "#################"
echo "# FLOAT         #"
echo "#################"
${CC} -DAGG_FLT -o err_flt -std=c99 -flto err.c ../src/*.c -lm
./err_flt
set +e

# The second part is mostly informational as to whether increased optimizations
# and the fast math mode that disables full IEEE compliance, errno-setting, and
# assumes all math is finite, still produces valid results within the expected
# margin of error.
echo "#################"
echo "# DOUBLE FAST   #"
echo "#################"
${CC} -DAGG_DBL -o err_dbl_fast -std=c99 -Ofast -flto err.c ../src/*.c -lm
./err_dbl_fast

echo "#################"
echo "# FLOAT FAST    #"
echo "#################"
${CC} -DAGG_FLT -o err_flt_fast -std=c99 -Ofast -flto err.c ../src/*.c -lm
./err_flt_fast

# Ensure successful exit code of the testing, even if the fast tests fail.
true
