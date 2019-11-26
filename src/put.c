// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#include <stdlib.h>
#include <math.h>

#include "agg.h"


/// Update the first value of the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_fst(struct agg* agg, const AGG_TYPE inp)
{
  agg->ag_val[!!agg->ag_cnt[0]] = inp;
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
  agg->ag_val[4] = AGG_FMIN(inp, agg->ag_val[4]);
}

/// Update the maximal value in the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_max(struct agg* agg, const AGG_TYPE inp)
{
  agg->ag_val[5] = AGG_FMAX(inp, agg->ag_val[5]);
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
  y = x / (AGG_TYPE)(agg->ag_cnt[0] + 1);

  agg->ag_tmp[0] = y;
  agg->ag_tmp[1] = y * y;
  agg->ag_tmp[2] = x * y * (AGG_TYPE)agg->ag_cnt[0];
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
  agg->ag_val[2] += agg->ag_tmp[2] * agg->ag_tmp[0] * (AGG_TYPE)(agg->ag_cnt[0] - 1)
                  - AGG_3_0        * agg->ag_tmp[0] * agg->ag_val[1];
}

/// Update the fourth moment.
///
/// @param[in] agg aggregate function
static void
fth_mnt(struct agg* agg)
{
  AGG_TYPE x;

  x = (AGG_TYPE)(agg->ag_cnt[0] + 1);
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

/// Linear extrapolation between two heights.
/// @return
///
/// This function works with the assumption that the `idx` argument
/// is never the first (0) or the last (4) element of the array.
///
/// @param[in] agg aggregate
/// @param[in] idx index of the value to adjust
/// @param[in] dir direction of the change
static AGG_TYPE
qnt_lin(struct agg* agg, const uint64_t idx, const int64_t dir)
{
  return agg->ag_val[idx]       + (AGG_TYPE)dir
      * (agg->ag_val[idx + dir] - agg->ag_val[idx])
      / (agg->ag_cnt[idx + dir] - agg->ag_cnt[idx]);
}

/// Parabolic extrapolation between two heights.
/// @return
///
/// This function works with the assumption that the `idx` argument
/// is never the first (0) or the last (4) element of the array.
///
/// @param[in] agg aggregate
/// @param[in] idx index of
/// @param[in] dir direction of the extrapolation
static AGG_TYPE
qnt_prb(struct agg* agg, const uint64_t idx, const int64_t dir)
{
  AGG_TYPE x;
  AGG_TYPE y;

  x = (AGG_TYPE)(agg->ag_cnt[idx]     - agg->ag_cnt[idx  - 1] + dir)
    *           (agg->ag_val[idx + 1] - agg->ag_val[idx])
    / (AGG_TYPE)(agg->ag_cnt[idx + 1] - agg->ag_cnt[idx]);

  y = (AGG_TYPE)(agg->ag_cnt[idx + 1] - agg->ag_cnt[idx] - dir)
    *           (agg->ag_val[idx]     - agg->ag_val[idx  - 1])
    / (AGG_TYPE)(agg->ag_cnt[idx]     - agg->ag_cnt[idx  - 1]);

  return agg->ag_val[idx] + dir
       * (x + y)
       / (AGG_TYPE)(agg->ag_cnt[idx + 1] - agg->ag_cnt[idx - 1]);
}

/// Readjust values after a new value was applied.
///
/// @param[in] agg aggregate
/// @param[in] idx index of the value to readjust
static void
qnt_adj(struct agg* agg, const uint64_t idx)
{
  AGG_TYPE dlt;
  bool     ord[2];
  int64_t  dir;
  AGG_TYPE par;

  // Compute the current differences.
  dlt    = agg->ag_val[idx + 5] - (AGG_TYPE)agg->ag_cnt[idx];
  ord[0] = agg->ag_cnt[idx + 1] > (agg->ag_cnt[idx] + 1);
  ord[1] = agg->ag_cnt[idx - 1] < (agg->ag_cnt[idx] - 1);

  // Only continue with the readjustment if the values are out of order.
  if ((dlt >= AGG_1_0 && ord[0]) || (dlt <= -AGG_1_0 && ord[1])) {
    // Decide the movement direction.
    dir = (int64_t)AGG_SIGN(AGG_1_0, dlt);

    // Determine which estimation to use.
    par = qnt_prb(agg, idx, dir);

    // In case the piecewise parabolic estimation would result in out of order values, revert to
    // the linear estimation.
    if (agg->ag_val[idx - 1] < par && par < agg->ag_val[idx + 1]) {
      agg->ag_val[idx] = par;
    } else {
      agg->ag_val[idx] = qnt_lin(agg, idx, dir);
    }

    agg->ag_cnt[idx] += dir;
  }
}

/// Compare two elements of the stream.
/// @return comparison
/// @retval 0 elements are equal
/// @retval 1 first element is greater
/// @retval -1 second element is greater
///
/// @param[in] a first element
/// @param[in] b second element
static int
qnt_cmp(const void* a, const void* b)
{
  AGG_TYPE x;
  AGG_TYPE y;

  x = *(AGG_TYPE*)a;
  y = *(AGG_TYPE*)b;

  return (x > y) - (x < y);
}

/// Update the p-quantile of the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_qnt(struct agg* agg, const AGG_TYPE inp)
{
  uint64_t inc[3];

  // Perform a sorted insert of the first 5 elements.
  if (agg->ag_cnt[4] < 4) {
    agg->ag_val[agg->ag_cnt[4]] = inp;
    agg->ag_cnt[4]++;

    return;
  }

  // Switch to the advanced algorithm.
  if (agg->ag_cnt[4] == 4) {
    agg->ag_val[4] = inp;

    // Sort the values.
    (void)qsort(agg->ag_val, 5, AGG_SIZE, qnt_cmp);

    // Initialise the counts.
    agg->ag_cnt[0] = 0; // Will get incremented by `agg_put`.
    agg->ag_cnt[1] = 2;
    agg->ag_cnt[2] = 3;
    agg->ag_cnt[3] = 4;
    agg->ag_cnt[4] = 5;

    // Initialize the desired counts.
    agg->ag_val[5] = AGG_1_0;
    agg->ag_val[6] = AGG_1_0 + AGG_2_0 * agg->ag_par;
    agg->ag_val[7] = AGG_1_0 + AGG_4_0 * agg->ag_par;
    agg->ag_val[8] = AGG_3_0 + AGG_2_0 * agg->ag_par;
    agg->ag_val[9] = AGG_5_0;

    return;
  }

  // Determine which counts need to be incremented.
  inc[0] = !!(inp < agg->ag_val[1]);
  inc[1] = !!(inp < agg->ag_val[2]);
  inc[2] = !!(inp < agg->ag_val[3]);

  // Increment the counts.
  agg->ag_cnt[1] += inc[0];
  agg->ag_cnt[2] += inc[0] || inc[1];
  agg->ag_cnt[3] += inc[0] || inc[1] || inc[2];
  agg->ag_cnt[4]++;

  // Adjust minimum and maximum.
  agg->ag_val[0] = AGG_FMIN(agg->ag_val[0], inp);
  agg->ag_val[4] = AGG_FMAX(agg->ag_val[4], inp);

  // Increment the desired counts.
  agg->ag_val[6] += agg->ag_par / AGG_2_0;
  agg->ag_val[7] += agg->ag_par;
  agg->ag_val[8] += (AGG_1_0 + agg->ag_par) / AGG_2_0;
  agg->ag_val[9] += AGG_1_0;

  // Adjust the middle values.
  qnt_adj(agg, 1);
  qnt_adj(agg, 2);
  qnt_adj(agg, 3);

  // Decrement the first count, a this gets automatically incremented by the generic `agg_put`
  // function. This can cause unsigned integer underflow, which is a well-defined behaviour.
  agg->ag_cnt[0]--;
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
  put_krt,
  put_qnt
};

/// Update the aggregated value.
///
/// @param[in] agg aggregated value
/// @param[in] inp input value
void
agg_put(struct agg* agg, const AGG_TYPE inp)
{
  put_fnc[agg->ag_fnc](agg, inp);
  agg->ag_cnt[0]++;
}
