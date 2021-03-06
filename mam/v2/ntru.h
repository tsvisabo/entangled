/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * MAM is based on an original implementation & specification by apmi.bsu.by
 * [ITSec Lab]
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __MAM_V2_NTRU_H__
#define __MAM_V2_NTRU_H__

#include "mam/v2/prng.h"
#include "mam/v2/trits.h"

/*! \brief NTRU public key - 3g(x)/(1+3f(x)) - size. */
#define MAM2_NTRU_PK_SIZE 9216
/*! \brief NTRU private key - f(x) - size. */
#define MAM2_NTRU_SK_SIZE 1024
/*! \brief NTRU session symmetric key size. */
#define MAM2_NTRU_KEY_SIZE 243
/*! \brief NTRU encrypted key size. */
#define MAM2_NTRU_EKEY_SIZE 9216
/*! \brief NTRU id size. */
#define MAM2_NTRU_ID_SIZE 81

typedef struct _intru {
  trit_t
      *id; /*!< Key id - the first 27 trytes of the corresponding public key.*/
  trit_t *sk; /*!< Private key words. */
} intru;

trits_t ntru_id_trits(intru *n);

trits_t ntru_sk_trits(intru *n);

void ntru_gen(intru *n,  /*!< [in] NTRU interface */
              prng_t *p, /*!< [in] PRNG interface */
              trits_t N, /*!< [in] nonce */
              trits_t pk /*!< [out] NTRU public key */
);

void ntru_encr(trits_t pk, /*!< [in] NTRU public key */
               prng_t *p,  /*!< [in] PRNG interface */
               trits_t K,  /*!< [in] session symmetric key to be encrypted */
               trits_t N,  /*!< [in] nonce */
               trits_t Y   /*!< [out] encrypted K */
);

bool ntru_decr(intru *n,  /*!< [in] NTRU interface */
               trits_t Y, /*!< [in] encrypted K */
               trits_t K  /*!< [out] decrypted session symmetric key */
);

#endif  // __MAM_V2_NTRU_H__
