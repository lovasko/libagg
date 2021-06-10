#!/bin/bash
# Copyright (c) 2019-2021 Daniel Lovasko
# All Rights Reserved
#
# Distributed under the terms of the 2-clause BSD License. The full
# license is in the file LICENSE, distributed as part of this software.

set -x

# Ensure that all relative paths remain correct.
cd $(dirname $0)

# C compilation settings.
CC="cc"
OPT="-flto -march=native -mtune=native"
CFLAGS="-I../src -D_POSIX_C_SOURCE=201912 -std=c99 -Wall -Wextra -Werror"
LDFLAGS="-lm"
SRCS="./cap.c ../src/get.c ../src/put.c ../src/new.c ../src/run.c"
ARGS="${CFLAGS} ${OPT} ${SRCS} ${LDFLAGS}"

REPEAT=1000
SCALE=100.0
OFFSET=0.0

# Execute the testing for a selected aggregate function.
# param 1: floating-point bit width
# param 2: integer bit width
# param 3: aggregate function name (fst, lst, ...)
# param 4: aggregate function argument
run_for_func() {
  echo -n "$2 "
  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l10
  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l100
  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l1000
  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l10000
  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l100000
  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l1000000
  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l10000000
#  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l100000000
#  ./bin/cap_o0_f$1_i$2 -f$3 -p$4 -s${SCALE} -o${OFFSET} -r${REPEAT} -l1000000000
  echo
}

# Execute all error detections for a particular bit width configuration.
# param 1: floating-point bit width
# param 2: integer bit width
run_for_bits() {
  run_for_func $1 $2 fst 0.0  > ./res/f$1_i$2_fst     &
  run_for_func $1 $2 lst 0.0  > ./res/f$1_i$2_lst     &
  wait

  run_for_func $1 $2 cnt 0.0  > ./res/f$1_i$2_cnt     &
  run_for_func $1 $2 sum 0.0  > ./res/f$1_i$2_sum     &
  wait

  run_for_func $1 $2 min 0.0  > ./res/f$1_i$2_min     &
  run_for_func $1 $2 max 0.0  > ./res/f$1_i$2_max     &
  wait

  run_for_func $1 $2 avg 0.0  > ./res/f$1_i$2_avg     &
  run_for_func $1 $2 var 0.0  > ./res/f$1_i$2_var     &
  wait

  run_for_func $1 $2 skw 0.0  > ./res/f$1_i$2_skw     &
  run_for_func $1 $2 krt 0.0  > ./res/f$1_i$2_krt     &
  wait

  run_for_func $1 $2 qnt 0.1  > ./res/f$1_i$2_qnt_01  &
  run_for_func $1 $2 qnt 0.75 > ./res/f$1_i$2_qnt_075 &
  wait

  run_for_func $1 $2 qnt 0.9  > ./res/f$1_i$2_qnt_09  &
  run_for_func $1 $2 qnt 0.99 > ./res/f$1_i$2_qnt_099 &
  wait

  run_for_func $1 $2 med 0.0  > ./res/f$1_i$2_med     &
  run_for_func $1 $2 dev 0.0  > ./res/f$1_i$2_dev     &
  wait
}

${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=16  -o ./bin/cap_o0_f32_i16   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=32  -o ./bin/cap_o0_f32_i32   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=64  -o ./bin/cap_o0_f32_i64   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=32  -DAGGSTAT_INT_BIT=128 -o ./bin/cap_o0_f32_i128  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=16  -o ./bin/cap_o0_f64_i16   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=32  -o ./bin/cap_o0_f64_i32   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=64  -o ./bin/cap_o0_f64_i64   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=64  -DAGGSTAT_INT_BIT=128 -o ./bin/cap_o0_f64_i128  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=16  -o ./bin/cap_o0_f80_i16   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=32  -o ./bin/cap_o0_f80_i32   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=64  -o ./bin/cap_o0_f80_i64   -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=80  -DAGGSTAT_INT_BIT=128 -o ./bin/cap_o0_f80_i128  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=16  -o ./bin/cap_o0_f128_i16  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=32  -o ./bin/cap_o0_f128_i32  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=64  -o ./bin/cap_o0_f128_i64  -O0 ${ARGS}
${CC} -DAGGSTAT_FLT_BIT=128 -DAGGSTAT_INT_BIT=128 -o ./bin/cap_o0_f128_i128 -O0 ${ARGS}

run_for_bits 32  16
run_for_bits 32  32
run_for_bits 32  64
run_for_bits 32  128
run_for_bits 64  16
run_for_bits 64  32
run_for_bits 64  64
run_for_bits 64  128
run_for_bits 80  16
run_for_bits 80  32
run_for_bits 80  64
run_for_bits 80  128
run_for_bits 128 16
run_for_bits 128 32
run_for_bits 128 64
run_for_bits 128 128

wait
