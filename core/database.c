#include "../include/database.h"
#include "../include/game.h"
#include "../include/u_string.h"


PGresult* SQL_query(PGresult *query, char* format, ...)
{
  char buffer[16384] = { };

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, sizeof(buffer), format, args);

  va_end(args);

  query = PQexec(conn, buffer);

  if (PQntuples(query) == 0)
  {
    LOG_DB_ERROR(query);
    return NULL;
  }

  return query;
}

PGconn* establish_connection(char* conninfo)
{
  PGconn* db_conn = PQconnectdb(conninfo);

  if (PQstatus(db_conn) != CONNECTION_OK)
  {
    fprintf(stderr, PQerrorMessage(db_conn) );
    PQfinish(db_conn);
    exit(1);
  }

  PQexec(db_conn, "SELECT pg_catalog.set_config('search_path', '', false)");

  return db_conn;
}

void create_player_data(u64snowflake user_id)
{
  PGresult *player_query = (PGresult*) { 0 };

  player_query = SQL_query(player_query, 
      "select * from public.player where user_id = %ld",
      user_id);

  if (PQntuples(player_query) == 0)
  {
    PQclear(player_query);

    player_query = SQL_query(player_query,
        "insert into public.player values(%ld, 0, 0, 0, %ld, 0, 0, 4, 0, 0); \n"
        "insert into public.biome_story values(%ld, 0, 0, 0, 0, 0)",
        user_id, discord_timestamp(client), user_id, user_id);
  }

  PQclear(player_query);
}

PGresult* get_player_data(PGresult *player_query, u64snowflake user_id, const char* columns)
{
  player_query = SQL_query(player_query, 
      "select %s from public.player where user_id = %ld",
      columns, user_id);

  if (PQntuples(player_query) == 0)
  {
    LOG_DB_ERROR(player_query);
    return NULL;
  }

  return player_query;
}

PGresult* get_game_data(PGresult *game_query, u64snowflake user_id, const char* columns)
{
  game_query = SQL_query(game_query, 
      "select * from public.game where user_id = %ld",
      user_id);

  if (PQntuples(game_query) == 0)
  {
    PQclear(game_query);

    struct sd_game new_game = { 0 };

    // set next piece
    new_game.shuffle.prev = -1;
    new_game.shuffle.next = 2 * (rand() % 4);

    new_game.shuffle.prev = new_game.shuffle.current;
    new_game.shuffle.current = new_game.shuffle.next;

    new_game.shuffle.next = piece_generation(&new_game.shuffle, new_game.shuffle.prev, 0);

    format_db_shuffle(&new_game.shuffle);

    // set current piece to current, decorate, and encode
    merge_caches(&new_game.current_piece, &pieces[new_game.shuffle.current]);
    decorate_piece(&new_game.current_piece);
    format_db_stash(&new_game.current_piece);

    // set next piece to next, decorate, and encode
    merge_caches(&new_game.next_piece, &pieces[new_game.shuffle.next]);
    decorate_piece(&new_game.next_piece);
    format_db_stash(&new_game.next_piece);

    game_query = SQL_query(game_query,
        "insert into public.game values(%ld, 0, 1, '%s', "EMPTY_CACHE", "EMPTY_CACHE", "EMPTY_CACHE", '%s', '%s', 1, 0, -1, -1, -1, 1)",
        user_id, new_game.shuffle.db_format, new_game.current_piece.db_format, new_game.next_piece.db_format);
  }
  PQclear(game_query);

  game_query = SQL_query(game_query, 
      "select %s from public.game where user_id = %ld",
      columns, user_id);

  if (PQntuples(game_query) == 0)
  {
    LOG_DB_ERROR(game_query);
    return NULL;
  }

  return game_query;
}

CACHE PGscan_stash(PGresult *query, int column)
{
  CACHE stash = (CACHE) { 0 };

  // query column is assumed to be unnested!
  for (int cache_idx = 0; cache_idx < SLOTS_SIZE; cache_idx++)
  {
    stash.cache[cache_idx] = u_strtol( PQgetvalue(query, cache_idx, column) );
  }

  return stash;
}

PIECES PGscan_shuffle(PGresult *query, int column)
{
  PIECES shuffle = (PIECES) { 0 };

  // query column is assumed to be unnested!
  shuffle.prev = u_strtol( PQgetvalue(query, 0, column) );
  shuffle.current = u_strtol( PQgetvalue(query, 1, column) );
  shuffle.next = u_strtol( PQgetvalue(query, 2, column) );

  return shuffle;
}
