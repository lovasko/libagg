// Copyright (c) 2019 Daniel Lovasko
// All Rights Reserved
//
// Distributed under the terms of the 2-clause BSD License. The full
// license is in the file LICENSE, distributed as part of this software.

#include "agg.h"


/// Initialise the aggregate function.
/// @return success/failure indication
///
/// @param[in] agg aggregate function
/// @param[in] fnc function type
bool
agg_new(struct agg* agg, const uint8_t fnc)
{
  // Verify that the chosen function is valid.
  if (fnc > AGG_FNC_KRT) {
    return false;
  }

  // Reset number of samples.
  agg->ag_cnt    = 0;

  // Reset all state variables.
  agg->ag_val[0] = AGG_0_0;
  agg->ag_val[1] = AGG_0_0;
  agg->ag_val[2] = AGG_0_0;
  agg->ag_val[3] = AGG_0_0;

  // Reset all temporary variables.
  agg->ag_tmp[0] = AGG_0_0;
  agg->ag_tmp[1] = AGG_0_0;
  agg->ag_tmp[2] = AGG_0_0;
  agg->ag_tmp[3] = AGG_0_0;

  // Select aggregate function.
  if (fnc != 0) {
    agg->ag_fnc  = fnc;
  }

  // Ensure the appropriate main value. This is done due to the use of
  // fmin/fmax function in the respective functions.
  if (fnc == AGG_FNC_MIN) {
    agg->ag_val[0] = AGG_MAX;
  }

  if (fnc == AGG_FNC_MAX) {
    agg->ag_val[0] = AGG_MIN;
  }

  return true;
}
