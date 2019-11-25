// Copyright (c) 2019 Daniel Lovasko
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
run_fst(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
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
run_lst(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
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
run_cnt(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  (void)arr;
  (void)par;

  *out = (AGG_TYPE)len;
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
run_sum(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  uint64_t idx;
  AGG_TYPE sum;

  (void)par;

  sum = AGG_0_0;
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
run_min(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  uint64_t idx;
  AGG_TYPE min;

  (void)par;

  if (len == 0) {
    return false;
  }

  min = arr[0];
  for (idx = 1; idx < len; idx++) {
    min = AGG_FMIN(min, arr[idx]);
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
run_max(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  uint64_t idx;
  AGG_TYPE max;

  // Ignore the parameter.
  (void)par;

  if (len == 0) {
    return false;
  }

  max = arr[0];
  for (idx = 1; idx < len; idx++) {
    max = AGG_FMAX(max, arr[idx]);
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
run_avg(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  AGG_TYPE sum;

  (void)par;

  if (len == 0) {
    return false;
  }

  (void)run_sum(&sum, arr, len, par);
  *out = sum / (AGG_TYPE)len;
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
run_var(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  AGG_TYPE avg;
  AGG_TYPE var;
  uint64_t idx;

  if (len == 0) {
    return false;
  }

  if (len == 1) {
    *out = AGG_0_0;
    return true;
  }

  (void)run_avg(&avg, arr, len, par);
  var = AGG_0_0;
  for (idx = 0; idx < len; idx++) {
    var += AGG_POW(arr[idx] - avg, AGG_2_0);
  }
  var /= (AGG_TYPE)len - AGG_1_0;

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
run_dev(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  AGG_TYPE var;

  if (len == 0) {
    return false;
  }

  (void)run_var(&var, arr, len, par);
  *out = AGG_SQRT(var);
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
run_skw(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  uint64_t idx;
  AGG_TYPE avg;
  AGG_TYPE dev;
  AGG_TYPE skw;

  if (len < 2) {
    return false;
  }

  (void)run_avg(&avg, arr, len, par);
  (void)run_dev(&dev, arr, len, par);

  skw = AGG_0_0;
  for (idx = 0; idx < len; idx++) {
    skw += AGG_POW(arr[idx] - avg, AGG_3_0) / (AGG_TYPE)len;
  }
  skw /= AGG_POW(dev, AGG_3_0);

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
run_krt(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  uint64_t idx;
  AGG_TYPE avg;
  AGG_TYPE dev;
  AGG_TYPE krt;

  if (len < 2) {
    return false;
  }

  (void)run_avg(&avg, arr, len, par);
  (void)run_dev(&dev, arr, len, par);

  krt = AGG_0_0;
  for (idx = 0; idx < len; idx++) {
    krt += AGG_POW(arr[idx] - avg, AGG_4_0) / (AGG_TYPE)len;
  }
  krt /= AGG_POW(dev, AGG_4_0);

  *out = krt - AGG_3_0;
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
  AGG_TYPE x;
  AGG_TYPE y;

  x = *(AGG_TYPE*)a;
  y = *(AGG_TYPE*)b;

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
run_qnt(      AGG_TYPE *restrict out,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const AGG_TYPE           par)
{
  uint64_t idx;
  AGG_TYPE x;
  AGG_TYPE y;

  // Validate the stream length.
  if (len == 0) {
    return false;
  }

  // Validate the parameter.
  if (AGG_0_0 > par && par > AGG_1_0) {
    return false;
  }

  // Sort the stream.
  (void)qsort((void*)arr, len, AGG_SIZE, qnt_cmp);

  // Select the appropriate field. This is achieved by finding the precise decimal index, followed
  // by decomposition of the number into the integral and fractional parts.
  x = AGG_MODF((len - 1) * par, &y);
  idx = (AGG_TYPE)x;

  // Perform linear interpolation between the two candidate values. The first of the values
  // corresponds to the integral part, whereas the parameter for the linear interpolation is the
  // fractional part.
  if (idx == (len - 1)) {
    *out = arr[idx];
  } else {
    *out = arr[idx] + y * (arr[idx + 1] - arr[idx]);
  }

  return true;
}

/// Function table for push_* functions based on ag_typ.
static bool (*run_fnc[])(AGG_TYPE*, const AGG_TYPE*, const uint64_t, const AGG_TYPE) = {
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
  run_qnt
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
agg_run(      AGG_TYPE *restrict val,
        const AGG_TYPE *restrict arr,
        const uint64_t           len,
        const uint8_t            fnc,
        const AGG_TYPE           par)
{
  return run_fnc[fnc](val, arr, len, par);
}
