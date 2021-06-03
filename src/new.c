// Copyright (c) 2019-2021 Daniel Lovasko
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
/// @param[in] par function parameter
void
aggstat_new(struct aggstat* agg, const uint8_t fnc, const AGGSTAT_FLT par)
{
  // Select aggregate function.
  agg->ag_fnc    = fnc;
  agg->ag_par    = par;

  // Reset number of samples.
  agg->ag_cnt[0] = 0;
  agg->ag_cnt[1] = 0;
  agg->ag_cnt[2] = 0;
  agg->ag_cnt[3] = 0;
  agg->ag_cnt[4] = 0;

  // Reset all state variables.
  agg->ag_val[0] = AGGSTAT_0_0;
  agg->ag_val[1] = AGGSTAT_0_0;
  agg->ag_val[2] = AGGSTAT_0_0;
  agg->ag_val[3] = AGGSTAT_0_0;
  agg->ag_val[4] = AGGSTAT_0_0;
  agg->ag_val[5] = AGGSTAT_0_0;
  agg->ag_val[6] = AGGSTAT_0_0;
  agg->ag_val[7] = AGGSTAT_0_0;
  agg->ag_val[8] = AGGSTAT_0_0;
  agg->ag_val[9] = AGGSTAT_0_0;

  // Override the default value for minimum computation.
  if (fnc == AGGSTAT_FNC_MIN) {
    agg->ag_val[0] = AGGSTAT_MAX;
  }

  // Override the default value for maximum computation.
  if (fnc == AGGSTAT_FNC_MAX) {
    agg->ag_val[0] = AGGSTAT_MIN;
  }

  // Override the parameter for median computation.
  if (fnc == AGGSTAT_FNC_MED) {
    agg->ag_par = AGGSTAT_0_5;
  }
}
