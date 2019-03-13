/*
 * Copyright (c) 2018 IOTA Stiftung
 * https:github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdio.h>

#include "mam/examples/send-common.h"

int main(int ac, char **av) {
  mam_api_t api;
  bundle_transactions_t *bundle = NULL;
  mam_channel_t *channel = NULL;
  mam_msg_pubkey_t msg_pubkey;
  int msg_pubkey_int;
  retcode_t ret = RC_OK;

  if (ac != 5) {
    fprintf(stderr,
            "usage: send-header <host> <port> <seed> <msg_public_key_type> (0 "
            "-  on channel, 1 - on endpoint, 2 - announce channel, 3 - "
            "announce endpoint)\n");
    return EXIT_FAILURE;
  }

  // Loading or creating MAM API
  if ((ret = mam_api_load(MAM_FILE, &api)) != RC_OK) {
    if ((ret = mam_api_init(&api, (tryte_t *)av[3])) != RC_OK) {
      fprintf(stderr, "mam_api_init failed with err %d\n", ret);
      return EXIT_FAILURE;
    }
  }

  msg_pubkey_int = atoi(av[4]);
  if (msg_pubkey_int < 0 || msg_pubkey_int > 4) {
    fprintf(stderr,
            "usage: send-header <host> <port> <seed> <msg_public_key_type> (0 "
            "-  on channel, 1 - on endpoint, 2 - announce channel, 3 - "
            "announce endpoint)\n");
    return EXIT_FAILURE;
  }

  msg_pubkey = (mam_msg_pubkey_t)msg_pubkey_int;

  // Creating channel
  if ((ret = mam_example_create_channel(&api, &channel)) != RC_OK) {
    fprintf(stderr, "mam_example_create_channel failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  bundle_transactions_new(&bundle);

  {
    // Writing header to bundle
    trit_t msg_id[MAM_MSG_ID_SIZE];

    if (msg_pubkey == mam_msg_pubkey_chid) {
      if ((ret = mam_example_write_header_on_channel(&api, channel, bundle,
                                                     msg_id)) != RC_OK) {
        fprintf(stderr, "mam_example_write_header failed with err %d\n", ret);
        return EXIT_FAILURE;
      }
    } else if (msg_pubkey == mam_msg_pubkey_epid) {
      mam_endpoint_t *new_endpoint = NULL;
      if ((ret = mam_example_announce_new_endpoint(
               &api, channel, bundle, msg_id, &new_endpoint)) != RC_OK) {
        fprintf(stderr,
                "mam_example_announce_new_endpoint failed with err %d\n", ret);
        return EXIT_FAILURE;
      }
      // Sending bundle
      if ((ret = send_bundle(av[1], atoi(av[2]), bundle)) != RC_OK) {
        fprintf(stderr, "send_bundle failed with err %d\n", ret);
        return EXIT_FAILURE;
      }

      bundle_transactions_free(&bundle);
      bundle_transactions_new(&bundle);
      if ((ret = mam_example_write_header_on_endpoint(
               &api, channel, new_endpoint, bundle, msg_id)) != RC_OK) {
        fprintf(stderr,
                "mam_example_write_header_on_endpoint failed with err %d\n",
                ret);
        return EXIT_FAILURE;
      }
      return 0;
    } else if (msg_pubkey == mam_msg_pubkey_epid1) {
      mam_endpoint_t *new_endpoint = NULL;
      bundle_transactions_free(&bundle);
      bundle_transactions_new(&bundle);
      if ((ret = mam_example_announce_new_endpoint(
               &api, channel, bundle, msg_id, &new_endpoint)) != RC_OK) {
        fprintf(stderr,
                "mam_example_announce_new_endpoint failed with err %d\n", ret);
        return EXIT_FAILURE;
      }
    } else if (msg_pubkey == mam_msg_pubkey_chid1) {
      mam_channel_t *new_channel = NULL;
      if ((ret = mam_example_announce_new_channel(&api, channel, bundle, msg_id,
                                                  &new_channel)) != RC_OK) {
        fprintf(stderr,
                "mam_example_announce_new_endpoint failed with err %d\n", ret);
        return EXIT_FAILURE;
      }
      // Sending bundle
      if ((ret = send_bundle(av[1], atoi(av[2]), bundle)) != RC_OK) {
        fprintf(stderr, "send_bundle failed with err %d\n", ret);
        return EXIT_FAILURE;
      }

      bundle_transactions_free(&bundle);
      bundle_transactions_new(&bundle);
      if ((ret = mam_example_write_header_on_channel(&api, new_channel, bundle,
                                                     msg_id)) != RC_OK) {
        fprintf(stderr, "mam_example_write_header failed with err %d\n", ret);
        return EXIT_FAILURE;
      }
    }
  }

  // Sending bundle
  if ((ret = send_bundle(av[1], atoi(av[2]), bundle)) != RC_OK) {
    fprintf(stderr, "send_bundle failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  // Saving and destroying MAM API
  if ((ret = mam_api_save(&api, MAM_FILE)) != RC_OK) {
    fprintf(stderr, "mam_api_save failed with err %d\n", ret);
  }
  if ((ret = mam_api_destroy(&api)) != RC_OK) {
    fprintf(stderr, "mam_api_destroy failed with err %d\n", ret);
    return EXIT_FAILURE;
  }

  // Cleanup
  { bundle_transactions_free(&bundle); }

  return EXIT_SUCCESS;
}
