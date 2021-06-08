#!/bin/bash
# Copyright (c) 2019-2021 Daniel Lovasko
# All Rights Reserved
#
# Distributed under the terms of the 2-clause BSD License. The full
# license is in the file LICENSE, distributed as part of this software.

# TODO
#  - accept a single optional argument to specify the size of the test suite:
#    "std" executes the standard set of tests only
#    "full" executes all tests

# Ensure that all relative paths remain correct.
cd $(dirname $0)

# C compilation settings.
CC="cc"
OPT="-flto -march=native -mtune=native"
CFLAGS="-D_POSIX_C_SOURCE=201912 -std=c99 -Wall -Wextra -Werror"
LDFLAGS="-lm"
SRCS="err.c ../src/get.c ../src/put.c ../src/new.c ../src/run.c"
ARGS="${CFLAGS} ${OPT} ${SRCS} ${LDFLAGS}"

# Ensure all program invocations are logged.
set -x

# The first part of the testing is to execute the test assuming full IEEE
# floating-point operations compliance. If any of the following steps were to
# fail, we assume that the test suite has not passed. As the types used in the
# testing are all included in the C standard, these tests ought to pass on all
# architectures and platforms. Furthermore, the compiler optimisations are
# turned off to prevent any accidental corruption.
set -e

${CC} -DAGGSTAT_FLT_BIT=32 -DAGGSTAT_INT_BIT=16 -o bin/err_o0_f32_i16 -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32 -DAGGSTAT_INT_BIT=32 -o bin/err_o0_f32_i32 -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32 -DAGGSTAT_INT_BIT=64 -o bin/err_o0_f32_i64 -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64 -DAGGSTAT_INT_BIT=16 -o bin/err_o0_f64_i16 -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64 -DAGGSTAT_INT_BIT=32 -o bin/err_o0_f64_i32 -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64 -DAGGSTAT_INT_BIT=64 -o bin/err_o0_f64_i64 -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80 -DAGGSTAT_INT_BIT=16 -o bin/err_o0_f80_i16 -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80 -DAGGSTAT_INT_BIT=32 -o bin/err_o0_f80_i32 -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80 -DAGGSTAT_INT_BIT=64 -o bin/err_o0_f80_i64 -O0 ${ARGS}

./bin/err_o0_f32_i16
./bin/err_o0_f32_i32
./bin/err_o0_f32_i64
./bin/err_o0_f64_i16
./bin/err_o0_f64_i32
./bin/err_o0_f64_i64
./bin/err_o0_f80_i16
./bin/err_o0_f80_i32
./bin/err_o0_f80_i64

# The second part is mostly informational as to whether increased optimizations
# and the fast math mode that disables full IEEE compliance, errno-setting, and
# assumes all math is finite, still produces valid results within the expected
# margin of error.
set +e

${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=16  -o bin/err_o0_f128_i16  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=32  -o bin/err_o0_f128_i32  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=64  -o bin/err_o0_f128_i64  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=128 -o bin/err_o0_f32_i128  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=128 -o bin/err_o0_f64_i128  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=128 -o bin/err_o0_f80_i128  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=128 -o bin/err_o0_f128_i128 -O0 ${ARGS}

./bin/err_o0_f128_i16
./bin/err_o0_f128_i32
./bin/err_o0_f128_i64
./bin/err_o0_f32_i128
./bin/err_o0_f64_i128
./bin/err_o0_f80_i128
./bin/err_o0_f128_i128

# Third part is a concatenation of two previous parts, alas with all
# executables being built with optimisations turned on. These need not pass, as
# the fast math and other optimisations could corrupt the correctness
# guarantees.
${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=16  -o bin/err_o3_f32_i16   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=32  -o bin/err_o3_f32_i32   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=64  -o bin/err_o3_f32_i64   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=128 -o bin/err_o3_f32_i128  -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=16  -o bin/err_o3_f64_i16   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=32  -o bin/err_o3_f64_i32   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=64  -o bin/err_o3_f64_i64   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=128 -o bin/err_o3_f64_i128  -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=16  -o bin/err_o3_f80_i16   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=32  -o bin/err_o3_f80_i32   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=64  -o bin/err_o3_f80_i64   -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=128 -o bin/err_o3_f80_i128  -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=16  -o bin/err_o3_f128_i16  -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=32  -o bin/err_o3_f128_i32  -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=64  -o bin/err_o3_f128_i64  -O3 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=128 -o bin/err_o3_f128_i128 -O3 ${ARGS}

./bin/err_o3_f32_i16
./bin/err_o3_f32_i32
./bin/err_o3_f32_i64
./bin/err_o3_f32_i128
./bin/err_o3_f64_i16
./bin/err_o3_f64_i32
./bin/err_o3_f64_i64
./bin/err_o3_f64_i128
./bin/err_o3_f80_i16
./bin/err_o3_f80_i32
./bin/err_o3_f80_i64
./bin/err_o3_f80_i128
./bin/err_o3_f128_i16
./bin/err_o3_f128_i32
./bin/err_o3_f128_i64
./bin/err_o3_f128_i128

# Ensure successful exit code of the testing, even if the fast tests fail.
true
