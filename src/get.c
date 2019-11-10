// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#include <stddef.h>
#include <math.h>

#include "agg.h"


/// Obtain the first value of the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out output value
static bool
get_fst(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  if (agg->ag_cnt > 0) {
    *out = agg->ag_val[0];
    return true;
  } else {
    return false;
  }
}

/// Obtain the last value of the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out output value
static bool
get_lst(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  if (agg->ag_cnt > 0) {
    *out = agg->ag_val[0];
    return true;
  } else {
    return false;
  }
}

/// Obtain the number of values in the stream.
/// @return always true
///
/// @param[in]  agg aggregate function
/// @param[out] out output value
static bool
get_cnt(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = (AGG_TYPE)agg->ag_cnt;
  return true;
}

/// Obtain a sum of all values in the stream.
/// @return always true
///
/// @param[in]  agg aggregate function
/// @param[out] out output value
static bool
get_sum(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = agg->ag_val[0];
  return true;
}

/// Obtain the minimal value in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out output value
static bool
get_min(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  if (agg->ag_cnt > 0) {
    *out = agg->ag_val[0];
    return true;
  } else {
    return false;
  }
}

/// Obtain the maximal value in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out output value
static bool
get_max(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  if (agg->ag_cnt > 0) {
    *out = agg->ag_val[0];
    return true;
  } else {
    return false;
  }
}

/// Obtain the average value in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out output value
static bool
get_avg(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  if (agg->ag_cnt > 0) {
    *out = agg->ag_val[0];
    return true;
  } else {
    return false;
  }
}

/// Obtain the variance of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out variance
static bool
get_var(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  if (agg->ag_cnt > 1) {
    *out = agg->ag_val[1] / (AGG_TYPE)(agg->ag_cnt - 1);
    return true;
  } else {
    return false;
  }
}

/// Obtain the standard deviation of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out standard deviation
static bool
get_dev(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  bool ret;

  ret = get_var(agg, out);
  if (ret == true) {
    *out = AGG_SQRT(*out);
    return true;
  } else {
    return false;
  }
}

/// Obtain the skewness of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out skewness
static bool
get_skw(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = AGG_SQRT((AGG_TYPE)agg->ag_cnt)
       * agg->ag_val[2]
       / AGG_POW(agg->ag_val[1], AGG_1_5);

  return true;
}

/// Obtain the kurtosis of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out kurtosis
static bool
get_krt(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = (AGG_TYPE)(agg->ag_cnt)
       * agg->ag_val[3] 
       / (agg->ag_val[1] * agg->ag_val[1])
       - 3.0;
  return true;
}

/// Function table for get_* functions based on ag_fnc.
static bool (*get_fnc[])(const struct agg*, AGG_TYPE*) = {
  NULL,
  get_fst,
  get_lst,
  get_cnt,
  get_sum,
  get_min,
  get_max,
  get_avg,
  get_var,
  get_dev,
  get_skw,
  get_krt
};

/// Obtain the aggregated value.
/// @return success/failure indication
/// 
/// @param[in]  agg aggregate function
/// @param[out] out output value
bool
agg_get(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  return get_fnc[agg->ag_fnc](agg, out);
}
