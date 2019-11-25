// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "../src/agg.h"
#include "err.h"


#define TEST_LEN 6
#define TEST_TRY 100


/// Generate a next random number from the inclusive interval (0.0, 1.0).
/// @return random number
static AGG_TYPE 
random_number(void)
{
  static uint32_t num = 77;
  uint32_t per; 
  
  per = ((uint32_t)1 << 31) - 1;
  num = (num * 214013 + 2531011) & per;

  return (AGG_TYPE)num / (AGG_TYPE)per;
}

/// Run the on-line and off-line algorithms and verify whether they differ
/// the acceptable margin of error.
/// @return success/failure indication
///
/// @param[in] arr array of values
/// @param[in] len length of the array
/// @param[in] fnc aggregate function
/// @param[in] idx test case index
static bool
exec(AGG_TYPE* arr, const uint64_t len, const uint8_t fnc, const uint64_t idx)
{
  struct agg agg;
  uint64_t   run;
  AGG_TYPE   val[2];
  AGG_TYPE   dif;
  bool       ret[2];

  // Populate the array.
  for (run = 0; run < len; run++) {
    arr[run] = random_number();
  }

  // Run the on-line algorithm.
  agg_new(&agg, fnc);
  for (run = 0; run < len; run++) {
    agg_put(&agg, arr[run]);
  }
  ret[0] = agg_get(&agg, &val[0]);

  // Run the off-line algorithm.
  ret[1] = agg_run(&val[1], arr, len, fnc);

  // Certify that the functions resulted in the same way.
  if (ret[0] != ret[1]) {
    (void)printf("\e[31mfail\e[0m\n  exp = %d, act = %d\n", ret[1], ret[0]);
    return false;
  }

  // Certify that the functions produced an acceptable value within the error
  // margin.
  dif = AGG_ABS(val[0] - val[1]);
  if (dif > err[fnc - 1][idx]) {
    (void)printf("\e[31mfail\e[0m\n"
                 "  value exp = " AGG_FMT ", act = " AGG_FMT "\n"
                 "  error acc = " AGG_FMT ", act = " AGG_FMT "\n",
                 val[1], val[0], err[fnc - 1][idx], dif);
    return false;
  }

  return true;
}

/// Run a single test multiple times and check the results.
///
/// @param[out] res  result
/// @param[in]  fnc  aggregate function
static void
test(bool* res, const uint8_t fnc)
{
  AGG_TYPE* arr;
  uint64_t  ctr;
  uint64_t  len;
  uint64_t  idx;
  bool      ret;

  // Run the test with various input sizes.
  len = 10;
  for (idx = 0; idx < TEST_LEN; idx++) {
    (void)printf("%*" PRIu64 " -> ", 9, len);

    // Allocate the array.
    arr = malloc(sizeof(*arr) * len); 
    if (arr == NULL) {
      perror("allocation error\n");
      return;
    }

    // Run each test multiple times to ensure that it satisfies the margin of
    // error under various inputs.
    for (ctr = 0; ctr < TEST_TRY; ctr++) {
      ret = exec(arr, len, fnc, idx);
      if (ret == false) {
        printf("\n");
        *res = *res && ret;
        return;
      }
    }
    
    (void)printf("\e[32mokay\e[0m\n");
    free(arr);

    // Increase the array length.
    len = len * 10;
  }

  (void)printf("\n");
}

/// The goal of the test suite is to compare the on-line and off-line
/// implementations of the aggregate functions and to determine the error
/// between the two - and crucially - whether that error is of an acceptable
/// magnitude.
int
main(void)
{
  bool res;

  res = true;

  (void)printf("fst\n");
  test(&res, AGG_FNC_FST);

  (void)printf("lst\n");
  test(&res, AGG_FNC_LST);

  (void)printf("cnt\n");
  test(&res, AGG_FNC_CNT);

  (void)printf("sum\n");
  test(&res, AGG_FNC_SUM);

  (void)printf("min\n");
  test(&res, AGG_FNC_MIN);

  (void)printf("max\n");
  test(&res, AGG_FNC_MAX);

  (void)printf("avg\n");
  test(&res, AGG_FNC_AVG);

  (void)printf("var\n");
  test(&res, AGG_FNC_VAR);

  (void)printf("dev\n");
  test(&res, AGG_FNC_DEV);

  (void)printf("skw\n");
  test(&res, AGG_FNC_SKW);

  (void)printf("krt\n");
  test(&res, AGG_FNC_KRT);

  // Ensure that the process succeeds only and only if all tests passed.
  if (res == true) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
