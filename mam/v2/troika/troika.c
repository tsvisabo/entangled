/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#include "mam/v2/troika/troika.h"
#include "common/troika/troika.h"
#include "mam/v2/trits/trits.h"

/*void troika_permutation(trit_t *state, unsigned long num_rounds)*/
void mam_troika_transform(trit_t *const state, size_t state_size) {
  size_t i;
  for (i = 0; i != state_size; ++i) {
    state[i] += 1;
  }
  troika_permutation(state, MAM2_TROIKA_NUM_ROUNDS);
  for (i = 0; i != state_size; ++i) {
    state[i] -= 1;
  }
}