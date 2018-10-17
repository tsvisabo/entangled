/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#ifndef __UTILS_HASH_CONTAINERS_H__
#define __UTILS_HASH_CONTAINERS_H__

#include "uthash.h"
#include "utlist.h"

#include "common/errors.h"
#include "common/trinary/flex_trit.h"
#include "utils/containers/hash/hash243_set.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Functors definitions
 */

typedef retcode_t (*hash_foreach_func)(void *data, flex_trit_t *hash);

/*
 * List types
 */

typedef struct hash_list_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  struct hash_list_entry_s *next;
} hash_list_entry_t;

typedef struct hash_dll_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  struct hash_dll_entry_s *next;
  struct hash_dll_entry_s *prev;
} hash_dll_entry_t;

/*
 * Queue type and operations
 */

typedef hash_dll_entry_t *hash_queue_t;

bool hash_queue_empty(hash_queue_t const queue);
retcode_t hash_queue_push(hash_queue_t *const queue,
                          flex_trit_t const *const hash);
void hash_queue_pop(hash_queue_t *const queue);
flex_trit_t *hash_queue_peek(hash_queue_t const queue);
void hash_queue_free(hash_queue_t *const queue);

/*
 * Stack type and operations
 */

typedef hash_list_entry_t *hash_stack_t;

bool hash_stack_empty(hash_stack_t const stack);
retcode_t hash_stack_push(hash_stack_t *const stack,
                          flex_trit_t const *const hash);
void hash_stack_pop(hash_stack_t *const stack);
flex_trit_t *hash_stack_peek(hash_stack_t const stack);
void hash_stack_free(hash_stack_t *const stack);

/*
 * Set type and operations
 */

typedef struct hash_set_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  UT_hash_handle hh;
} hash_set_entry_t;

typedef hash_set_entry_t *hash_set_t;

uint32_t hash_set_size(hash_set_t const *const set);
retcode_t hash_set_add(hash_set_t *const set, flex_trit_t const *const hash);
retcode_t hash_set_append(hash_set_t const *const set, hash_set_t *const clone);
bool hash_set_contains(hash_set_t const *const set,
                       flex_trit_t const *const hash);
void hash_set_free(hash_set_t *const set);
retcode_t hash_set_for_each(hash_set_t *set, hash_foreach_func func,
                            void *container);

/*
 * Hash-int map type and operations
 */

typedef struct hash_to_int_map_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243];
  int64_t value;
  UT_hash_handle hh;
} hash_to_int_map_entry_t;

typedef hash_to_int_map_entry_t *hash_int_map_t;

retcode_t hash_int_map_add(hash_int_map_t *const map,
                           flex_trit_t const *const hash, int64_t const value);
bool hash_int_map_contains(hash_int_map_t const *const map,
                           flex_trit_t const *const hash);
void hash_int_map_free(hash_int_map_t *const map);

/*
 * Hash-indexed_hash_set map type and operations
 */

typedef struct hash_to_indexed_hash_set_entry_s {
  flex_trit_t hash[FLEX_TRIT_SIZE_243]; /*key*/
  size_t idx;
  hash243_set_t approvers; /*value*/
  UT_hash_handle hh;
} hash_to_indexed_hash_set_entry_t;

typedef hash_to_indexed_hash_set_entry_t *hash_to_indexed_hash_set_map_t;

bool hash_to_indexed_hash_set_map_contains(
    hash_to_indexed_hash_set_map_t const *const map,
    flex_trit_t const *const hash);
retcode_t hash_to_indexed_hash_set_map_add_new_set(
    hash_to_indexed_hash_set_map_t *const map, flex_trit_t const *const hash,
    hash_to_indexed_hash_set_entry_t **const new_set_entry, size_t const index);
void hash_to_indexed_hash_set_map_free(hash_to_indexed_hash_set_map_t *map);

#ifdef __cplusplus
}
#endif

#endif  // __UTILS_HASH_CONTAINERS_H__
