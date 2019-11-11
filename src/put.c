// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#include <stddef.h>
#include <math.h>

#include "agg.h"


/// Update the first value of the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_fst(struct agg* agg, const AGG_TYPE inp)
{
  agg->ag_val[!!agg->ag_cnt] = inp;
}

/// Update the last value of the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void 
put_lst(struct agg* agg, const AGG_TYPE inp)
{
  agg->ag_val[0] = inp;
}

/// Update the number of values in the stream.
///
/// @param[in] agg aggregate function (unused)
/// @param[in] inp input value (unused)
static void
put_cnt(struct agg* agg, const AGG_TYPE inp)
{
  (void)agg;
  (void)inp;
}

/// Update the sum of values in the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_sum(struct agg* agg, const AGG_TYPE inp)
{
  agg->ag_val[0] += inp;
}

/// Update the minimal value in the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_min(struct agg* agg, const AGG_TYPE inp)
{
  agg->ag_val[2] = AGG_FMIN(inp, agg->ag_val[2]);
}

/// Update the maximal value in the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_max(struct agg* agg, const AGG_TYPE inp)
{
  agg->ag_val[3] = AGG_FMAX(inp, agg->ag_val[3]);
}

/// Pre-compute temporary variables.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
set_tmp(struct agg* agg, const AGG_TYPE inp)
{
  AGG_TYPE x;
  AGG_TYPE y;

  x = inp - agg->ag_val[0];
  y = x / (AGG_TYPE)(agg->ag_cnt + 1);

  agg->ag_tmp[0] = y;
  agg->ag_tmp[1] = y * y;
  agg->ag_tmp[2] = x * y * (AGG_TYPE)agg->ag_cnt;
}

/// Update the first moment.
///
/// @param[in] agg aggregate function
static void
fst_mnt(struct agg* agg)
{
  agg->ag_val[0] += agg->ag_tmp[0];
}

/// Update the second moment.
///
/// @param[in] agg aggregate function
static void
snd_mnt(struct agg* agg)
{
  agg->ag_val[1] += agg->ag_tmp[2];
}

/// Update the third moment.
///
/// @param[in] agg aggregate function
static void
trd_mnt(struct agg* agg)
{
  agg->ag_val[2] += agg->ag_tmp[2] * agg->ag_tmp[0] * (AGG_TYPE)(agg->ag_cnt - 1)
                  - AGG_3_0        * agg->ag_tmp[0] * agg->ag_val[1];
}

/// Update the fourth moment.
///
/// @param[in] agg aggregate function
static void
fth_mnt(struct agg* agg)
{
  AGG_TYPE x;

  x = (AGG_TYPE)(agg->ag_cnt + 1);
  agg->ag_val[3] += agg->ag_tmp[2] * agg->ag_tmp[1]
                  * (x * x - AGG_3_0 * x + AGG_3_0)
                  + AGG_6_0 * agg->ag_tmp[1] * agg->ag_val[1]
                  - AGG_4_0 * agg->ag_tmp[0] * agg->ag_val[2];
}

/// Update the average value in the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_avg(struct agg* agg, const AGG_TYPE inp)
{
  set_tmp(agg, inp);
  fst_mnt(agg);
}

/// Update the variance of the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_var(struct agg* agg, const AGG_TYPE inp)
{
  set_tmp(agg, inp);
  fst_mnt(agg);
  snd_mnt(agg);
}

/// Update the standard deviation of the stream.
///
/// Note: As the standard deviation is computed as the square root of variance, we choose to 
/// maintain the variance computation throughout the update process.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_dev(struct agg* agg, const AGG_TYPE inp)
{
  put_var(agg, inp);
}

/// Update the skewness of the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_skw(struct agg* agg, const AGG_TYPE inp)
{
  set_tmp(agg, inp);
  fst_mnt(agg);
  trd_mnt(agg);
  snd_mnt(agg);
}

/// Update the kurtosis of the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_krt(struct agg* agg, const AGG_TYPE inp)
{
  set_tmp(agg, inp);
  fst_mnt(agg);
  fth_mnt(agg);
  trd_mnt(agg);
  snd_mnt(agg);
}

/// Function table for put_* functions based on ag_fnc.
static void (*put_fnc[])(struct agg*, const AGG_TYPE) = {
  NULL,
  put_fst,
  put_lst,
  put_cnt,
  put_sum,
  put_min,
  put_max,
  put_avg,
  put_var,
  put_dev,
  put_skw,
  put_krt
};

/// Update the aggregated value.
///
/// @param[in] agg aggregated value
/// @param[in] inp input value
void
agg_put(struct agg* agg, const AGG_TYPE inp)
{
  put_fnc[agg->ag_fnc](agg, inp);
  agg->ag_cnt++;
}
