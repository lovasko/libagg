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
/// @param[in] par function parameter
void
agg_new(struct agg* agg, const uint8_t fnc, const AGG_TYPE par)
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
  agg->ag_val[0] = AGG_0_0;
  agg->ag_val[1] = AGG_0_0;
  agg->ag_val[2] = AGG_0_0;
  agg->ag_val[3] = AGG_0_0;
  agg->ag_val[4] = AGG_MAX; // For use in `put_min`.
  agg->ag_val[5] = AGG_MIN; // For use in `put_max`.
  agg->ag_val[6] = AGG_0_0;
  agg->ag_val[7] = AGG_0_0;
  agg->ag_val[8] = AGG_0_0;
  agg->ag_val[9] = AGG_0_0;

  // Override the parameter for median computation.
  if (fnc == AGG_FNC_MED) {
    agg->ag_par = AGG_0_5;
  }
}
