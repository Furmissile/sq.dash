#ifndef GAME_H
  #define GAME_H

  #include "database.h"
  #include "cache.h"
  #include "biomes.h"
  #include "player.h"

  #include <stdlib.h>

  #define MAX_STASHES 3
  #define BAR_SIZE 5 /* length of progress bar in total */

  #define CHRISTMAS_MONTH 11

  #define COMMON_CHANCE 50 // 50% -- acorn
  #define RARE_CHANCE 80 // 30% -- golden acorn
  #define MAX_CHANCE 100 // 20% -- conjured acorn

  #define DISASTER_CHANCE 90 // 10% -- disaster
  #define EVENT_CHANCE 80 // 20% -- event resources, encounter, story chance
  #define SPECIAL_ACORN_CHANCE 70 // 30% -- leafy and magic acorn
  
  #define DAILY_REWARD 10
  #define DAILY_STASHES 30

  #define BASE_BIOME 50     // first biome starts with 50 pts
  #define BIOME_INTERVAL 25 // to be multiplied with game level

  #define TIME_INTERVAL 10 // disaster chanace increase interval
  #define MAX_INTERVALS 5 // max number of increases allowed
  
  #define THREAT_INCREASE 20
  #define ENCOUNTER_INCREASE 4 // caps at +20% chance increase

  #define MAX_BIOME_TIME TIME_INTERVAL * MAX_INTERVALS // max time interval

  struct sd_game
  {
    u64snowflake user_id;
    int level;
    int progress;
    int piece_count;
    int is_restart;

    int biome;
    int is_new_biome;
    int biome_time;
    int threat;
    int disaster_stash;

    int encounter_increase;
    int is_biome_story;
    int encounter;
    int section;

    CACHE stashes[MAX_STASHES];
    CACHE current_piece;
    CACHE next_piece;

    PIECES shuffle;
  };

  /*
  @brief 
    reshuffle game pieces

  @params 
    shuffle: game shuffle to be rearranged
    ref_piece: the other piece to take into consideration
    piece_count: player's piece count

  @return
    (void)
  */
  int piece_generation(PIECES *shuffle, int ref_piece, int piece_count);

  /*
  @brief 
    builds level progress bar

  @params 
    field: field bar is placed in
    game: game content

  @return
    (void)
  */
  void build_progress_bar(struct sd_field *field, struct sd_game *game);

  /*
  @brief 
    gives each filled index of a cache a certain acorn type

  @params 
    piece: piece to be decoreated

  @return
    (void)
  */
  void decorate_piece(CACHE *piece);

  /*
  @brief 
    computes leafy acorn mechanic

  @params 
    stash: stash of leafy acorn
    tree_idx: slot idx of tree

  @return
    (void)
  */
  void use_leafy_acorn(CACHE *stash, int tree_idx);

  /*
  @brief 
    initializes encounter event

  @params 
    game: game content

  @return
    (void)
  */
  void spawn_encounter(struct sd_game *game);

  /*
  @brief 
    shifts game pieces when a piece is placed

  @params 
    game: game content

  @return
    (void)
  */
  void shift_pieces(struct sd_game *game);

  /*
  @brief 
    computes daily task completion

  @params 
    event: discord interaction content
    player: player content

  @return
    (void)
  */
  void set_daily(const struct discord_interaction *event, struct sd_player *player);

  /*
  @brief 
    initializes biome disaster event

  @params 
    player: player content
    game: game content

  @return
    (void)
  */
  void spawn_biome_disaster(struct sd_player *player, struct sd_game *game);

#endif