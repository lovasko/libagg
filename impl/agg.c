// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#include <math.h>

#include "agg.h"


/// Aggregate function types.
#define AGG_FNC_FST 0x0 // First.
#define AGG_FNC_LST 0x1 // Last.
#define AGG_FNC_CNT 0x2 // Count.
#define AGG_FNC_SUM 0x3 // Sum.
#define AGG_FNC_MIN 0x4 // Minimum.
#define AGG_FNC_MAX 0x5 // Maximum.
#define AGG_FNC_AVG 0x6 // Average.
#define AGG_FNC_VAR 0x7 // Variance.
#define AGG_FNC_DEV 0x8 // Standard deviation.
#define AGG_FNC_SKW 0x9 // Skewness.
#define AGG_FNC_KRT 0xa // Kurtosis.
//#define AGG_FNC_QTL 0xb // Quantile.
//#define AGG_FNC_USR 0xc // User-defined.

/// Update the first value of the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_fst(struct agg* agg, const AGG_TYPE inp)
{
  if (agg->ag_cnt == 0) {
    agg->ag_val[0] = inp;
  }
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
  if (agg->ag_cnt == 0 || agg->ag_val[0] > inp) {
    agg->ag_val[0] = inp;
  }
}

/// Update the maximal value in the stream.
///
/// @param[in] agg aggregate function
/// @param[in] inp input value
static void
put_max(struct agg* agg, const AGG_TYPE inp)
{
  if (agg->ag_cnt == 0 || agg->ag_val[0] < inp) {
    agg->ag_val[0] = inp;
  }
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
/// Note: As the standard deviation is computed as the square root of variance,
/// we choose to maintain the variance computation throughout the update
/// process.
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

/// Obtain the first value of the stream.
/// @return success/failure indication
///
/// @param[in]  agg aggregate function
/// @param[out] out output value
static bool
get_fst(const struct agg* agg, AGG_TYPE* out)
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
get_lst(const struct agg* agg, AGG_TYPE* out)
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
get_cnt(const struct agg* agg, AGG_TYPE* out)
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
get_sum(const struct agg* agg, AGG_TYPE* out)
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
get_min(const struct agg* agg, AGG_TYPE* out)
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
get_max(const struct agg* agg, AGG_TYPE* out)
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
get_avg(const struct agg* agg, AGG_TYPE* out)
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
get_var(const struct agg* agg, AGG_TYPE* out)
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
get_dev(const struct agg* agg, AGG_TYPE* out)
{
  bool retb;

  retb = get_var(agg, out);
  if (retb == true) {
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
get_skw(const struct agg* agg, AGG_TYPE* out)
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
get_krt(const struct agg* agg, AGG_TYPE* out)
{
  *out = (AGG_TYPE)(agg->ag_cnt)
       * agg->ag_val[3] 
       / (agg->ag_val[1] * agg->ag_val[1])
       - 3.0;
  return true;
}

/// Function table for put_* functions based on ag_typ.
static void (*put_fns[])(struct agg*, const AGG_TYPE) = {
  put_fst, put_lst, put_cnt,
  put_sum, put_min, put_max,
  put_avg, put_var, put_dev,
  put_skw, put_krt
};

/// Function table for get_* functions based on ag_typ.
static bool (*get_fns[])(const struct agg*, AGG_TYPE*) = {
  get_fst, get_lst, get_cnt,
  get_sum, get_min, get_max,
  get_avg, get_var, get_dev,
  get_skw, get_krt
};

/// Update the aggregated value.
///
/// @param[in] agg aggregated value
/// @param[in] inp input value
void
agg_put(struct agg* agg, const AGG_TYPE inp)
{
  put_fns[agg->ag_typ](agg, inp);
  agg->ag_cnt++;
}

/// Obtain the aggregated value.
/// @return success/failure indication
/// 
/// @param[in]  agg aggregate function
/// @param[out] out output value
bool
agg_get(const struct agg* agg, AGG_TYPE* out)
{
  return get_fns[agg->ag_typ](agg, out);
}

/// Initialize all counters and values to zero.
///
/// @param[in] agg aggregate function
static void
common_init(struct agg* agg)
{
  agg->ag_cnt    = 0;

  agg->ag_val[0] = AGG_0_0;
  agg->ag_val[1] = AGG_0_0;
  agg->ag_val[2] = AGG_0_0;
  agg->ag_val[3] = AGG_0_0;

  agg->ag_tmp[0] = AGG_0_0;
  agg->ag_tmp[1] = AGG_0_0;
  agg->ag_tmp[2] = AGG_0_0;
  agg->ag_tmp[3] = AGG_0_0;
}

/// Start aggregating the first value.
void
agg_fst(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_FST;
}

/// Start aggregating the last value.
///
/// @param[in] agg aggregate function
void
agg_lst(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_LST;
}

/// Start aggregating the number of values in the stream.
///
/// @param[in] agg aggregate function
void
agg_cnt(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_CNT;
}

/// Start aggregating the sum of all values.
///
/// @param[in] agg aggregate function
void
agg_sum(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_SUM;
}

/// Start aggregating the minimal value.
///
/// @param[in] agg aggregate function
void
agg_min(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_MIN;
}

/// Start aggregating the maximal value.
///
/// @param[in] agg aggregate function
void
agg_max(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_MAX;
}

/// Start aggregating the average value.
///
/// @param[in] agg aggregate function
void
agg_avg(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_AVG;
}

/// Start aggregating the variance of values.
///
/// @param[in] agg aggregate function
void
agg_var(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_VAR;
}

/// Start aggregating the standard deviation of values.
///
/// @param[in] agg aggregate function
void
agg_dev(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_DEV;
}

/// Start aggregating the skewness of values.
///
/// @param[in] agg aggregate function
void
agg_skw(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_SKW;
}

/// Start aggregating the kurtosis of values.
///
/// @param[in] agg aggregate function
void
agg_krt(struct agg* agg)
{
  common_init(agg);
  agg->ag_typ = AGG_FNC_KRT;
}

/// Reset the aggregate function.
///
/// @param[in] agg aggregate function
void
agg_rst(struct agg* agg)
{
  common_init(agg);
}
