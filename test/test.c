#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "../impl/agg.h"


/// Number of test cases per aggregate function.
#define TEST_LEN 6

/// Number of times each test case is re-tried.
#define TEST_TRY 1000

/// Test case.
struct test {
  float    ts_err; ///< Acceptable magnitude of error.
  uint64_t ts_cnt; ///< Number of samples.
};

/// Test cases for `agg_fst` function.
struct test test_fst[TEST_LEN] = {
  {0.0f, 10},
  {0.0f, 100},
  {0.0f, 1000},
  {0.0f, 10000},
  {0.0f, 100000},
  {0.0f, 1000000},
};

/// Test cases for `agg_lst` function.
struct test test_lst[TEST_LEN] = {
  {0.0f, 10},
  {0.0f, 100},
  {0.0f, 1000},
  {0.0f, 10000},
  {0.0f, 100000},
  {0.0f, 1000000},
};

/// Compute the first value in the stream given the full stream information.
/// @return success/failure indication
///
/// @param[out] out first value
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
static bool
impl_fst(float* out, const float* arr, const uint64_t len)
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
impl_lst(float* out, const float* arr, const uint64_t len)
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
impl_cnt(float* out, const float* arr, const uint64_t len)
{
  (void)arr;

  *out = (float)len;
  return true;
}

/// Compute the sum of values in the stream given the full stream information.
/// @return always true
///
/// @param[out] out sum of values
/// @param[in]  arr array representing the stream
/// @param[in]  len length of the stream
static bool
impl_sum(float* out, const float* arr, const uint64_t len)
{
  uint64_t idx;
  float sum;

  sum = 0.0f;
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
impl_min(float* out, const float* arr, const uint64_t len)
{
  uint64_t idx;
  float min;

  if (len == 0) {
    return false;
  }

  min = arr[0];
  for (idx = 1; idx < len; idx++) {
    if (arr[idx] < min) {
      min = arr[idx];
    }
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
impl_max(float* out, const float* arr, const uint64_t len)
{
  uint64_t idx;
  float max;

  if (len == 0) {
    return false;
  }

  max = arr[0];
  for (idx = 1; idx < len; idx++) {
    if (arr[idx] > max) {
      max = arr[idx];
    }
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
impl_avg(float* out, const float* arr, const uint64_t len)
{
  float sum;

  if (len == 0) {
    return false;
  }

  (void)impl_sum(&sum, arr, len);
  *out = sum / (float)len;
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
impl_var(float* out, const float* arr, const uint64_t len)
{
  float avg;
  float var;
  uint64_t idx;

  if (len == 0) {
    return false;
  }

  if (len == 1) {
    *out = 0.0f;
    return true;
  }

  (void)impl_avg(&avg, arr, len);
  var = 0.0f;
  for (idx = 0; idx < len; idx++) {
    var += powf(arr[idx] - avg, 2.0f);
  }
  var /= (float)len - 1.0f;

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
impl_dev(float* out, const float* arr, const uint64_t len)
{
  float var;

  if (len == 0) {
    return false;
  }

  (void)impl_var(&var, arr, len);
  *out = sqrtf(var);
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
impl_skw(float* out, const float* arr, const uint64_t len)
{
  uint64_t idx;
  float avg;
  float dev;
  float skw;

  if (len < 2) {
    return false;
  }

  (void)impl_avg(&avg, arr, len);
  (void)impl_dev(&dev, arr, len);

  skw = 0.0;
  for (idx = 0; idx < len; idx++) {
    skw += powf(arr[idx] - avg, 3.0f) / (float)len;
  }
  skw /= powf(dev, 3.0f);

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
impl_krt(float* out, const float* arr, const uint64_t len)
{
  uint64_t idx;
  float avg;
  float dev;
  float krt;

  if (len < 2) {
    return false;
  }

  (void)impl_avg(&avg, arr, len);
  (void)impl_dev(&dev, arr, len);

  krt = 0.0;
  for (idx = 0; idx < len; idx++) {
    krt += powf(arr[idx] - avg, 4.0f) / (float)len;
  }
  krt /= powf(dev, 4.0f);

  *out = krt;
  return true;
}

/// Generate a next random float from the inclusive interval (0.0, 1.0).
/// @return random float
static float
random_float(void)
{
  static uint32_t num = 55;
  uint32_t per; 
  
  per = ((uint32_t)1 << 31) - 1;
  num = (num * 214013 + 2531011) & per;

  return (float)num / (float)per;
}

/// Run the on-line and off-ine 
/// @return success/failure indication
///
/// @param[in] agg  aggregate function
/// @param[in] ts   test case
/// @param[in] impl off-line implementation
static bool
verify(struct agg* agg,
       const struct test* ts,
       bool (*impl)(float*, const float*, const uint64_t))
{
  float*   arr;
  uint64_t run;
  float    val[2];
  float    dif;
  bool     ret[2];


  // Allocate the array.
  arr = malloc(sizeof(*arr) * ts->ts_cnt); 
  if (arr == NULL) {
    printf("allocation error\n");
    return false;
  }

  // Populate the array.
  for (run = 0; run < ts->ts_cnt; run++) {
    arr[run] = random_float();
  }

  // Run the on-line algorithm.
  agg_rst(agg);
  for (run = 0; run < ts->ts_cnt; run++) {
    agg_put(agg, arr[run]);
  }
  ret[0] = agg_get(agg, &val[0]);

  // Run the off-line algorithm.
  ret[1] = impl(&val[1], arr, ts->ts_cnt);

  // Deallocate the array as the actual values will not be needed going
  // forward.
  free(arr);

  // Certify that the functions resulted in the same way.
  if (ret[0] != ret[1]) {
    (void)printf("return mismatch\n  exp = %d, act = %d\n", ret[0], ret[1]);
    return false;
  }

  // Certify that the functions produced an acceptable value within the error
  // margin.
  dif = fabsf(val[0] - val[1]);
  if (dif > ts->ts_err) {
    (void)printf("result mismatch\n"
                 "  value exp = %f, act = %f\n"
                 "  error acc = %f, act = %f\n",
                 val[0], val[1], ts->ts_err, dif);
    return false;
  }

  return true;
}

/// Run a single test multiple times and check the results.
/// @return success/failure indication
///
/// @param[in] name test name
/// @param[in] agg  aggregate function
/// @param[in] ts   test cases
/// @param[in] impl off-line implementation of the same algorithm
bool
run_test(const char* name,
         struct agg* agg,
         const struct test ts[static TEST_LEN],
         bool (*impl)(float*, const float*, const uint64_t))
{
  uint64_t ctr;
  uint64_t idx;
  bool ret;

  (void)printf("%s\n", name);

  // Run the test with various input sizes.
  for (idx = 0; idx < TEST_LEN; idx++) {
    (void)printf("%*" PRIu64 " -> ", 9, ts[idx].ts_cnt);

    // Run each test multiple times to ensure that it satisfies the margin of
    // error under various inputs.
    for (ctr = 0; ctr < TEST_TRY; ctr++) {
      ret  = verify(agg, &ts[idx], impl);
      if (ret == false) {
        return false;
      }
    }
    
    (void)printf("OK\n");
  }

  (void)printf("\n");
  return true;
}

/// The goal of the test suite is to compare the on-line and off-line
/// implementations of the aggregate functions and to determine the error
/// between the two - and crucially - whether that error is of an acceptable
/// magnitude.
int
main(void)
{
  struct agg agg;
  bool fin;
  bool res;

  // By default we assume that the testing will be successful.
  fin = true;

  agg_fst(&agg);
  res = run_test("fst", &agg, test_fst, impl_fst);
  fin = fin && res;

  agg_lst(&agg);
  res = run_test("lst", &agg, test_lst, impl_lst);
  fin = fin && res;

  // Ensure that the process succeeds only and only if all tests passed.
  if (fin == true) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
