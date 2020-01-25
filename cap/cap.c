#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>

#include "agg.h"


/// Settings.
struct settings {
  uintmax_t s_len; ///< Length of the stream.
  uintmax_t s_rep; ///< Repetitions of the test measurements.
  AGG_TYPE  s_scl; ///< Scale of the values. 
  AGG_TYPE  s_off; ///< Offset of the values.
  AGG_TYPE  s_par; ///< Aggregate function parameter.
  uint8_t   s_fnc; ///< Aggregate function.
};

/// Fill the array with random values.
///
/// @param[in] arr array
/// @param[in] len length of the array
/// @param[in] mul value multiplier
/// @param[in] off value offset
static void
fill_array(AGG_TYPE* arr, const uintmax_t len, const AGG_TYPE mul, const AGG_TYPE off)
{
  uintmax_t idx;
  AGG_TYPE val;

  for (idx = 0; idx < len; idx++) {
    val = (AGG_TYPE)rand() / (AGG_TYPE)RAND_MAX * mul + off;
    arr[idx] = val;
  }
}

/// Perform the computation of the aggregated value using the streaming algorithms.
/// @return aggregate vale
///
/// @param[in] arr array
/// @param[in] len length of the array
/// @param[in] fnc aggregate function
/// @param[in] par parameter of the aggregate function
static AGG_TYPE
compute_online(AGG_TYPE* arr, const uintmax_t len, const uint8_t fnc, const AGG_TYPE par)
{
  struct agg agg;
  uintmax_t idx;
  AGG_TYPE ret;

  // Push all values into the aggregate function that computes the estimate.
  agg_new(&agg, fnc, par);
  for (idx = 0; idx < len; idx++) {
    agg_put(&agg, arr[idx]);
  }

  (void)agg_get(&agg, &ret);
  return ret;
}

/// Perform the computation of the aggregated value using the standard algorithms.
/// @return aggregate vale
///
/// @param[in] arr array
/// @param[in] len length of the array
/// @param[in] fnc aggregate function
/// @param[in] par parameter of the aggregate function
static AGG_TYPE
compute_offline(AGG_TYPE* arr, const uintmax_t len, const uint8_t fnc, const AGG_TYPE par)
{
  AGG_TYPE ret;

  (void)agg_run(&ret, arr, len, fnc, par);
  return ret;
}

/// Parse the aggregate function from a shortcut.
/// @return aggregate function ID
///
/// @param[in] str input string
static uint8_t
parse_function(const char* str)
{
  int ret;

  ret = strcmp(str, "fst"); if (ret == 0) { return AGG_FNC_FST; }
  ret = strcmp(str, "lst"); if (ret == 0) { return AGG_FNC_LST; }
  ret = strcmp(str, "cnt"); if (ret == 0) { return AGG_FNC_CNT; }
  ret = strcmp(str, "sum"); if (ret == 0) { return AGG_FNC_SUM; }
  ret = strcmp(str, "min"); if (ret == 0) { return AGG_FNC_MIN; }
  ret = strcmp(str, "max"); if (ret == 0) { return AGG_FNC_MAX; }
  ret = strcmp(str, "avg"); if (ret == 0) { return AGG_FNC_AVG; }
  ret = strcmp(str, "var"); if (ret == 0) { return AGG_FNC_VAR; }
  ret = strcmp(str, "dev"); if (ret == 0) { return AGG_FNC_DEV; }
  ret = strcmp(str, "skw"); if (ret == 0) { return AGG_FNC_SKW; }
  ret = strcmp(str, "krt"); if (ret == 0) { return AGG_FNC_KRT; }
  ret = strcmp(str, "qnt"); if (ret == 0) { return AGG_FNC_QNT; }
  ret = strcmp(str, "med"); if (ret == 0) { return AGG_FNC_MED; }

  return 0;
}

/// Parse the settings from the command-line arguments.
/// @return success/failure indication
///
/// @param[in] stg  settings
/// @param[in] argc argument count
/// @param[in] argv argument vector
static bool
parse_settings(struct settings* stg, int argc, char* argv[])
{
  int opt;
  int ret;

  while (true) {
    opt = getopt(argc, argv, "f:l:o:p:r:s:");
    if (opt == -1) {
      break;
    }

    // Aggregate function to test.
    if (opt == 'f') {
      stg->s_fnc = parse_function(optarg);
      if (stg->s_fnc == 0) {
        fprintf(stderr, "unable to parse the function from '%s'\n", optarg);
	return false;
      }
    }

    // Length of the stream of values.
    if (opt == 'l') {
      stg->s_len = strtoumax(optarg, NULL, 10);
      if (stg->s_len == 0 && errno != 0) {
        fprintf(stderr, "unable to parse the stream length from '%s'\n", optarg);
        return false;	
      }
    }

    // Offset of the tested values.
    if (opt == 'o') {
      ret = sscanf(optarg, AGG_FMT, &stg->s_off);
      if (ret != 1) {
        fprintf(stderr, "unable to parse offset from '%s'\n", optarg);
	return false;
      }
    }

    // Parameter of the aggregate function.
    if (opt == 'p') {
      ret = sscanf(optarg, AGG_FMT, &stg->s_par);
      if (ret != 1) {
        fprintf(stderr, "unable to parse parameter from '%s'\n", optarg);
	return false;
      }
    }

    // Number of test repetitions.
    if (opt == 'r') {
      stg->s_rep = strtoumax(optarg, NULL, 10);
      if (stg->s_rep == 0 && errno != 0) {
        fprintf(stderr, "unable to parse the repetition count from '%s'\n", optarg);
	return false;
      }
    }

    // Scale of the tested values.
    if (opt == 's') {
      ret = sscanf(optarg, AGG_FMT, &stg->s_scl);
      if (ret != 1) {
        fprintf(stderr, "unable to parse scale from '%s'\n", optarg);
	return false;
      }
    }

    // Unknown option.
    if (opt == '?') {
      fprintf(stderr, "unknown option '%c'\n", opt);
      return false;
    }
  }

  return true;
}

/// Perform repeated comparisons of streaming and standard algorithms and find
/// their largest difference.
/// @return maximum of differences
///
/// @param[in] arr memory for stream emulation
/// @param[in] stg settings
static AGG_TYPE
run_comparisons(AGG_TYPE* arr, struct settings* stg)
{
  uintmax_t idx;
  AGG_TYPE max;
  AGG_TYPE dif;
  AGG_TYPE onl;
  AGG_TYPE ofl;
  
  // Repeatedly generate a new array and compute both values.
  max = AGG_NUM(0, 0, +, 0);
  for (idx = 0; idx < stg->s_rep; idx++) {
    fill_array(arr, stg->s_len, stg->s_scl, stg->s_off);
    onl = compute_online(arr, stg->s_len, stg->s_fnc, stg->s_par);
    ofl = compute_offline(arr, stg->s_len, stg->s_fnc, stg->s_par);

    // Potentially set the new maximum.
    dif = AGG_ABS(onl - ofl);
    if (dif > max) {
      max = dif;
    }
  }

  return max;
}

int
main(int argc, char* argv[])
{
  struct settings stg;
  AGG_TYPE* arr;
  AGG_TYPE max;
  bool ret;

  // Parse input from command-line.
  ret = parse_settings(&stg, argc, argv);
  if (ret == false) {
    return EXIT_FAILURE;
  }

  // Initialise the random seed.
  srand(time(NULL));

  // Allocate the array of numbers.
  arr = calloc(sizeof(AGG_TYPE), stg.s_len);
  if (arr == NULL) {
    return EXIT_FAILURE;
  }

  // Find the cap of the error value.
  max = run_comparisons(arr, &stg);
  (void)printf(AGG_FMT " ", max);

  return EXIT_SUCCESS;
}
