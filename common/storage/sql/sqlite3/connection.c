/*
 * Copyright (c) 2018 IOTA Stiftung
 * https://github.com/iotaledger/entangled
 *
 * Refer to the LICENSE file for licensing information
 */

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <sqlite3.h>

#include "common/storage/connection.h"
#include "common/storage/defs.h"
#include "common/storage/sql/defs.h"
#include "utils/logger_helper.h"

#define CONNECTION_LOGGER_ID "stor_sqlite3_conn"

retcode_t init_connection(const connection_t* const conn,
                          const connection_config_t* const config) {
  retcode_t retcode = RC_OK;
  int rc;
  char* err_msg = 0;
  char* sql;

  logger_helper_init(CONNECTION_LOGGER_ID, LOGGER_DEBUG, true);
  if (config->db_path == NULL) {
    log_critical(CONNECTION_LOGGER_ID, "No path for db specified\n");
    return RC_SQLITE3_NO_PATH_FOR_DB_SPECIFIED;
  } else {
    rc = sqlite3_open_v2(config->db_path, (sqlite3**)&conn->db,
                         SQLITE_OPEN_READWRITE, NULL);
  }

  if (rc) {
    log_critical(CONNECTION_LOGGER_ID, "Failed to open db on path: %s\n",
                 config->db_path);
    return RC_SQLITE3_FAILED_OPEN_DB;
  } else {
    log_info(CONNECTION_LOGGER_ID, "Connection to database %s created\n",
             config->db_path);
  }

  // TODO - implement connections pool so no two threads
  // will access db through same connection simultaneously
  sqlite3_config(SQLITE_CONFIG_MULTITHREAD);
  sqlite3_config(SQLITE_CONFIG_MEMSTATUS, 0);
  sqlite3_busy_timeout((sqlite3*)conn->db, 500);
  sql = "PRAGMA journal_mode = WAL";
  rc = sqlite3_exec((sqlite3*)conn->db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    log_error(CONNECTION_LOGGER_ID, "Failed in statement: %s\n", sql);
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  sql = "PRAGMA foreign_keys = ON";
  rc = sqlite3_exec((sqlite3*)conn->db, sql, 0, 0, &err_msg);

  if (rc != SQLITE_OK) {
    log_error(CONNECTION_LOGGER_ID, "Failed in statement: %s\n", sql);
    sqlite3_free(err_msg);
    return RC_SQLITE3_FAILED_INSERT_DB;
  }

  log_info(CONNECTION_LOGGER_ID, "Connection to database %s initialized\n",
           config->db_path);

  return retcode;
}

retcode_t destroy_connection(connection_t* const conn) {
  if (conn->db != NULL) {
    log_info(CONNECTION_LOGGER_ID, "Destroying connection\n");
    sqlite3_close((sqlite3*)conn->db);
    logger_helper_destroy(CONNECTION_LOGGER_ID);
    conn->db = NULL;
  }
  return RC_OK;
}
