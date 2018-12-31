/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include "ciri/perceptive_node/conf.h"

#define DEFAULT_TEST_SAMPLE_SIZE 200
#define DEFAULT_SEQUENCE_SIZE 50
#define PERCEPTIVE_NODE_INTERVAL_SECONDS 60

retcode_t iota_perceptive_node_conf_init(
    iota_perceptive_node_conf_t* const conf) {
  if (conf == NULL) {
    return RC_NULL_PARAM;
  }

  conf->test_sample_size = DEFAULT_TEST_SAMPLE_SIZE;
  conf->monitored_transactions_sequence_size = DEFAULT_SEQUENCE_SIZE;
  conf->monitoring_interval_seconds = PERCEPTIVE_NODE_INTERVAL_SECONDS;
  conf->is_enabled = false;

  return RC_OK;
}
