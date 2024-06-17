#ifndef DATABASE_H
  #define DATABASE_H

  #include "globals.h" /* connection, NAME_SIZE and VALUE_SIZE */
  #include "u_string.h"
  #include "cache.h" /* CACHE */

  #include <libpq-fe.h> /* querying */
  #include "concord/discord.h" /* discord API */

  #include <stdlib.h>

  #define EMPTY_CACHE "array_fill(0, array[8])"

  #define LOG_DB_ERROR(query) \
      PQclear(query); \
      FILE *fp = fopen("crash.log", "a"); \
      fprintf(fp, PQerrorMessage(conn)); \
      fclose(fp); \
      printf("%s \n", PQerrorMessage(conn));

  /*
  @brief 
    requests query from database

  @params 
    query: query content
    format: command format

  @return
    pointer to query content
  */
  PGresult* SQL_query(PGresult *query, char* format, ...);

  /*
  @brief 
    establishes connection to database

  @params 
    conninfo: connection info

  @return
    pointer to database connection content
  */
  PGconn* establish_connection(char* conninfo);

  /*
  @brief 
    inserts player row w. biome story

  @params 
    user_id: id of user

  @return
    (void)
  */
  void create_player_data(u64snowflake user_id);

  /*
  @brief 
    fetch player columns from db and add if player doesnt exist

  @params 
    dest: query to be filled
    user_id: id of user
    columns: comma separated list of columns

  @return
    pointer to query content
  */
  PGresult* get_player_data(PGresult *player_query, u64snowflake user_id, const char* columns);

  /*
  @brief 
    fetch game columns from db and add if game doesnt exist

  @params 
    dest: query to be filled
    user_id: id of user
    columns: comma separated list of columns

  @return
    pointer to query content
  */
  PGresult* get_game_data(PGresult *game_query, u64snowflake user_id, const char* columns);

  /*
  @brief 
    fetch stash from db

  @params 
    query: specified query
    column: column index of stash

  @return
    CACHE content

  @note
    query column is assumed to be unnested
  */
  CACHE PGscan_stash(PGresult *query, int column);

  /*
  @brief 
    fetch shuffle from db

  @params 
    query: specified query
    column: column index of shuffle

  @return
    PIECES content

  @note
    query column is assumed to be unnested
  */
  PIECES PGscan_shuffle(PGresult *query, int column);

#endif