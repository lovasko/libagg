// Copyright (c) 2019-2020 Daniel Lovasko
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
#include <time.h>

#include "../src/agg.h"
#include "err.h"


#define TEST_LEN 6
#define TEST_TRY 100

// Define a type-correct constant for one hundred.
#ifdef AGG_FLT
  #define AGG_10_0 10.0f
#else
  #define AGG_10_0 10.0
#endif


/// Generate a next random number from the inclusive interval (0.0, 1.0).
/// @return random number
static AGG_TYPE
random_number(void)
{
  static uint32_t num = 77;
  uint32_t per;

  per = ((uint32_t)1 << 31) - 1;
  num = (num * 214013 + 2531011) & per;

  return (AGG_TYPE)num / (AGG_TYPE)per * AGG_10_0;
}

/// Obtain the current time with minimal external effects.
/// @return nanoseconds
static uint64_t
time_now(void)
{
  struct timespec ts;

  (void)clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec * 1000000000ULL + ts.tv_nsec;
}

/// Run the on-line and off-line algorithms and verify whether they differ
/// the acceptable margin of error.
/// @return success/failure indication
///
/// @param[out] onc on-line method clock
/// @param[out] ofc off-line method clock
/// @param[in]  arr array of values
/// @param[in]  len length of the array
/// @param[in]  fnc aggregate function
/// @param[in]  idx test case index
/// @param[in]  par function parameter
static bool
exec(uint64_t* onc,
     uint64_t* ofc,
     AGG_TYPE* arr,
     const uint64_t len,
     const uint8_t fnc,
     const uint64_t idx,
     const AGG_TYPE par)
{
  struct agg agg;
  uint64_t   run;
  AGG_TYPE   val[2];
  AGG_TYPE   dif;
  bool       ret[2];
  uint64_t   now[3];

  // Populate the array.
  for (run = 0; run < len; run++) {
    arr[run] = random_number();
  }


  // Run the on-line algorithm.
  now[0] = time_now();
  agg_new(&agg, fnc, par);
  for (run = 0; run < len; run++) {
    agg_put(&agg, arr[run]);
  }
  ret[0] = agg_get(&agg, &val[0]);
  now[1] = time_now();

  // Run the off-line algorithm.
  ret[1] = agg_run(&val[1], arr, len, fnc, par);
  now[2] = time_now();

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

  *onc += (uint64_t)(now[1] - now[0]);
  *ofc += (uint64_t)(now[2] - now[1]);

  return true;
}

/// Run a single test multiple times and check the results.
///
/// @param[out] res  result
/// @param[in]  fnc  aggregate function
/// @param[in]  par  parameter
static void
test(bool* res, const uint8_t fnc, const AGG_TYPE par)
{
  AGG_TYPE* arr;
  uint64_t  ctr;
  uint64_t  len;
  uint64_t  idx;
  bool      ret;
  uint64_t  onc;
  uint64_t  ofc;

  // Reset the clocks.
  onc = 0;
  ofc = 0;

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
      ret = exec(&onc, &ofc, arr, len, fnc, idx, par);
      if (ret == false) {
        printf("\n");
        *res = *res && ret;
        return;
      }
    }

    free(arr);

    // Report success and elapsed times.
    (void)printf("\e[32mokay\e[0m");
    (void)printf(" (on = %12" PRIu64 "ns total, %4" PRIu64 "ns avg ",   onc, onc / len / TEST_TRY);
    (void)printf("| of = %12" PRIu64 "ns total, %4" PRIu64 "ns avg)\n", ofc, ofc / len / TEST_TRY);

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
  test(&res, AGG_FNC_FST, AGG_0_0);

  (void)printf("lst\n");
  test(&res, AGG_FNC_LST, AGG_0_0);

  (void)printf("cnt\n");
  test(&res, AGG_FNC_CNT, AGG_0_0);

  (void)printf("sum\n");
  test(&res, AGG_FNC_SUM, AGG_0_0);

  (void)printf("min\n");
  test(&res, AGG_FNC_MIN, AGG_0_0);

  (void)printf("max\n");
  test(&res, AGG_FNC_MAX, AGG_0_0);

  (void)printf("avg\n");
  test(&res, AGG_FNC_AVG, AGG_0_0);

  (void)printf("var\n");
  test(&res, AGG_FNC_VAR, AGG_0_0);

  (void)printf("dev\n");
  test(&res, AGG_FNC_DEV, AGG_0_0);

  (void)printf("skw\n");
  test(&res, AGG_FNC_SKW, AGG_0_0);

  (void)printf("krt\n");
  test(&res, AGG_FNC_KRT, AGG_0_0);

  (void)printf("qnt(0.1)\n");
  test(&res, AGG_FNC_QNT, AGG_0_1);

  (void)printf("qnt(0.75)\n");
  test(&res, AGG_FNC_QNT, AGG_0_75);

  (void)printf("qnt(0.9)\n");
  test(&res, AGG_FNC_QNT, AGG_0_9);

  (void)printf("qnt(0.99)\n");
  test(&res, AGG_FNC_QNT, AGG_0_99);

  (void)printf("med\n");
  test(&res, AGG_FNC_MED, AGG_0_0);

  // Ensure that the process succeeds only and only if all tests passed.
  if (res == true) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
