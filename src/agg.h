// Copyright (c) 2019-2020 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#ifndef AGG_H
#define AGG_H

#include <stdbool.h>
#include <stdint.h>
#include <float.h>

// This constant ensures that all code that is made available by this library is strictly compliant
// with the C99 language standard.
#ifndef AGG_STD
  #define AGG_STD 1
#endif

// This constant selects the width of the floating point type used by the library in all
// computations. The default value is 64, which denotes the `double` type.  Other permissible values
// include 32 for `float` and 128 for `__float128`. The latter type is a non-standard extension and
// will result in compile-time error unless the `AGG_STD` macro evaluates to `0`.
#ifndef AGG_BIT
  #define AGG_BIT 64
#endif

// Determine the appropriate type-related constants and functions.
#if AGG_BIT == 32
  // Types.
  #define AGG_TYPE float
  #define AGG_SIZE sizeof(AGG_TYPE)

  // Functions.
  #define AGG_SQRT sqrtf
  #define AGG_POW  powf
  #define AGG_ABS  fabsf
  #define AGG_FMIN fminf
  #define AGG_FMAX fmaxf
  #define AGG_SIGN copysignf
  #define AGG_MODF modff

  // Constants.
  #define AGG_FMT  "%e"
  #define AGG_NUM(I, F, S, E) I ## . ## F ## e ## S ## E ## f
  #define AGG_MIN  -FLT_MAX
  #define AGG_MAX  FLT_MAX
#elif AGG_BIT == 64
  // Types.
  #define AGG_TYPE double
  #define AGG_SIZE sizeof(AGG_TYPE)

  // Functions.
  #define AGG_SQRT sqrt
  #define AGG_POW  pow
  #define AGG_ABS  fabs
  #define AGG_FMIN fmin
  #define AGG_FMAX fmax
  #define AGG_SIGN copysign
  #define AGG_MODF modf

  // Constants.
  #define AGG_FMT  "%le"
  #define AGG_NUM(I, F, S, E) I ## . ## F ## e ## S ## E 
  #define AGG_MIN  -DBL_MAX
  #define AGG_MAX  DBL_MAX
#elif AGG_BIT == 128
  // Ensure that the type cannot be seleted when a strict standard-compliance is requested.
  #if AGG_STD == 1
    #error "AGG_BIT == 128 is a non-standard extension"
  #endif

  // Types.
  #define AGG_TYPE __float128
  #define AGG_SIZE sizeof(AGG_TYPE)

  // Functions.
  #define AGG_SQRT sqrtq
  #define AGG_POW  powq
  #define AGG_ABS  fabsq
  #define AGG_FMIN fminq
  #define AGG_FMAX fmaxq
  #define AGG_SIGN copysignq
  #define AGG_MODF modfq

  // Constants.
  #define AGG_FMT  "%Qe"
  #define AGG_NUM(I, F, S, E) I ## . ## F ## e ## S ## E ## Q
  #define AGG_MIN  -FLT128_MAX
  #define AGG_MAX  FLT128_MAX
#else
  #error "invalid value of AGG_BIT: " AGG_BIT
#endif

// Numerical constants.
#define AGG_0_0  AGG_NUM(0,  0, +, 0)
#define AGG_0_1  AGG_NUM(0,  1, +, 0)
#define AGG_0_5  AGG_NUM(0,  5, +, 0)
#define AGG_0_75 AGG_NUM(0, 75, +, 0)
#define AGG_0_9  AGG_NUM(0,  9, +, 0)
#define AGG_0_99 AGG_NUM(0, 99, +, 0)
#define AGG_1_0  AGG_NUM(1,  0, +, 0)
#define AGG_1_5  AGG_NUM(1,  5, +, 0)
#define AGG_2_0  AGG_NUM(2,  0, +, 0)
#define AGG_3_0  AGG_NUM(3,  0, +, 0)
#define AGG_4_0  AGG_NUM(4,  0, +, 0)
#define AGG_5_0  AGG_NUM(5,  0, +, 0)
#define AGG_6_0  AGG_NUM(6,  0, +, 0)

/// Aggregate function types.
#define AGG_FNC_FST 0x1 // First.
#define AGG_FNC_LST 0x2 // Last.
#define AGG_FNC_CNT 0x3 // Count.
#define AGG_FNC_SUM 0x4 // Sum.
#define AGG_FNC_MIN 0x5 // Minimum.
#define AGG_FNC_MAX 0x6 // Maximum.
#define AGG_FNC_AVG 0x7 // Average.
#define AGG_FNC_VAR 0x8 // Variance.
#define AGG_FNC_DEV 0x9 // Standard deviation.
#define AGG_FNC_SKW 0xa // Skewness.
#define AGG_FNC_KRT 0xb // Kurtosis.
#define AGG_FNC_QNT 0xc // Quantile.
#define AGG_FNC_MED 0xd // Median.


/// Aggregate function.
struct agg {
  uint8_t  ag_fnc;     ///< Type.
  uint8_t  ag_pad[7];  ///< Padding (unused).
  uint64_t ag_cnt[5];  ///< Number of observations.
  AGG_TYPE ag_par;     ///< Function argument.
  AGG_TYPE ag_val[10]; ///< State variables.
};

/// On-line algorithms.
void agg_new(struct agg* agg, const uint8_t fnc, const AGG_TYPE par);
void agg_put(struct agg* agg, const AGG_TYPE val);
bool agg_get(const struct agg *restrict agg, AGG_TYPE *restrict val);

/// Off-line algorithms.
bool agg_run(      AGG_TYPE *restrict val,
             const AGG_TYPE *restrict arr,
             const uint64_t           len,
             const uint8_t            fnc,
             const AGG_TYPE           par);

#endif
