// Copyright (c) 2019-2021 Daniel Lovasko
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

// Define a type-correct constant for ten.
#define AGGSTAT_10_0 AGGSTAT_NUM(10, 0, +, 0)

/// Generate a next random number from the inclusive interval (0.0, 1.0).
/// @return random number
static AGGSTAT_TYPE
random_number(void)
{
  static uint32_t num = 77;
  uint32_t per;

  per = ((uint32_t)1 << 31) - 1;
  num = (num * 214013 + 2531011) & per;

  return (AGGSTAT_TYPE)num / (AGGSTAT_TYPE)per * AGGSTAT_10_0;
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
exec(      uint64_t*     onc,
           uint64_t*     ofc,
           AGGSTAT_TYPE* arr,
     const uint64_t      len,
     const uint8_t       fnc,
     const uint64_t      idx,
     const AGGSTAT_TYPE  par)
{
  struct aggstat agg;
  uint64_t       run;
  AGGSTAT_TYPE   val[2];
  AGGSTAT_TYPE   dif;
  bool           ret[2];
  uint64_t       now[3];

  // Populate the array.
  for (run = 0; run < len; run++) {
    arr[run] = random_number();
  }

  // Run the on-line algorithm.
  now[0] = time_now();
  aggstat_new(&agg, fnc, par);
  for (run = 0; run < len; run++) {
    aggstat_put(&agg, arr[run]);
  }
  ret[0] = aggstat_get(&agg, &val[0]);
  now[1] = time_now();

  // Run the off-line algorithm.
  ret[1] = aggstat_run(&val[1], arr, len, fnc, par);
  now[2] = time_now();

  // Certify that the functions resulted in the same way.
  if (ret[0] != ret[1]) {
    (void)printf("\e[31mfail\e[0m\n  exp = %d, act = %d\n", ret[1], ret[0]);
    return false;
  }

  // Certify that the functions produced an acceptable value within the error
  // margin.
  dif = AGGSTAT_ABS(val[0] - val[1]);
  if (dif > err[fnc - 1][idx]) {
    (void)printf("\e[31mfail\e[0m\n"
                 "  value exp = " AGGSTAT_FMT ", act = " AGGSTAT_FMT "\n"
                 "  error acc = " AGGSTAT_FMT ", act = " AGGSTAT_FMT "\n",
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
test(bool* res, const uint8_t fnc, const AGGSTAT_TYPE par)
{
  AGGSTAT_TYPE* arr;
  uint64_t      ctr;
  uint64_t      len;
  uint64_t      idx;
  bool          ret;
  uint64_t      onc;
  uint64_t      ofc;

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
  test(&res, AGGSTAT_FNC_FST, AGGSTAT_0_0);

  (void)printf("lst\n");
  test(&res, AGGSTAT_FNC_LST, AGGSTAT_0_0);

  (void)printf("cnt\n");
  test(&res, AGGSTAT_FNC_CNT, AGGSTAT_0_0);

  (void)printf("sum\n");
  test(&res, AGGSTAT_FNC_SUM, AGGSTAT_0_0);

  (void)printf("min\n");
  test(&res, AGGSTAT_FNC_MIN, AGGSTAT_0_0);

  (void)printf("max\n");
  test(&res, AGGSTAT_FNC_MAX, AGGSTAT_0_0);

  (void)printf("avg\n");
  test(&res, AGGSTAT_FNC_AVG, AGGSTAT_0_0);

  (void)printf("var\n");
  test(&res, AGGSTAT_FNC_VAR, AGGSTAT_0_0);

  (void)printf("dev\n");
  test(&res, AGGSTAT_FNC_DEV, AGGSTAT_0_0);

  (void)printf("skw\n");
  test(&res, AGGSTAT_FNC_SKW, AGGSTAT_0_0);

  (void)printf("krt\n");
  test(&res, AGGSTAT_FNC_KRT, AGGSTAT_0_0);

  (void)printf("qnt(0.1)\n");
  test(&res, AGGSTAT_FNC_QNT, AGGSTAT_0_1);

  (void)printf("qnt(0.75)\n");
  test(&res, AGGSTAT_FNC_QNT, AGGSTAT_0_75);

  (void)printf("qnt(0.9)\n");
  test(&res, AGGSTAT_FNC_QNT, AGGSTAT_0_9);

  (void)printf("qnt(0.99)\n");
  test(&res, AGGSTAT_FNC_QNT, AGGSTAT_0_99);

  (void)printf("med\n");
  test(&res, AGGSTAT_FNC_MED, AGGSTAT_0_0);

  // Ensure that the process succeeds only and only if all tests passed.
  if (res == true) {
    return EXIT_SUCCESS;
  } else {
    return EXIT_FAILURE;
  }
}
