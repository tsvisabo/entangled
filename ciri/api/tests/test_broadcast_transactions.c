/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <unity/unity.h>

#include "ciri/api/api.h"
#include "ciri/api/tests/defs.h"
#include "consensus/conf.h"
#include "consensus/test_utils/bundle.h"
#include "consensus/test_utils/tangle.h"
#include "gossip/node.h"

static char *test_db_path = "ciri/api/tests/test.db";
static char *ciri_db_path = "ciri/api/tests/ciri.db";
static connection_config_t config;
static iota_api_t api;
static node_t node;
static iota_consensus_t consensus;

void setUp(void) {
  TEST_ASSERT(tangle_setup(&api.consensus->tangle, &config, test_db_path,
                           ciri_db_path) == RC_OK);
}

void tearDown(void) {
  TEST_ASSERT(tangle_cleanup(&api.consensus->tangle, test_db_path) == RC_OK);
}

void test_broadcast_transactions_empty(void) {
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();

  TEST_ASSERT(iota_api_broadcast_transactions(&api, req) == RC_OK);

  TEST_ASSERT_EQUAL_INT(broadcaster_size(&api.node->broadcaster), 0);

  broadcast_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

void test_broadcast_transactions_invalid_tx(void) {
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();
  struct _iota_transaction tx;
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];

  // Trying to broadcast an invalid transaction (invalid supply)

  flex_trits_from_trytes(
      tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION, TX_1_OF_4_VALUE_BUNDLE_TRYTES,
      NUM_TRYTES_SERIALIZED_TRANSACTION, NUM_TRYTES_SERIALIZED_TRANSACTION);
  transaction_deserialize_from_trits(&tx, tx_trits);
  transaction_set_value(&tx, -IOTA_SUPPLY - 1);
  transaction_serialize_on_flex_trits(&tx, tx_trits);
  hash_array_push(req->trytes, tx_trits);
  TEST_ASSERT(iota_api_broadcast_transactions(&api, req) == RC_OK);

  TEST_ASSERT_EQUAL_INT(broadcaster_size(&api.node->broadcaster), 0);

  broadcast_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

void test_broadcast_transactions(void) {
  broadcast_transactions_req_t *req = broadcast_transactions_req_new();
  tryte_t const *const txs_trytes[4] = {
      TX_1_OF_4_VALUE_BUNDLE_TRYTES, TX_2_OF_4_VALUE_BUNDLE_TRYTES,
      TX_3_OF_4_VALUE_BUNDLE_TRYTES, TX_4_OF_4_VALUE_BUNDLE_TRYTES};
  flex_trit_t tx_trits[FLEX_TRIT_SIZE_8019];

  // Broadcasting 4 transactions

  for (size_t i = 0; i < 4; i++) {
    flex_trits_from_trytes(tx_trits, NUM_TRITS_SERIALIZED_TRANSACTION,
                           txs_trytes[i], NUM_TRYTES_SERIALIZED_TRANSACTION,
                           NUM_TRYTES_SERIALIZED_TRANSACTION);
    hash_array_push(req->trytes, tx_trits);
  }
  TEST_ASSERT(iota_api_broadcast_transactions(&api, req) == RC_OK);

  TEST_ASSERT_EQUAL_INT(broadcaster_size(&api.node->broadcaster), 4);

  broadcast_transactions_req_free(&req);
  TEST_ASSERT(req == NULL);
}

int main(void) {
  UNITY_BEGIN();

  config.db_path = test_db_path;
  api.consensus = &consensus;
  api.node = &node;
  broadcaster_init(&api.node->broadcaster, api.node);
  TEST_ASSERT(iota_consensus_conf_init(&api.consensus->conf) == RC_OK);
  api.consensus->conf.snapshot_timestamp_sec = 1536845195;
  api.consensus->conf.mwm = 1;

  iota_consensus_transaction_validator_init(
      &api.consensus->transaction_validator, &api.consensus->conf);

  RUN_TEST(test_broadcast_transactions_empty);
  RUN_TEST(test_broadcast_transactions_invalid_tx);
  RUN_TEST(test_broadcast_transactions);

  return UNITY_END();
}
