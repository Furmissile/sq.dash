#ifndef PLAYER_H
  #define PLAYER_H

  #define DAILY_REWARD 10
  #define DAILY_STASHES 30

  #define STEAL_CHANCE 70
  #define STEAL_COST 25

  #include <libpq-fe.h>  /* access player */
  #include "concord/discord.h" /* discord API */

  enum DB_PLAYER
  {
    DB_USER_ID,
    DB_SCURRY_ID,
    DB_COLOR,
    DB_SQUIRREL,
    DB_HIGHEST_LEVEL, // rename high_acorn_count to highest_level
    DB_STOLEN_ACORNS,
    DB_ENCOUNTER,
    DB_TIMESTAMP,
    DB_CONJURED_ACORNS,
    DB_SECTION,
    DB_PURCHASED,
    DB_TREASURY,
    DB_DAILY_STASHES,
    DB_DAILY_DAY,

    DB_SIZE
  };
  
  struct sd_player
  {
    int highest_level;
    int treasury;
    int conjured_acorns;
    int purchased;

    int button_idx;
    int squirrel;
    int color;
    u64unix_ms timestamp;
    
    int daily_complete;
    int stashes_complete;
    int tm_mday;
  };

#endif