#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <errno.h>
#include <fenv.h>

#include "agg.h"


/// Settings.
struct settings {
  uintmax_t    s_len; ///< Length of the stream.
  uintmax_t    s_rep; ///< Repetitions of the test measurements.
  AGGSTAT_FLT  s_scl; ///< Scale of the values. 
  AGGSTAT_FLT  s_off; ///< Offset of the values.
  AGGSTAT_FLT  s_par; ///< Aggregate function parameter.
  uint8_t      s_fnc; ///< Aggregate function.
};

/// Fill the array with random values.
///
/// @param[in] arr array
/// @param[in] len length of the array
/// @param[in] mul value multiplier
/// @param[in] off value offset
static void
fill_array(      AGGSTAT_FLT* arr,
           const uintmax_t    len,
           const AGGSTAT_FLT  mul,
           const AGGSTAT_FLT  off)
{
  uintmax_t   idx;
  AGGSTAT_FLT val;

  for (idx = 0; idx < len; idx++) {
    val = (AGGSTAT_FLT)rand() / (AGGSTAT_FLT)RAND_MAX * mul + off;
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
static AGGSTAT_FLT
compute_online(AGGSTAT_FLT* arr, const uintmax_t len, const uint8_t fnc, const AGGSTAT_FLT par)
{
  struct aggstat agg;
  uintmax_t      idx;
  AGGSTAT_FLT    ret;

  // Push all values into the aggregate function that computes the estimate.
  aggstat_new(&agg, fnc, par);
  for (idx = 0; idx < len; idx++) {
    aggstat_put(&agg, arr[idx]);
  }

  (void)aggstat_get(&agg, &ret);
  return ret;
}

/// Perform the computation of the aggregated value using the standard algorithms.
/// @return aggregate vale
///
/// @param[in] arr array
/// @param[in] len length of the array
/// @param[in] fnc aggregate function
/// @param[in] par parameter of the aggregate function
static AGGSTAT_FLT
compute_offline(AGGSTAT_FLT* arr, const uintmax_t len, const uint8_t fnc, const AGGSTAT_FLT par)
{
  AGGSTAT_FLT ret;

  (void)aggstat_run(&ret, arr, len, fnc, par);
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

/// Check the state of the floating-point environment for exceptions.
///
/// @param[out] ovr overflow exception
/// @param[out] udr underflow exception
/// @param[out] ine inexact computation exception
static void
read_exceptions(bool* ovr, bool* udr, bool* ine)
{
  int test;
  int flag;

  flag = FE_UNDERFLOW | FE_OVERFLOW | FE_INEXACT;

  test = fetestexcept(flag);
  *udr = *udr || (test & FE_OVERFLOW);
  *ovr = *ovr || (test & FE_UNDERFLOW);
  *ine = *ine || (test & FE_INEXACT);

  feclearexcept(flag);
}

/// Perform repeated comparisons of streaming and standard algorithms and find
/// their largest difference.
///
/// @param[in] arr memory for stream emulation
/// @param[in] stg settings
static void
run_comparisons(AGGSTAT_FLT* arr, struct settings* stg)
{
  uintmax_t   idx;
  AGGSTAT_FLT max;
  AGGSTAT_FLT dif;
  AGGSTAT_FLT val[2];
  bool        ovr[2];
  bool        udr[2];
  bool        ine[2];
  bool        dum;

  // Assume no exceptions.
  ovr[0] = false;
  ovr[1] = false;
  udr[0] = false;
  udr[1] = false;
  ine[0] = false;
  ine[1] = false;
  
  // Repeatedly generate a new array and compute both values.
  max = AGGSTAT_NUM(0, 0, +, 0);
  for (idx = 0; idx < stg->s_rep; idx++) {
    // Prepare the array and clear any exceptions that might have been caused
    // by the random number generation.
    fill_array(arr, stg->s_len, stg->s_scl, stg->s_off);
    read_exceptions(&dum, &dum, &dum);

    // Execute the streaming computation.
    val[0] = compute_online(arr, stg->s_len, stg->s_fnc, stg->s_par);
    read_exceptions(&ovr[0], &udr[0], &ine[0]);
    
    // Execute the standard computation.
    val[1] = compute_offline(arr, stg->s_len, stg->s_fnc, stg->s_par);
    read_exceptions(&ovr[1], &udr[1], &ine[1]);

    // Potentially set the new maximum.
    dif = AGGSTAT_ABS(val[0] - val[1]);
    if (dif > max) {
      max = dif;
    }
  }

  (void)printf(AGGSTAT_FMT "(%s%s%s)(%s%s%s) ",
    max,
    ovr[0] ? "o" : "", udr[0] ? "u" : "", ine[0] ? "i" : "",
    ovr[1] ? "o" : "", udr[1] ? "u" : "", ine[1] ? "i" : "");
}

int
main(int argc, char* argv[])
{
  struct settings stg;
  AGGSTAT_FLT*    arr;
  bool            ret;

  // Parse input from command-line.
  ret = parse_settings(&stg, argc, argv);
  if (ret == false) {
    return EXIT_FAILURE;
  }

  // Initialise the random seed.
  srand(time(NULL));

  // Allocate the array of numbers.
  arr = calloc(sizeof(AGGSTAT_FLT), stg.s_len);
  if (arr == NULL) {
    perror("calloc");
    return EXIT_FAILURE;
  }

  // Find the cap of the error value.
  run_comparisons(arr, &stg);

  return EXIT_SUCCESS;
}
