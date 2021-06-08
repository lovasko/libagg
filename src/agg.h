// Copyright (c) 2019-2021 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#ifndef AGGSTAT_H
#define AGGSTAT_H

#include <stdbool.h>
#include <stdint.h>
#include <float.h>


// This constant ensures that all code that is made available by this library is strictly compliant
// with the C99 language standard.
#ifndef AGGSTAT_STD
  #define AGGSTAT_STD 1
#endif

// This constant selects the width of the floating point type used by the library in all
// computations. The default value is 64, which denotes the `double` type.  Other permissible values
// include 32 for `float` and 128 for `__float128`. The latter type is a non-standard extension and
// will result in compile-time error unless the `AGGSTAT_STD` macro evaluates to `0`.
#ifndef AGGSTAT_FLT_BIT
  #define AGGSTAT_FLT_BIT 64
#endif

// This constant selects the width of the unsigned integer type used by the
// library in all computations. The default value is 64, which denotes the
// `uint64_t` type. Other permissible values are 32 for `uint32_t` and 16 for
// `uint16_t`. Furthermore, value 128 results in the `__uint128_t` type which
// is a non-standard extension that will result in compile-time error unless
// the `AGGSTAT_STD` macro evaluates to `0`.
#ifndef AGGSTAT_INT_BIT
  #define AGGSTAT_INT_BIT 64
#endif

// Determine the appropriate type-related constants and functions.
#if AGGSTAT_FLT_BIT == 32
  // Types.
  #define AGGSTAT_FLT float

  // Functions.
  #define AGGSTAT_SQRT sqrtf
  #define AGGSTAT_POW  powf
  #define AGGSTAT_ABS  fabsf
  #define AGGSTAT_FMIN fminf
  #define AGGSTAT_FMAX fmaxf
  #define AGGSTAT_SIGN copysignf
  #define AGGSTAT_MODF modff

  // Constants.
  #define AGGSTAT_FMT  "%e"
  #define AGGSTAT_NUM(I, F, S, E) I ## . ## F ## e ## S ## E ## f
  #define AGGSTAT_MIN  -FLT_MAX
  #define AGGSTAT_MAX  FLT_MAX
#elif AGGSTAT_FLT_BIT == 64
  // Types.
  #define AGGSTAT_FLT double

  // Functions.
  #define AGGSTAT_SQRT sqrt
  #define AGGSTAT_POW  pow
  #define AGGSTAT_ABS  fabs
  #define AGGSTAT_FMIN fmin
  #define AGGSTAT_FMAX fmax
  #define AGGSTAT_SIGN copysign
  #define AGGSTAT_MODF modf

  // Constants.
  #define AGGSTAT_FMT  "%le"
  #define AGGSTAT_NUM(I, F, S, E) I ## . ## F ## e ## S ## E
  #define AGGSTAT_MIN  -DBL_MAX
  #define AGGSTAT_MAX  DBL_MAX
#elif AGGSTAT_FLT_BIT == 80
  // Types.
  #define AGGSTAT_FLT long double

  // Functions.
  #define AGGSTAT_SQRT sqrtl
  #define AGGSTAT_POW  powl
  #define AGGSTAT_ABS  fabsl
  #define AGGSTAT_FMIN fminl
  #define AGGSTAT_FMAX fmaxl
  #define AGGSTAT_SIGN copysignl
  #define AGGSTAT_MODF modfl

  // Constants.
  #define AGGSTAT_FMT  "%Le"
  #define AGGSTAT_NUM(I, F, S, E) I ## . ## F ## e ## S ## E ## L
  #define AGGSTAT_MIN  -LDBL_MAX
  #define AGGSTAT_MAX  LDBL_MAX
#elif AGGSTAT_FLT_BIT == 128
  // Ensure that the type cannot be seleted when a strict standard-compliance is requested.
  #if AGGSTAT_STD == 1
    #error "AGGSTAT_FLT_BIT == 128 is a non-standard extension"
  #endif

  // Types.
  #define AGGSTAT_FLT __float128

  // Functions.
  #define AGGSTAT_SQRT sqrtq
  #define AGGSTAT_POW  powq
  #define AGGSTAT_ABS  fabsq
  #define AGGSTAT_FMIN fminq
  #define AGGSTAT_FMAX fmaxq
  #define AGGSTAT_SIGN copysignq
  #define AGGSTAT_MODF modfq

  // Constants.
  #define AGGSTAT_FMT  "%Qe"
  #define AGGSTAT_NUM(I, F, S, E) I ## . ## F ## e ## S ## E ## Q
  #define AGGSTAT_MIN  -FLT128_MAX
  #define AGGSTAT_MAX  FLT128_MAX
#else
  #error "invalid value of AGGSTAT_FLT_BIT: " AGGSTAT_FLT_BIT
#endif

#if AGGSTAT_INT_BIT == 16
  #define AGGSTAT_INT     uint16_t
  #define AGGSTAT_INT_MAX UINT16_MAX
#elif AGGSTAT_INT_BIT == 32
  #define AGGSTAT_INT     uint32_t
  #define AGGSTAT_INT_MAX UINT32_MAX
#elif AGGSTAT_INT_BIT == 64
  #define AGGSTAT_INT     uint64_t
  #define AGGSTAT_INT_MAX UINT64_MAX
#elif AGGSTAT_INT_BIT == 128
  // Ensure that the type cannot be selected when a strict standard-compliance is requested.
  #if AGGSTAT_STD == 1
    #error "AGGSTAT_INT_BIT == 128 is a non-standard extension"
  #endif
  #define AGGSTAT_INT     __uint128_t
  #define AGGSTAT_INT_MAX UINT128_MAX
#else
  #error "invalid value of AGGSTAT_INT_BIT: " AGGSTAT_INT_BIT
#endif

// Numerical constants.
#define AGGSTAT_0_0  AGGSTAT_NUM(0,  0, +, 0)
#define AGGSTAT_0_1  AGGSTAT_NUM(0,  1, +, 0)
#define AGGSTAT_0_5  AGGSTAT_NUM(0,  5, +, 0)
#define AGGSTAT_0_75 AGGSTAT_NUM(0, 75, +, 0)
#define AGGSTAT_0_9  AGGSTAT_NUM(0,  9, +, 0)
#define AGGSTAT_0_99 AGGSTAT_NUM(0, 99, +, 0)
#define AGGSTAT_1_0  AGGSTAT_NUM(1,  0, +, 0)
#define AGGSTAT_1_5  AGGSTAT_NUM(1,  5, +, 0)
#define AGGSTAT_2_0  AGGSTAT_NUM(2,  0, +, 0)
#define AGGSTAT_3_0  AGGSTAT_NUM(3,  0, +, 0)
#define AGGSTAT_4_0  AGGSTAT_NUM(4,  0, +, 0)
#define AGGSTAT_5_0  AGGSTAT_NUM(5,  0, +, 0)
#define AGGSTAT_6_0  AGGSTAT_NUM(6,  0, +, 0)

/// Aggregate function types.
#define AGGSTAT_FNC_FST 0x1 // First.
#define AGGSTAT_FNC_LST 0x2 // Last.
#define AGGSTAT_FNC_CNT 0x3 // Count.
#define AGGSTAT_FNC_SUM 0x4 // Sum.
#define AGGSTAT_FNC_MIN 0x5 // Minimum.
#define AGGSTAT_FNC_MAX 0x6 // Maximum.
#define AGGSTAT_FNC_AVG 0x7 // Average.
#define AGGSTAT_FNC_VAR 0x8 // Variance.
#define AGGSTAT_FNC_DEV 0x9 // Standard deviation.
#define AGGSTAT_FNC_SKW 0xa // Skewness.
#define AGGSTAT_FNC_KRT 0xb // Kurtosis.
#define AGGSTAT_FNC_QNT 0xc // Quantile.
#define AGGSTAT_FNC_MED 0xd // Median.


/// Aggregate function.
struct aggstat {
  uint8_t     ag_fnc;     ///< Type.
  uint8_t     ag_pad[7];  ///< Padding (unused).
  AGGSTAT_INT ag_cnt[5];  ///< Number of observations.
  AGGSTAT_FLT ag_par;     ///< Function argument.
  AGGSTAT_FLT ag_val[10]; ///< State variables.
};

/// On-line algorithms.
void aggstat_new(struct aggstat* agg, const uint8_t fnc, const AGGSTAT_FLT par);
void aggstat_put(struct aggstat* agg, const AGGSTAT_FLT val);
bool aggstat_get(const struct aggstat *restrict agg, AGGSTAT_FLT *restrict val);

/// Off-line algorithms.
bool aggstat_run(      AGGSTAT_FLT *restrict val,
                 const AGGSTAT_FLT *restrict arr,
                 const AGGSTAT_INT           len,
                 const uint8_t               fnc,
                 const AGGSTAT_FLT           par);

#endif
