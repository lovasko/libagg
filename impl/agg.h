// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#ifndef AGG_H
#define AGG_H

#include <stdbool.h>
#include <stdint.h>


// Determine the appropriate type-related constants and functions.
#ifdef AGG_FLT
  #define AGG_TYPE float 
  #define AGG_FMT  "%f"
  #define AGG_SQRT sqrtf
  #define AGG_POW  powf
  #define AGG_ABS  fabsf
  #define AGG_0_0  0.0f
  #define AGG_1_0  1.0f
  #define AGG_1_5  1.5f
  #define AGG_2_0  2.0f
  #define AGG_3_0  3.0f
  #define AGG_4_0  4.0f
#else
  #define AGG_TYPE double
  #define AGG_FMT  "%lf"
  #define AGG_SQRT sqrt
  #define AGG_POW  pow
  #define AGG_ABS  fabs
  #define AGG_0_0  0.0
  #define AGG_1_0  1.0
  #define AGG_1_5  1.5
  #define AGG_2_0  2.0
  #define AGG_3_0  3.0
  #define AGG_4_0  4.0
#endif


/// Aggregate function. 
struct agg {
  uint8_t  ag_typ;    ///< Type.
  uint8_t  ag_pad[7]; ///< Padding (unused).
  uint64_t ag_cnt;    ///< Number of observations.
  AGG_TYPE ag_val[4]; ///< State variables.
};

/// Start the aggregate.
void agg_fst(struct agg* agg);
void agg_lst(struct agg* agg);
void agg_cnt(struct agg* agg);
void agg_sum(struct agg* agg);
void agg_min(struct agg* agg);
void agg_max(struct agg* agg);
void agg_avg(struct agg* agg);
void agg_var(struct agg* agg);
void agg_dev(struct agg* agg);
void agg_skw(struct agg* agg);
void agg_krt(struct agg* agg);

/// Reset the aggregate.
void agg_rst(struct agg* agg);

/// Update the aggregate.
void agg_put(struct agg* agg, const AGG_TYPE inp);

/// Obtain the aggregate.
bool agg_get(const struct agg* agg, AGG_TYPE* out);

#endif
