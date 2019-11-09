// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#ifndef AGG_H
#define AGG_H

#include <stdbool.h>
#include <stdint.h>
#include <float.h>


// Determine the appropriate type-related constants and functions.
#ifdef AGG_FLT
  // Types.
  #define AGG_TYPE float 

  // Functions.
  #define AGG_SQRT sqrtf
  #define AGG_POW  powf
  #define AGG_ABS  fabsf
  #define AGG_FMIN fminf
  #define AGG_FMAX fmaxf

  // Constants.
  #define AGG_FMT  "%e"
  #define AGG_0_0  0.0f
  #define AGG_1_0  1.0f
  #define AGG_1_5  1.5f
  #define AGG_2_0  2.0f
  #define AGG_3_0  3.0f
  #define AGG_4_0  4.0f
  #define AGG_6_0  6.0f
  #define AGG_MIN  -FLT_MAX
  #define AGG_MAX  FLT_MAX
#else
  // Types.
  #define AGG_TYPE double
  
  // Functions.
  #define AGG_SQRT sqrt
  #define AGG_POW  pow
  #define AGG_ABS  fabs
  #define AGG_FMIN fmin
  #define AGG_FMAX fmax

  // Constants.
  #define AGG_FMT  "%le"
  #define AGG_0_0  0.0
  #define AGG_1_0  1.0
  #define AGG_1_5  1.5
  #define AGG_2_0  2.0
  #define AGG_3_0  3.0
  #define AGG_4_0  4.0
  #define AGG_6_0  6.0
  #define AGG_MIN  -DBL_MAX
  #define AGG_MAX  DBL_MAX
#endif

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

/// Aggregate function. 
struct agg {
  uint8_t  ag_fnc;    ///< Type.
  uint8_t  ag_pad[7]; ///< Padding (unused).
  uint64_t ag_cnt;    ///< Number of observations.
  AGG_TYPE ag_val[4]; ///< State variables.
  AGG_TYPE ag_tmp[4]; ///< Temporary variables.
};

/// On-line algorithms.
bool agg_new(struct agg* agg, const uint8_t fnc);
void agg_put(struct agg* agg, const AGG_TYPE val);
bool agg_get(const struct agg* agg, AGG_TYPE* val);

/// Off-line algorithms.
bool agg_run(AGG_TYPE* val, const AGG_TYPE* arr, const uint64_t len, const uint8_t fnc);

#endif
