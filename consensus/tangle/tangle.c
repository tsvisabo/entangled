/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <inttypes.h>

#include "consensus/tangle/tangle.h"
#include "utils/logger_helper.h"

#define TANGLE_LOGGER_ID "consensus_tangle"

retcode_t iota_tangle_init(tangle_t *const tangle,
                           connection_config_t const *const conf) {
  logger_helper_init(TANGLE_LOGGER_ID, LOGGER_DEBUG, true);
  return iota_stor_init(&tangle->conn, conf);
}
retcode_t iota_tangle_destroy(tangle_t *const tangle) {
  logger_helper_destroy(TANGLE_LOGGER_ID);
  return iota_stor_destroy(&tangle->conn);
}

/*
 * Transaction operations
 */

retcode_t iota_tangle_transaction_count(tangle_t const *const tangle,
                                        size_t *const count) {
  return iota_stor_transaction_count(&tangle->conn, count);
}

retcode_t iota_tangle_transaction_store(tangle_t const *const tangle,
                                        iota_transaction_t const tx) {
  return iota_stor_transaction_store(&tangle->conn, tx);
}

retcode_t iota_tangle_transaction_load(tangle_t const *const tangle,
                                       transaction_field_t const field,
                                       flex_trit_t const *const key,
                                       iota_stor_pack_t *const tx) {
  return iota_stor_transaction_load(&tangle->conn, field, key, tx);
}

retcode_t iota_tangle_transaction_update_solid_state(
    tangle_t const *const tangle, flex_trit_t const *const hash,
    bool const state) {
  return iota_stor_transaction_update_solid_state(&tangle->conn, hash, state);
}

retcode_t iota_tangle_transactions_update_solid_state(
    tangle_t const *const tangle, hash243_set_t const hashes,
    bool const is_solid) {
  return iota_stor_transactions_update_solid_state(&tangle->conn, hashes,
                                                   is_solid);
}

retcode_t iota_tangle_transaction_load_hashes_of_approvers(
    tangle_t const *const tangle, flex_trit_t const *const approvee_hash,
    iota_stor_pack_t *const pack) {
  retcode_t res = RC_OK;

  res = iota_stor_transaction_load_hashes_of_approvers(&tangle->conn,
                                                       approvee_hash, pack);

  while (res == RC_OK && pack->insufficient_capacity) {
    res = hash_pack_resize(pack, 2);
    if (res == RC_OK) {
      pack->num_loaded = 0;
      res = iota_stor_transaction_load_hashes_of_approvers(&tangle->conn,
                                                           approvee_hash, pack);
    }
  }

  if (res != RC_OK) {
    log_error(TANGLE_LOGGER_ID,
              "Failed in loading approvers, error code is: %" PRIu64 "\n", res);
  }

  return res;
}

retcode_t iota_tangle_transaction_load_partial(
    tangle_t const *const tangle, flex_trit_t const *const hash,
    iota_stor_pack_t *const pack, partial_transaction_model_e models_mask) {
  if (models_mask == PARTIAL_TX_MODEL_METADATA) {
    return iota_stor_transaction_load_metadata(&tangle->conn, hash, pack);
  } else if (models_mask == PARTIAL_TX_MODEL_ESSENCE_ATTACHMENT_METADATA) {
    return iota_stor_transaction_load_essence_attachment_and_metadata(
        &tangle->conn, hash, pack);
  } else if (models_mask == PARTIAL_TX_MODEL_ESSENCE_CONSENSUS) {
    return iota_stor_transaction_load_essence_and_consensus(&tangle->conn, hash,
                                                            pack);
  } else {
    return RC_CONSENSUS_NOT_IMPLEMENTED;
  }
}

retcode_t iota_tangle_transaction_load_hashes_of_requests(
    tangle_t const *const tangle, iota_stor_pack_t *const pack,
    size_t const limit) {
  retcode_t res = RC_OK;

  res =
      iota_stor_transaction_load_hashes_of_requests(&tangle->conn, pack, limit);

  while (res == RC_OK && pack->insufficient_capacity) {
    if ((res = hash_pack_resize(pack, 2)) == RC_OK) {
      pack->num_loaded = 0;
      res = iota_stor_transaction_load_hashes_of_requests(&tangle->conn, pack,
                                                          limit);
    }
  }

  if (res != RC_OK) {
    log_error(TANGLE_LOGGER_ID,
              "Failed in loading hash requests, error code is: %" PRIu64 "\n",
              res);
  }

  return res;
}

retcode_t iota_tangle_transaction_load_hashes_of_tips(
    tangle_t const *const tangle, iota_stor_pack_t *const pack,
    size_t const limit) {
  retcode_t res = RC_OK;

  res = iota_stor_transaction_load_hashes_of_tips(&tangle->conn, pack, limit);

  while (res == RC_OK && pack->insufficient_capacity) {
    if ((res = hash_pack_resize(pack, 2)) == RC_OK) {
      pack->num_loaded = 0;
      res =
          iota_stor_transaction_load_hashes_of_tips(&tangle->conn, pack, limit);
    }
  }

  if (res != RC_OK) {
    log_error(TANGLE_LOGGER_ID,
              "Failed in loading hashes of tips, error code is: %" PRIu64 "\n",
              res);
  }

  return res;
}

retcode_t iota_tangle_transaction_load_hashes_of_milestone_candidates(
    tangle_t const *const tangle, iota_stor_pack_t *const pack,
    flex_trit_t const *const coordinator) {
  retcode_t res = RC_OK;

  res = iota_stor_transaction_load_hashes_of_milestone_candidates(
      &tangle->conn, pack, coordinator);

  while (res == RC_OK && pack->insufficient_capacity) {
    if ((res = hash_pack_resize(pack, 2)) == RC_OK) {
      pack->num_loaded = 0;
      res = iota_stor_transaction_load_hashes_of_milestone_candidates(
          &tangle->conn, pack, coordinator);
    }
  }

  if (res != RC_OK) {
    log_error(TANGLE_LOGGER_ID,
              "Failed in loading hashes of milestone candidates, error code "
              "is: %" PRIu64 "\n",
              res);
  }

  return res;
}

retcode_t iota_tangle_transaction_update_snapshot_index(
    tangle_t const *const tangle, flex_trit_t const *const hash,
    uint64_t const snapshot_index) {
  return iota_stor_transaction_update_snapshot_index(&tangle->conn, hash,
                                                     snapshot_index);
}

retcode_t iota_tangle_transactions_update_snapshot_index(
    tangle_t const *const tangle, hash243_set_t const hashes,
    uint64_t const snapshot_index) {
  return iota_stor_transactions_update_snapshot_index(&tangle->conn, hashes,
                                                      snapshot_index);
}

retcode_t iota_tangle_transaction_exist(tangle_t const *const tangle,
                                        transaction_field_t const field,
                                        flex_trit_t const *const key,
                                        bool *const exist) {
  return iota_stor_transaction_exist(&tangle->conn, field, key, exist);
}

retcode_t iota_tangle_transaction_approvers_count(tangle_t const *const tangle,
                                                  flex_trit_t const *const hash,
                                                  size_t *const count) {
  return iota_stor_transaction_approvers_count(&tangle->conn, hash, count);
}

retcode_t iota_tangle_transaction_find(tangle_t const *const tangle,
                                       hash243_queue_t const bundles,
                                       hash243_queue_t const addresses,
                                       hash81_queue_t const tags,
                                       hash243_queue_t const approvees,
                                       iota_stor_pack_t *const pack) {
  return iota_stor_transaction_find(&tangle->conn, bundles, addresses, tags,
                                    approvees, pack);
}

/*
 * Milestone operations
 */

retcode_t iota_tangle_milestone_store(tangle_t const *const tangle,
                                      iota_milestone_t const *const data_in) {
  return iota_stor_milestone_store(&tangle->conn, data_in);
}

retcode_t iota_tangle_milestone_load(tangle_t const *const tangle,
                                     flex_trit_t const *const hash,
                                     iota_stor_pack_t *const pack) {
  return iota_stor_milestone_load(&tangle->conn, hash, pack);
}

retcode_t iota_tangle_milestone_load_first(tangle_t const *const tangle,
                                           iota_stor_pack_t *const pack) {
  return iota_stor_milestone_load_first(&tangle->conn, pack);
}

retcode_t iota_tangle_milestone_load_last(tangle_t const *const tangle,
                                          iota_stor_pack_t *const pack) {
  return iota_stor_milestone_load_last(&tangle->conn, pack);
}

retcode_t iota_tangle_milestone_load_next(tangle_t const *const tangle,
                                          uint64_t const index,
                                          iota_stor_pack_t *const pack) {
  return iota_stor_milestone_load_next(&tangle->conn, index, pack);
}

retcode_t iota_tangle_milestone_exist(tangle_t const *const tangle,
                                      flex_trit_t const *const hash,
                                      bool *const exist) {
  return iota_stor_milestone_exist(&tangle->conn, hash, exist);
}

/*
 * Utilities
 */

retcode_t iota_tangle_find_tail(tangle_t const *const tangle,
                                flex_trit_t const *const tx_hash,
                                flex_trit_t *const tail,
                                bool *const found_tail) {
  retcode_t res = RC_OK;
  struct _iota_transaction next_tx_s;
  iota_transaction_t next_tx = &next_tx_s;
  flex_trit_t bundle_hash[FLEX_TRIT_SIZE_243];
  bool found_approver = false;
  DECLARE_PACK_SINGLE_TX(curr_tx_s, curr_tx, tx_pack);

  *found_tail = false;

  res = iota_tangle_transaction_load_partial(
      tangle, tx_hash, &tx_pack, PARTIAL_TX_MODEL_ESSENCE_CONSENSUS);
  if (res != RC_OK || tx_pack.num_loaded == 0) {
    return res;
  }

  uint32_t index = transaction_current_index(curr_tx);
  memcpy(bundle_hash, transaction_bundle(curr_tx), FLEX_TRIT_SIZE_243);

  iota_stor_pack_t hash_pack;
  if ((res = hash_pack_init(&hash_pack, 10)) != RC_OK) {
    return res;
  }

  while (res == RC_OK && index > 0 &&
         memcmp(transaction_bundle(curr_tx), bundle_hash, FLEX_TRIT_SIZE_243) ==
             0) {
    hash_pack_reset(&hash_pack);
    if ((res = iota_tangle_transaction_load_hashes_of_approvers(
             tangle, transaction_hash(curr_tx), &hash_pack)) != RC_OK) {
      break;
    }

    --index;
    uint32_t approver_idx = 0;
    found_approver = false;
    while (approver_idx < hash_pack.num_loaded) {
      flex_trit_t *approver_hash =
          (flex_trit_t *)hash_pack.models[approver_idx];
      tx_pack.models = (void **)(&next_tx);
      hash_pack_reset(&tx_pack);
      res = iota_tangle_transaction_load_partial(
          tangle, approver_hash, &tx_pack, PARTIAL_TX_MODEL_ESSENCE_CONSENSUS);
      if (res != RC_OK || tx_pack.num_loaded == 0) {
        break;
      }
      if (transaction_current_index(next_tx) == index &&
          memcmp(transaction_bundle(next_tx), bundle_hash,
                 FLEX_TRIT_SIZE_243) == 0) {
        curr_tx = next_tx;
        found_approver = true;
        break;
      }
    }

    if (!found_approver) {
      break;
    }
  }

  if (transaction_current_index(curr_tx) == 0) {
    memcpy(tail, transaction_hash(curr_tx), FLEX_TRIT_SIZE_243);
    *found_tail = true;
  }

  hash_pack_free(&hash_pack);

  return res;
}

/*
 * State delta operations
 */

retcode_t iota_tangle_state_delta_store(tangle_t const *const tangle,
                                        uint64_t const index,
                                        state_delta_t const *const delta) {
  return iota_stor_state_delta_store(&tangle->conn, index, delta);
}

retcode_t iota_tangle_state_delta_load(tangle_t const *const tangle,
                                       uint64_t const index,
                                       state_delta_t *const delta) {
  return iota_stor_state_delta_load(&tangle->conn, index, delta);
}
