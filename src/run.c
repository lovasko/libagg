// Copyright (c) 2019-2021 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#include <stdlib.h>
#include <math.h>

#include "agg.h"


/// Compute the first value in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out first value
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  par parameter (unused)
static bool
run_fst(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  (void)par;

  if (len == 0) {
    return false;
  }

  *out = arr[0];
  return true;
}

/// Compute the last value in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out last value
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  par parameter (unused)
static bool
run_lst(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  (void)par;

  if (len == 0) {
    return false;
  }

  *out = arr[len - 1];
  return true;
}

/// Compute the number of values in the stream given the full stream information.
/// @return always true
///
/// @param[out] out number of values
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  par parameter (unused)
static bool
run_cnt(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  (void)arr;
  (void)par;

  *out = (AGGSTAT_FLT)len;
  return true;
}

/// Compute the sum of values in the stream given the full stream information.
/// @return always true
///
/// @param[out] out sum of values
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  par parameter (unused)
static bool
run_sum(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  uint64_t    idx;
  AGGSTAT_FLT sum;

  (void)par;

  sum = AGGSTAT_0_0;
  for (idx = 0; idx < len; idx++) {
    sum += arr[idx];
  }

  *out = sum;
  return true;
}

/// Compute the minimal value in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out minimal value
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  par parameter (unused)
static bool
run_min(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  uint64_t    idx;
  AGGSTAT_FLT min;

  (void)par;

  if (len == 0) {
    return false;
  }

  min = arr[0];
  for (idx = 1; idx < len; idx++) {
    min = AGGSTAT_FMIN(min, arr[idx]);
  }

  *out = min;
  return true;
}

/// Compute the maximal value in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out maximal value
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  par parameter (unused)
static bool
run_max(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  uint64_t    idx;
  AGGSTAT_FLT max;

  // Ignore the parameter.
  (void)par;

  if (len == 0) {
    return false;
  }

  max = arr[0];
  for (idx = 1; idx < len; idx++) {
    max = AGGSTAT_FMAX(max, arr[idx]);
  }

  *out = max;
  return true;
}

/// Compute the average value in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out average value
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  par parameter (unused)
static bool
run_avg(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  AGGSTAT_FLT sum;

  (void)par;

  if (len == 0) {
    return false;
  }

  (void)run_sum(&sum, arr, len, par);
  *out = sum / (AGGSTAT_FLT)len;
  return true;
}

/// Compute the variance of values in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out variance of values
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  par parameter (unused)
static bool
run_var(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  AGGSTAT_FLT avg;
  AGGSTAT_FLT var;
  uint64_t    idx;

  if (len == 0) {
    return false;
  }

  if (len == 1) {
    *out = AGGSTAT_0_0;
    return true;
  }

  (void)run_avg(&avg, arr, len, par);
  var = AGGSTAT_0_0;
  for (idx = 0; idx < len; idx++) {
    var += AGGSTAT_POW(arr[idx] - avg, AGGSTAT_2_0);
  }
  var /= (AGGSTAT_FLT)len - AGGSTAT_1_0;

  *out = var;
  return true;
}

/// Compute the standard deviation of values in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out standard deviation of values
/// @param[in]  arr array representing the stream
/// @param[in]  len array length
/// @param[in]  par parameter (unused)
static bool
run_dev(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  AGGSTAT_FLT var;

  if (len == 0) {
    return false;
  }

  (void)run_var(&var, arr, len, par);
  *out = AGGSTAT_SQRT(var);
  return true;
}

/// Compute the skewness of values in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out skewness of values
/// @param[in]  arr array representing the stream
/// @param[in]  len array length
/// @param[in]  par parameter (unused)
static bool
run_skw(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  uint64_t    idx;
  AGGSTAT_FLT avg;
  AGGSTAT_FLT dev;
  AGGSTAT_FLT skw;

  if (len < 2) {
    return false;
  }

  (void)run_avg(&avg, arr, len, par);
  (void)run_dev(&dev, arr, len, par);

  skw = AGGSTAT_0_0;
  for (idx = 0; idx < len; idx++) {
    skw += AGGSTAT_POW(arr[idx] - avg, AGGSTAT_3_0) / (AGGSTAT_FLT)len;
  }
  skw /= AGGSTAT_POW(dev, AGGSTAT_3_0);

  *out = skw;
  return true;
}

/// Compute the kurtosis of values in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out kurtosis of values
/// @param[in]  arr array representing the stream
/// @param[in]  len array length
/// @param[in]  par parameter (unused)
static bool
run_krt(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  uint64_t    idx;
  AGGSTAT_FLT avg;
  AGGSTAT_FLT dev;
  AGGSTAT_FLT krt;

  if (len < 2) {
    return false;
  }

  (void)run_avg(&avg, arr, len, par);
  (void)run_dev(&dev, arr, len, par);

  krt = AGGSTAT_0_0;
  for (idx = 0; idx < len; idx++) {
    krt += AGGSTAT_POW(arr[idx] - avg, AGGSTAT_4_0) / (AGGSTAT_FLT)len;
  }
  krt /= AGGSTAT_POW(dev, AGGSTAT_4_0);

  *out = krt - AGGSTAT_3_0;
  return true;
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
  AGGSTAT_FLT x;
  AGGSTAT_FLT y;

  x = *(AGGSTAT_FLT*)a;
  y = *(AGGSTAT_FLT*)b;

  return (x > y) - (x < y);
}

/// Compute the p-quantile of the values in the stream given full stream information.
/// @return success/failure indication
///
/// @param[out] out p-quantile of values
/// @param[in]  arr array representing the stream
/// @param[in]  len array length
/// @param[in]  par parameter
static bool
run_qnt(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  uint64_t    idx;
  AGGSTAT_FLT inp;
  AGGSTAT_FLT frp;

  // Validate the stream length.
  if (len == 0) {
    return false;
  }

  // Validate the parameter.
  if (AGGSTAT_0_0 > par && par > AGGSTAT_1_0) {
    return false;
  }

  // Sort the stream.
  (void)qsort((void*)arr, len, sizeof(AGGSTAT_FLT), qnt_cmp);

  // Select the appropriate field. This is achieved by finding the precise decimal index, followed
  // by decomposition of the number into the integral and fractional parts.
  frp = AGGSTAT_MODF((len - 1) * par, &inp);
  idx = (uint64_t)inp;

  // Perform linear interpolation between the two candidate values. The first of the values
  // corresponds to the integral part, whereas the parameter for the linear interpolation is the
  // fractional part.
  if (idx == (len - 1)) {
    *out = arr[idx];
  } else {
    *out = arr[idx] + frp * (arr[idx + 1] - arr[idx]);
  }

  return true;
}

/// Compute the median of the values in the stream given full stream information.
/// @return success/failure indication
///
/// @param[out] out median of values
/// @param[in]  arr array representing the stream
/// @param[in]  len array length
/// @param[in]  par parameter (unused)
static bool
run_med(      AGGSTAT_FLT *restrict out,
        const AGGSTAT_FLT *restrict arr,
        const uint64_t              len,
        const AGGSTAT_FLT           par)
{
  (void)par;
  return run_qnt(out, arr, len, AGGSTAT_0_5);
}

/// Function table for push_* functions based on ag_typ.
static bool (*run_fnc[])(AGGSTAT_FLT*, const AGGSTAT_FLT*, const uint64_t, const AGGSTAT_FLT) = {
  NULL,
  run_fst,
  run_lst,
  run_cnt,
  run_sum,
  run_min,
  run_max,
  run_avg,
  run_var,
  run_dev,
  run_skw,
  run_krt,
  run_qnt,
  run_med
};

/// Compute an aggregate of a stream with full information.
/// @return success/failure indication
///
/// @param[out] val aggregate of the stream
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  fnc aggregate function
/// @param[in]  par parameter
bool
aggstat_run(      AGGSTAT_FLT *restrict val,
            const AGGSTAT_FLT *restrict arr,
            const uint64_t              len,
            const uint8_t               fnc,
            const AGGSTAT_FLT           par)
{
  return run_fnc[fnc](val, arr, len, par);
}
