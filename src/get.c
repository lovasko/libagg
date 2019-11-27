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
/// @param[out] out first value
static bool
get_fst(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = agg->ag_val[0];
  return agg->ag_cnt[0] > 0;
}

/// Obtain the last value of the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out last value
static bool
get_lst(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = agg->ag_val[0];
  return agg->ag_cnt[0] > 0;
}

/// Obtain the number of values in the stream.
/// @return always true
///
/// @param[in]  agg aggregate function
/// @param[out] out number of values
static bool
get_cnt(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = (AGG_TYPE)agg->ag_cnt[0];
  return true;
}

/// Obtain a sum of all values in the stream.
/// @return always true
///
/// @param[in]  agg aggregate function
/// @param[out] out sum of values
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
/// @param[out] out minimal value
static bool
get_min(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = agg->ag_val[4];
  return agg->ag_cnt[0] > 0;
}

/// Obtain the maximal value in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out maximal value
static bool
get_max(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = agg->ag_val[5];
  return agg->ag_cnt[0] > 0;
}

/// Obtain the average value in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out average value
static bool
get_avg(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = agg->ag_val[0];
  return agg->ag_cnt[0] > 0;
}

/// Obtain the variance of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out variance of values
static bool
get_var(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  // The following potential division by zero might appear as a mistake; it is not. The division by
  // zero is a well-defined IEEE-745 operation yielding a positive/negative infinity. The value
  // itself in this case is wrong, but that does not matter, as the function returns `false` in
  // this case, meaning that the resulting value shall not be consulted.
  *out = agg->ag_val[1] / (AGG_TYPE)(agg->ag_cnt[0] - 1);
  return agg->ag_cnt[0] > 1;
}

/// Obtain the standard deviation of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out standard deviation of values
static bool
get_dev(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  bool ret;

  ret = get_var(agg, out);
  *out = AGG_SQRT(*out);
  return ret;
}

/// Obtain the skewness of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out skewness of values
static bool
get_skw(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = AGG_SQRT((AGG_TYPE)agg->ag_cnt[0])
       * agg->ag_val[2]
       / AGG_POW(agg->ag_val[1], AGG_1_5);

  return true;
}

/// Obtain the kurtosis of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out kurtosis of values
static bool
get_krt(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  *out = (AGG_TYPE)(agg->ag_cnt[0])
       * agg->ag_val[3]
       / (agg->ag_val[1] * agg->ag_val[1])
       - AGG_3_0;
  return true;
}

/// Obtain the p-quantile of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out p-quantile of values
static bool
get_qtl(const struct agg* restrict agg, AGG_TYPE *restrict out)
{
  *out = agg->ag_val[2];
  return agg->ag_cnt[4] > 0;
}

/// Obtain the median of the values in the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out median of values
static bool
get_med(const struct agg* restrict agg, AGG_TYPE *restrict out)
{
  return get_qtl(agg, out);
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
  get_krt,
  get_qtl,
  get_med
};

/// Obtain the aggregated value.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out aggregate value
bool
agg_get(const struct agg *restrict agg, AGG_TYPE *restrict out)
{
  return get_fnc[agg->ag_fnc](agg, out);
}
