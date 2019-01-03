/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_DFS_IMPL_H__
#define __CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_DFS_IMPL_H__

#include "utarray.h"
#include "uthash.h"

#include "consensus/cw_rating_calculator/cw_rating_calculator.h"

#ifdef __cplusplus
extern "C" {
#endif

void init_cw_calculator_dfs(cw_rating_calculator_base_t *calculator);
/**
 *
 * @param cw_calc - the calculator
 * @param entry_point  - where should the rating calculation start from
 * @param out - a struct containing the ratings and mapping between txs and
 *              their approvers - both should be freed!!!
 * @return retcode_t
 *
 * This implementation does a DFS from entry point to discover all transactions
 * this first DFS is done using storage to load each transaction approvers, and
 * we use this DFS to store transactions loaded from storage in a map (in
 * memory) then, for each transaction, another DFS is performed to discover it's
 * approvers using the map we got from initial DFS Complexity: DFS + (num
 * vertices)*DFS = (E+V) + V*(E+V) ~ V*(E+V)~O(V^2)
 *
 * (E ~ 2*V - because each transaction has two outcoming edges)
 */

extern retcode_t cw_rating_calculate_dfs(
    cw_rating_calculator_t const *const cw_calc, tangle_t *const tangle,
    flex_trit_t *entry_point, cw_calc_result *out);

extern retcode_t cw_rating_calculate_dfs_ratings_from_approvers_map(
    size_t max_subtangle_size,
    hash_to_indexed_hash_set_map_t const tx_to_approvers,
    hash_to_int64_t_map_t *const cw_ratings, bool skip_entry_point_rating);

extern retcode_t cw_rating_dfs_do_dfs_from_db(
    cw_rating_calculator_t const *const cw_calc, tangle_t *const tangle,
    flex_trit_t *entry_point, hash_to_indexed_hash_set_map_t *tx_to_approvers,
    uint64_t *subtangle_size, int64_t subtangle_before_timestamp);

static cw_calculator_vtable cw_topological_vtable = {
    .cw_rating_calculate = cw_rating_calculate_dfs,
};

#ifdef __cplusplus
}
#endif

#endif  //__CONSENSUS_CW_RATING_CALCULATOR_CW_RATING_DFS_IMPL_H__
