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
OPT="-flto -march=native -mtune=native"
CFLAGS="-D_POSIX_C_SOURCE=201912 -std=c99 -Wall -Wextra -Werror $OPT"
LDFLAGS="-lm"
SRC="err.c ../src/get.c ../src/put.c ../src/new.c ../src/run.c"

# The first part of the testing is to execute the test assuming full IEEE
# floating-point operations compliance. If any of the following steps were to
# fail, we assume that the test suite has not passed.
set -e

echo "##############"
echo "# AGG_BIT 32 #"
echo "##############"
${CC} -DAGG_BIT=32 -o err_32 ${CFLAGS} ${SRC} ${LDFLAGS}
./err_32

echo "##############"
echo "# AGG_BIT 64 #"
echo "##############"
${CC} -DAGG_BIT=64 -o err_64 ${CFLAGS} ${SRC} ${LDFLAGS}
./err_64

echo "##############"
echo "# AGG_BIT 80 #"
echo "##############"
${CC} -DAGG_BIT=80 -o err_80 ${CFLAGS} ${SRC} ${LDFLAGS}
./err_80

# The second part is mostly informational as to whether increased optimizations
# and the fast math mode that disables full IEEE compliance, errno-setting, and
# assumes all math is finite, still produces valid results within the expected
# margin of error.
set +e

echo "#####################"
echo "# AGG_BIT 32 (FAST) #"
echo "#####################"
${CC} -DAGG_BIT=32 -o err_32_fast -Ofast ${CFLAGS} ${SRC} ${LDFLAGS}
./err_32_fast

echo "#####################"
echo "# AGG_BIT 64 (FAST) #"
echo "#####################"
${CC} -DAGG_BIT=64 -o err_64_fast -Ofast ${CFLAGS} ${SRC} ${LDFLAGS}
./err_64_fast

echo "#####################"
echo "# AGG_BIT 80 (FAST) #"
echo "#####################"
${CC} -DAGG_BIT=80 -o err_80_fast -Ofast ${CFLAGS} ${SRC} ${LDFLAGS}
./err_80_fast

# Ensure successful exit code of the testing, even if the fast tests fail.
true
