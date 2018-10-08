/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "consensus/exit_probability_validator/exit_probability_validator.h"
#include "utarray.h"
#include "utils/logger_helper.h"

#define WALKER_VALIDATOR_LOGGER_ID "consensus_walker_validator"

static flex_trit_t null_hash[FLEX_TRIT_SIZE_243];

retcode_t iota_consensus_exit_prob_transaction_validator_init(
    tangle_t *const tangle, milestone_tracker_t *const mt,
    ledger_validator_t *const lv, exit_prob_transaction_validator_t *epv,
    uint32_t max_analyzed_txs, uint32_t max_depth) {
  logger_helper_init(WALKER_VALIDATOR_LOGGER_ID, LOGGER_DEBUG, true);
  epv->tangle = tangle;
  epv->mt = mt;
  epv->lv = lv;
  epv->max_analyzed_txs = max_analyzed_txs;
  epv->max_depth = max_depth;
  epv->max_depth_ok_memoization = NULL;
  state_map_t diff = NULL;
  hash_set_t analyzed_hashes = NULL;
  memset(null_hash, FLEX_TRIT_NULL_VALUE, FLEX_TRIT_SIZE_243);
  return RC_OK;
}

retcode_t iota_consensus_exit_prob_transaction_validator_destroy(
    exit_prob_transaction_validator_t *epv) {
  logger_helper_destroy(WALKER_VALIDATOR_LOGGER_ID);

  hash_set_free(&epv->max_depth_ok_memoization);
  hash_set_free(&epv->analyzed_hashes);
  iota_snapshot_state_destroy(&epv->diff);
  epv->diff = NULL;
  epv->tangle = NULL;
  epv->mt = NULL;
  epv->lv = NULL;

  return RC_OK;
}

retcode_t iota_consensus_exit_prob_transaction_validator_is_valid(
    exit_prob_transaction_validator_t *epv, trit_array_p tail_hash,
    bool *is_valid) {
  retcode_t ret = RC_OK;
  DECLARE_PACK_SINGLE_TX(tx, tx_models, tx_pack);

  ret = iota_tangle_transaction_load(epv->tangle, TRANSACTION_COL_HASH,
                                     tail_hash, &tx_pack);
  if (ret != RC_OK) {
    *is_valid = false;
    return ret;
  }

  if (tx_pack.num_loaded == 0) {
    *is_valid = false;
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, transaction is missing in db\n");
    return RC_OK;
  }

  if (tx.current_index != 0) {
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, transaction is not a tail\n");
    *is_valid = false;
    return RC_OK;
  }

  ret = iota_consensus_ledger_validator_update_diff(
      epv->lv, &epv->analyzed_hashes, &epv->diff, tail_hash->trits, is_valid);
  if (ret != RC_OK) {
    *is_valid = false;
    return ret;
  }
  if (!*is_valid) {
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, tail is inconsistent\n");
    return RC_OK;
  }

  bool below_max_depth = false;

  // TODO - lowest_allowed_depth
  ret = iota_consensus_exit_prob_transaction_validator_below_max_depth(
      epv, tail_hash, epv->max_depth, &below_max_depth);
  if (ret != RC_OK) {
    return ret;
  }

  if (below_max_depth) {
    *is_valid = false;
    log_error(WALKER_VALIDATOR_LOGGER_ID,
              "Validation failed, tail is below max depth\n");
    return RC_OK;
  }

  // TODO - change schema and update solidity
  /*TransactionViewModel transactionViewModel =
  TransactionViewModel.fromHash(tangle, transactionHash); if
  if (!transactionViewModel.isSolid()) {
    log.debug("Validation failed: {} is not solid", transactionHash);
    return false;
  }*/

  *is_valid = true;

  return ret;
}

retcode_t iota_consensus_exit_prob_transaction_validator_below_max_depth(
    exit_prob_transaction_validator_t *epv, trit_array_p tail_hash,
    uint32_t lowest_allowed_depth, bool *below_max_depth) {
  retcode_t res = RC_OK;
  flex_trit_t curr_hash_trits[FLEX_TRIT_SIZE_243];

  hash_queue_t non_analyzed_hashes = NULL;
  hash_queue_push(&non_analyzed_hashes, tail_hash->trits);

  // Load the transaction
  struct _iota_transaction curr_tx_s;
  iota_transaction_t curr_tx = &curr_tx_s;

  iota_stor_pack_t pack = {.models = (void **)(&curr_tx),
                           .capacity = 1,
                           .num_loaded = 0,
                           .insufficient_capacity = false};

  hash_set_t visited_hashes = NULL;
  while (non_analyzed_hashes != NULL) {
    if (hash_set_size(&visited_hashes) == epv->max_analyzed_txs) {
      log_error(WALKER_VALIDATOR_LOGGER_ID,
                "Validation failed, exceeded num of transactions\n");
      *below_max_depth = false;
      break;
    }

    flex_trit_t *curr_hash_trits = hash_queue_peek(non_analyzed_hashes);
    if (hash_set_contains(&visited_hashes, curr_hash_trits)) {
      continue;
    }

    // Mark the transaction as visited
    if ((res = hash_set_add(&visited_hashes, curr_hash_trits))) {
      break;
    }

    TRIT_ARRAY_DECLARE(hash_trits_array, NUM_TRITS_HASH);
    memcpy(hash_trits_array.trits, curr_hash_trits, FLEX_TRIT_SIZE_243);
    res = iota_tangle_transaction_load(epv->tangle, TRANSACTION_COL_HASH,
                                       &hash_trits_array, &pack);
    bool tail_is_not_genesis =
        (curr_tx_s.snapshot_index != 0 ||
         memcmp(null_hash, curr_tx_s.hash, FLEX_TRIT_SIZE_243) == 0);
    if (tail_is_not_genesis &&
        (curr_tx_s.snapshot_index < lowest_allowed_depth)) {
      log_error(WALKER_VALIDATOR_LOGGER_ID,
                "Validation failed, transaction is below max depth\n");
      *below_max_depth = true;
      break;
    }
    if (curr_tx->snapshot_index == 0) {
      hash_queue_push(&non_analyzed_hashes, curr_tx->trunk);
      hash_queue_push(&non_analyzed_hashes, curr_tx->branch);
    }
    hash_queue_pop(&non_analyzed_hashes);
  }

  hash_queue_free(&non_analyzed_hashes);
  hash_set_free(&visited_hashes);
  hash_set_add(&epv->max_depth_ok_memoization, tail_hash->trits);

  return res;
}
