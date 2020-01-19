// Copyright (c) 2019-2020 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#ifndef AGGREGATE_TEST_ERR_H
#define AGGREGATE_TEST_ERR_H

// Define error threshold constants.
#define P_03 AGG_NUM(1, 0, +,  3)
#define P_02 AGG_NUM(1, 0, +,  2)
#define P_01 AGG_NUM(1, 0, +,  1)
#define Z_01 AGG_NUM(1, 0, +,  0)
#define Z_00 AGG_NUM(0, 0, +,  0)
#define M_01 AGG_NUM(1, 0, -,  1)
#define M_02 AGG_NUM(1, 0, -,  2)
#define M_03 AGG_NUM(1, 0, -,  3)
#define M_04 AGG_NUM(1, 0, -,  4)
#define M_05 AGG_NUM(1, 0, -,  5)
#define M_06 AGG_NUM(1, 0, -,  6)
#define M_07 AGG_NUM(1, 0, -,  7)
#define M_08 AGG_NUM(1, 0, -,  8)
#define M_09 AGG_NUM(1, 0, -,  9)
#define M_10 AGG_NUM(1, 0, -, 10)
#define M_12 AGG_NUM(1, 0, -, 11)
#define M_13 AGG_NUM(1, 0, -, 12)
#define M_14 AGG_NUM(1, 0, -, 13)
#define M_15 AGG_NUM(1, 0, -, 14)
#define M_16 AGG_NUM(1, 0, -, 15)

// The tables below denote the acceptable magnitudes of error for each
// aggregate function. The corresponding length of the input list is
// implied, starting with 10 elements and increasing tenfold in each
// iteration.
#if AGG_BIT == 32
  #ifdef __FAST_MATH__
    static const AGG_TYPE err[13][6] = {
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // fst
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // snd
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // cnt
      {M_05, M_03, M_02, Z_01, P_01, P_03}, // sum
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // min
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // max
      {M_06, M_05, M_04, M_04, M_04, M_03}, // avg
      {M_05, M_05, M_04, M_04, M_03, M_02}, // var
      {M_06, M_05, E_04, M_04, M_04, M_03}, // dev
      {Z_01, M_02, M_03, M_04, M_04, M_03}, // skw
      {Z_01, M_01, M_02, M_03, M_03, M_02}, // krt
      {P_01, Z_01, Z_01, M_01, M_01, M_01}, // qnt
      {P_01, Z_01, Z_01, M_01, M_01, M_01}, // med
    };
  #else
    static const AGG_TYPE err[13][6] = {
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // fst
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // lst
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // cnt
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // sum
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // min
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // max
      {M_06, M_05, M_04, M_04, M_04, M_03}, // avg
      {M_05, M_05, M_04, M_04, M_03, M_03}, // var
      {M_06, M_05, M_04, M_04, M_04, M_04}, // dev
      {Z_01, M_02, M_03, M_04, M_04, M_03}, // skw
      {Z_01, M_01, M_02, M_03, M_03, M_03}, // krt
      {P_01, Z_01, Z_01, M_01, M_01, M_01}, // qnt
      {P_01, Z_01, Z_01, M_01, M_01, M_01}, // med
    };
  #endif
#elif AGG_BIT == 64
  #ifdef __FAST_MATH__
    static const AGG_TYPE err[13][6] = {
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // fst
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // lst
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // cnt
      {M_13, M_12, M_10, M_09, M_07, M_06}, // sum
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // min
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // max
      {M_14, M_14, M_13, M_13, M_12, M_12}, // avg
      {M_14, M_14, M_13, M_13, M_12, M_12}, // var
      {M_14, M_14, M_13, M_13, M_12, M_12}, // dev
      {Z_01, M_02, M_03, M_05, M_06, M_08}, // skw
      {Z_01, M_01, M_02, M_03, M_04, M_05}, // krt
      {P_01, Z_01, Z_01, M_01, M_02, M_03}, // qnt
      {P_01, Z_01, Z_01, M_01, M_02, M_03}, // med
    };
  #else
    static const AGG_TYPE err[13][6] = {
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // fst
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // lst
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // cnt
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // sum
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // min
      {Z_00, Z_00, Z_00, Z_00, Z_00, Z_00}, // max
      {M_14, M_14, M_13, M_13, M_12, M_12}, // avg
      {M_14, M_14, M_13, M_13, M_12, M_12}, // var
      {M_14, M_14, M_13, M_13, M_12, M_12}, // dev
      {Z_01, M_02, M_03, M_05, M_06, M_08}, // skw
      {P_01, M_01, M_02, M_03, M_04, M_05}, // krt
      {P_01, Z_01, Z_01, M_01, M_02, M_03}, // qnt
      {P_01, Z_01, Z_01, M_01, M_02, M_03}, // med
    };
  #endif
#else
  #error "AGG_BIT is not supported" AGG_BIT
#endif

#endif
