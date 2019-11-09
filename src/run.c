// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#include <stddef.h>
#include <math.h>

#include "agg.h"


/// Compute the first value in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out first value
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
static bool
run_fst(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
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
static bool
run_lst(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  if (len == 0) {
    return false;
  }

  *out = arr[len - 1];
  return true;
}

/// Compute the number of values in the stream given the full stream
/// information.
/// @return always true
///
/// @param[out] out number of values
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
static bool
run_cnt(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  (void)arr;

  *out = (AGG_TYPE)len;
  return true;
}

/// Compute the sum of values in the stream given the full stream information.
/// @return always true
///
/// @param[out] out sum of values
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
static bool
run_sum(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  uint64_t idx;
  AGG_TYPE sum;

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
static bool
run_min(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  uint64_t idx;
  AGG_TYPE min;

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
static bool
run_max(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  uint64_t idx;
  AGG_TYPE max;

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
static bool
run_avg(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  AGG_TYPE sum;

  if (len == 0) {
    return false;
  }

  (void)run_sum(&sum, arr, len);
  *out = sum / (AGG_TYPE)len;
  return true;
}

/// Compute the variance of values in the stream given the full stream
/// information.
/// @return success/failure indication
///
/// @param[out] out variance of values
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
static bool
run_var(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
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

  (void)run_avg(&avg, arr, len);
  var = AGG_0_0;
  for (idx = 0; idx < len; idx++) {
    var += AGG_POW(arr[idx] - avg, AGG_2_0);
  }
  var /= (AGG_TYPE)len - AGG_1_0;

  *out = var;
  return true;
}

/// Compute the standard deviation of values in the stream given the full
/// stream information.
/// @return success/failure indication
///
/// @param[out] out standard deviation of values
/// @param[in]  arr array representing the stream
/// @param[in]  len array length
static bool
run_dev(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  AGG_TYPE var;

  if (len == 0) {
    return false;
  }

  (void)run_var(&var, arr, len);
  *out = AGG_SQRT(var);
  return true;
}

/// Compute the skewness of values in the stream given the full stream
/// information.
/// @return success/failure indication
///
/// @param[out] out skewness of values
/// @param[in]  arr array representing the stream
/// @param[in]  len array length
static bool
run_skw(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  uint64_t idx;
  AGG_TYPE avg;
  AGG_TYPE dev;
  AGG_TYPE skw;

  if (len < 2) {
    return false;
  }

  (void)run_avg(&avg, arr, len);
  (void)run_dev(&dev, arr, len);

  skw = AGG_0_0;
  for (idx = 0; idx < len; idx++) {
    skw += AGG_POW(arr[idx] - avg, AGG_3_0) / (AGG_TYPE)len;
  }
  skw /= AGG_POW(dev, AGG_3_0);

  *out = skw;
  return true;
}

/// Compute the kurtosis of values in the stream given the full stream
/// information.
/// @return success/failure indication
///
/// @param[out] out kurtosis of values
/// @param[in]  arr array representing the stream
/// @param[in]  len array length
static bool
run_krt(AGG_TYPE* out, const AGG_TYPE* arr, const uint64_t len)
{
  uint64_t idx;
  AGG_TYPE avg;
  AGG_TYPE dev;
  AGG_TYPE krt;

  if (len < 2) {
    return false;
  }

  (void)run_avg(&avg, arr, len);
  (void)run_dev(&dev, arr, len);

  krt = AGG_0_0;
  for (idx = 0; idx < len; idx++) {
    krt += AGG_POW(arr[idx] - avg, AGG_4_0) / (AGG_TYPE)len;
  }
  krt /= AGG_POW(dev, AGG_4_0);

  *out = krt - AGG_3_0;
  return true;
}

/// Function table for push_* functions based on ag_typ.
static bool (*run_fnc[])(AGG_TYPE*, const AGG_TYPE*, const uint64_t) = {
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
  run_krt
};

/// Compute an aggregate of a stream with full information.
/// @return success/failure indication
///
/// @param[out] val aggregate of the stream
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
/// @param[in]  fnc aggregate function ID
bool
agg_run(AGG_TYPE* val, const AGG_TYPE* arr, const uint64_t len, const uint8_t fnc)
{
  return run_fnc[fnc](val, arr, len);
}
