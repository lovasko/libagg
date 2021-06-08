#!/bin/bash
# Copyright (c) 2019-2021 Daniel Lovasko
# All Rights Reserved
#
# Distributed under the terms of the 2-clause BSD License. The full
# license is in the file LICENSE, distributed as part of this software.

# Ensure that all relative paths remain correct.
cd $(dirname $0)

# C compilation settings.
CC="cc"
OPT="-flto -march=native -mtune=native"
CFLAGS="-I../src -D_POSIX_C_SOURCE=201912 -std=c99 -Wall -Wextra -Werror $OPT"
LDFLAGS="-lm"
SRC="cap.c ../src/get.c ../src/put.c ../src/new.c ../src/run.c"

REPEAT=1000
SCALE=100.0
OFFSET=0.0

# Execute the testing for a selected aggregate function.
# param 1: floating-point bit width
# param 2: aggregate function name (fst, lst, ...)
# param 3: aggregate function argument
run_for_func() {
  echo -n "$2 "
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l10
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l100
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l1000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l10000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l100000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l1000000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l10000000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l100000000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r${REPEAT} -l1000000000
  echo
}

# Execute all error detections for a particular bit width configuration.
# param 1: floating-point bit width
run_for_bits() {
  run_for_func $1 fst 0.0  > "$1_fst" &
  run_for_func $1 lst 0.0  > "$1_lst" &
  wait

  run_for_func $1 cnt 0.0  > "$1_cnt" &
  run_for_func $1 sum 0.0  > "$1_sum" &
  wait

  run_for_func $1 min 0.0  > "$1_min" &
  run_for_func $1 max 0.0  > "$1_max" &
  wait

  run_for_func $1 avg 0.0  > "$1_avg" &
  run_for_func $1 var 0.0  > "$1_var" &
  wait

  run_for_func $1 skw 0.0  > "$1_skw" &
  run_for_func $1 krt 0.0  > "$1_krt" &
  wait

  run_for_func $1 qnt 0.1  > "$1_qnt_01" &
  run_for_func $1 qnt 0.75 > "$1_qnt_075" &
  wait

  run_for_func $1 qnt 0.9  > "$1_qnt_09" &
  run_for_func $1 qnt 0.99 > "$1_qnt_099" &
  wait

  run_for_func $1 med 0.0  > "$1_med" &
  run_for_func $1 dev 0.0  > "$1_dev" &
  wait
}

${CC} -DAGG_BIT=32 -o "cap_32" ${CFLAGS} ${SRC} ${LDFLAGS}
run_for_bits 32

${CC} -DAGG_BIT=64 -o "cap_64" ${CFLAGS} ${SRC} ${LDFLAGS}
run_for_bits 64

${CC} -DAGG_BIT=80 -o "cap_80" ${CFLAGS} ${SRC} ${LDFLAGS}
run_for_bits 80

wait
