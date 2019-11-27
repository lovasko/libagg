// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#ifndef AGGREGATE_TEST_ERR_H
#define AGGREGATE_TEST_ERR_H

// The tables below denote the acceptable magnitudes of error for each
// aggregate function. The corresponding length of the input list is
// implied, starting with 10 elements and increasing tenfold in each
// iteration.
#ifdef AGG_FLT
  #ifdef __FAST_MATH__
    static const float err[12][6] = {
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // fst
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // lst
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // cnt
      {1.0e-5f, 1.0e-3f, 1.0e-2f, 1.0e0f,  1.0e1f,  1.0e3f},  // sum
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // min 
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // max
      {1.0e-6f, 1.0e-5f, 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-3f}, // avg
      {1.0e-5f, 1.0e-5f, 1.0e-4f, 1.0e-4f, 1.0e-3f, 1.0e-2f}, // var
      {1.0e-6f, 1.0e-5f, 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-3f}, // dev
      {1.0e0f,  1.0e-2f, 1.0e-3f, 1.0e-4f, 1.0e-4f, 1.0e-3f}, // skw
      {1.0e0f,  1.0e-1f, 1.0e-2f, 1.0e-3f, 1.0e-3f, 1.0e-2f}, // krt
      {1.0e1f,  1.0e0f,  1.0e0f,  1.0e-1f, 1.0e-1f, 1.0e-1f}, // qnt
    };
  #else
    static const float err[12][6] = {
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // fst
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // lst
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // cnt
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // sum 
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // min 
      {0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f,  0.0e0f},  // max
      {1.0e-6f, 1.0e-5f, 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-3f}, // avg
      {1.0e-5f, 1.0e-5f, 1.0e-4f, 1.0e-4f, 1.0e-3f, 1.0e-3f}, // var
      {1.0e-6f, 1.0e-5f, 1.0e-4f, 1.0e-4f, 1.0e-4f, 1.0e-4f}, // dev
      {1.0e0f,  1.0e-2f, 1.0e-3f, 1.0e-4f, 1.0e-4f, 1.0e-3f}, // skw
      {1.0e0f,  1.0e-1f, 1.0e-2f, 1.0e-3f, 1.0e-3f, 1.0e-3f}, // krt
      {1.0e1f,  1.0e0f,  1.0e0f,  1.0e-1f, 1.0e-1f, 1.0e-1f}, // qnt
    };
  #endif
#else
  #ifdef __FAST_MATH__
    static const double err[12][6] = {
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // fst
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // lst
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // cnt
      {1.0e-13, 1.0e-12, 1.0e-10, 1.0e-9,  1.0e-7,  1.0e-6},  // sum
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // min
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // max
      {1.0e-14, 1.0e-14, 1.0e-13, 1.0e-13, 1.0e-12, 1.0e-12}, // avg
      {1.0e-14, 1.0e-14, 1.0e-13, 1.0e-13, 1.0e-12, 1.0e-12}, // var
      {1.0e-14, 1.0e-14, 1.0e-13, 1.0e-13, 1.0e-12, 1.0e-12}, // dev
      {1.0e0,   1.0e-2,  1.0e-3,  1.0e-5,  1.0e-6,  1.0e-8},  // skw
      {1.0e0,   1.0e-1,  1.0e-2,  1.0e-3,  1.0e-4,  1.0e-5},  // krt
      {1.0e1,   1.0e0,   1.0e0,   1.0e-1,  1.0e-2,  1.0e-3},  // qnt
    };
  #else
    static const double err[12][6] = {
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // fst
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // lst
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // cnt
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // sum
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // min
      {0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0,   0.0e0},   // max
      {1.0e-14, 1.0e-14, 1.0e-13, 1.0e-13, 1.0e-12, 1.0e-12}, // avg
      {1.0e-14, 1.0e-14, 1.0e-13, 1.0e-13, 1.0e-12, 1.0e-12}, // var
      {1.0e-14, 1.0e-14, 1.0e-13, 1.0e-13, 1.0e-12, 1.0e-12}, // dev
      {1.0e0,   1.0e-2,  1.0e-3,  1.0e-5,  1.0e-6,  1.0e-8},  // skw
      {1.0e1,   1.0e-1,  1.0e-2,  1.0e-3,  1.0e-4,  1.0e-5},  // krt
      {1.0e1,   1.0e0,   1.0e0,   1.0e-1,  1.0e-2,  1.0e-3},  // qnt
    };
  #endif
#endif

#endif
