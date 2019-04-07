/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __CONSENSUS_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_H__
#define __CONSENSUS_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_H__

#include <stdbool.h>
#include <stdint.h>

#include "consensus/conf.h"
#include "consensus/milestone_tracker/milestone_tracker.h"
#include "consensus/tangle/tangle.h"
#include "utils/handles/cond.h"
#include "utils/handles/rw_lock.h"
#include "utils/handles/thread.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct local_snapshots_manager_s {
  bool running;
  iota_consensus_conf_t* conf;
  thread_handle_t local_snapshots_thread;
  cond_handle_t cond_local_snapshots;
  milestone_tracker_t const* mt;

  // Muteable data
  tangle_t tangle;
  size_t last_snapshot_transactions_count;
} local_snapshots_manager_t;

/**
 * Initializes a local snapshots manager
 *
 * @param lsm The local snapshots manager
 * @param conf Consensus configuration
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_init(local_snapshots_manager_t* lsm, iota_consensus_conf_t const* const conf,
                                    milestone_tracker_t const* const mt);

/**
 * Starts a local snapshots manager
 *
 * @param lsm The local snapshots manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_start(local_snapshots_manager_t* const lsm);

/**
 * Stops a local snapshots manager
 *
 * @param lsm The local snapshots manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_stop(local_snapshots_manager_t* const lsm);

/**
 * Destroys a local snapshots manager
 *
 * @param lsm The local snapshots manager
 *
 * @return a status code
 */
retcode_t iota_local_snapshots_destroy(local_snapshots_manager_t* const lsm);

bool local_snapshots_should_take_snapshot(local_snapshots_manager_t const* const lsm);

retcode_t local_snapshots_take_snapshot(local_snapshots_manager_t* const lsm);

#ifdef __cplusplus
}
#endif

#endif  // __CONSENSUS_LOCAL_SNAPSHOTS_LOCAL_SNAPSHOTS_H__
