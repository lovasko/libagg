#!/bin/bash
# Copyright (c) 2019-2020 Daniel Lovasko
# All Rights Reserved
#
# Distributed under the terms of the 2-clause BSD License. The full
# license is in the file LICENSE, distributed as part of this software.

set -e

# Ensure that all relative paths remain correct.
cd $(dirname $0)

# C compilation settings.
CC="cc"
CFLAGS="-I../src -D_POSIX_C_SOURCE=201912 -std=c99 -Wall -Wextra -Werror -flto"
LDFLAGS="-lm"
SRC="cap.c ../src/get.c ../src/put.c ../src/new.c ../src/run.c"

REPEAT=1000
SCALE=100.0
OFFSET=0.0

run_for_func() {
  echo -n "$2 "
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l10
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l100
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l1000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l10000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l100000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l1000000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l10000000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l100000000
  ./cap_$1 -f$2 -p$3 -s${SCALE} -o${OFFSET} -r1000 -l1000000000
  echo
}

run_for_bits() {
  run_for_func $1 fst 0.0  > "$1_fst" &
  run_for_func $1 lst 0.0  > "$1_lst" &
  run_for_func $1 cnt 0.0  > "$1_cnt" &
  run_for_func $1 sum 0.0  > "$1_sum" &
  run_for_func $1 min 0.0  > "$1_min" &
  run_for_func $1 max 0.0  > "$1_max" &
  run_for_func $1 avg 0.0  > "$1_avg" &
  run_for_func $1 var 0.0  > "$1_var" &
  run_for_func $1 dev 0.0  > "$1_dev" &
  run_for_func $1 skw 0.0  > "$1_skw" &
  run_for_func $1 krt 0.0  > "$1_krt" &
  run_for_func $1 qnt 0.1  > "$1_qnt_01" &
  run_for_func $1 qnt 0.75 > "$1_qnt_075" &
  run_for_func $1 qnt 0.9  > "$1_qnt_09" &
  run_for_func $1 qnt 0.99 > "$1_qnt_099" &
  run_for_func $1 med 0.0  > "$1_med" &
}

${CC} -DAGG_BIT=32 -o "cap_32" ${CFLAGS} ${SRC} ${LDFLAGS}
run_for_bits 32

${CC} -DAGG_BIT=64 -o "cap_64" ${CFLAGS} ${SRC} ${LDFLAGS}
run_for_bits 64

${CC} -DAGG_BIT=80 -o "cap_80" ${CFLAGS} ${SRC} ${LDFLAGS}
run_for_bits 80

wait
