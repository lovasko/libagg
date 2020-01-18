#!/bin/bash
# Copyright (c) 2019-2020 Daniel Lovasko
# All Rights Reserved
#
# Distributed under the terms of the 2-clause BSD License. The full
# license is in the file LICENSE, distributed as part of this software.

# Ensure that all relative paths remain correct.
cd $(dirname $0)

# C compilation settings.
CC="cc"
CFLAGS="-D_POSIX_C_SOURCE=201912 -std=c99 -Wall -Wextra -Werror -flto"
LDFLAGS="-lm"
SRC="err.c ../src/get.c ../src/put.c ../src/new.c ../src/run.c"

# The first part of the testing is to execute the test assuming full IEEE
# floating-point operations compliance. If any of the following steps were to
# fail, we assume that the test suite has not passed.
set -e
echo "#################"
echo "# DOUBLE        #"
echo "#################"
${CC} -DAGG_BIT=64 -o "err_dbl" ${CFLAGS} ${SRC} ${LDFLAGS}
./err_dbl

echo "#################"
echo "# FLOAT         #"
echo "#################"
${CC} -DAGG_BIT=32 -o "err_flt" ${CFLAGS} ${SRC} ${LDFLAGS}
./err_flt
set +e

# The second part is mostly informational as to whether increased optimizations
# and the fast math mode that disables full IEEE compliance, errno-setting, and
# assumes all math is finite, still produces valid results within the expected
# margin of error.
echo "#################"
echo "# DOUBLE FAST   #"
echo "#################"
${CC} -DAGG_BIT=64 -o "err_dbl_fast" -Ofast ${CFLAGS} ${SRC} ${LDFLAGS}
./err_dbl_fast

echo "#################"
echo "# FLOAT FAST    #"
echo "#################"
${CC} -DAGG_BIT=32 -o "err_flt_fast" -Ofast ${CFLAGS} ${SRC} ${LDFLAGS}
./err_flt_fast

# Ensure successful exit code of the testing, even if the fast tests fail.
true
