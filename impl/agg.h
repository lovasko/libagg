#ifndef AGG_H
#define AGG_H

#include <stdbool.h>
#include <stdint.h>


/// Aggregate function. 
struct agg {
  uint8_t  ag_typ;    ///< Type.
  uint8_t  ag_pad[7]; ///< Padding (unused).
  uint64_t ag_cnt;    ///< Number of observations.
  float    ag_val[4]; ///< State variables.
};

/// Start the aggregate.
void agg_fst(struct agg* agg);
void agg_lst(struct agg* agg);
void agg_cnt(struct agg* agg);
void agg_sum(struct agg* agg);
void agg_min(struct agg* agg);
void agg_max(struct agg* agg);
void agg_avg(struct agg* agg);
void agg_var(struct agg* agg);
void agg_dev(struct agg* agg);
void agg_skw(struct agg* agg);
void agg_krt(struct agg* agg);

/// Reset the aggregate.
void agg_rst(struct agg* agg);

/// Update the aggregate.
void agg_put(struct agg* agg, const float inp);

/// Obtain the aggregate.
bool agg_get(const struct agg* agg, float* out);

#endif
