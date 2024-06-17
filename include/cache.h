#ifndef CACHE_H
  #define CACHE_H

  #include <stdlib.h>

  #include "fields.h" /* format cache */

  #define ENCOUNTER_SLOTS 2

  enum SLOTS
  {
    SLOT_TOP_LEFT,
    SLOT_TOP_CENTER,
    SLOT_TOP_RIGHT,
    SLOT_MID_RIGHT,
    SLOT_BOTTOM_RIGHT,
    SLOT_BOTTOM_CENTER,
    SLOT_BOTTOM_LEFT,
    SLOT_MID_LEFT,

    SLOTS_SIZE
  };

  enum PIECES 
  {
    PIECE_CORNER_ONE,
    PIECE_CENTER_ONE,
    PIECE_FIRST_TWO,
    PIECE_LAST_TWO,
    PIECE_ROW_THREE,
    PIECE_CORNER_THREE,
    PIECE_L_FOUR,
    PIECE_L_INV_FOUR,

    PIECES_SIZE
  };


  typedef struct { 
    short cache[SLOTS_SIZE]; 
    char db_format[NAME_SIZE]; 

    int value;
    int conjured_acorns;
  } 
  CACHE;

  typedef struct {
    short prev;
    short current;
    short next;

    char db_format[NAME_SIZE];
  }
  PIECES;

  /*
  @brief 
    put formatted cache into field

  @params 
    dest: pointer to array of field sources
    content: array of button content
    field_name: field name

  @return
    pointer to field source
  */
  char* format_cache(struct sd_field *dest, CACHE *content);

  /*
  @brief 
    prints out cache content as formatted emojis

  @params 
    content: pointer to cache content

  @return
    (void)
  */
  void print_cache(CACHE *content);

  /*
  @brief 
    format cache to be placed back in db

  @params 
    content: pointer to cache content

  @return
    (void)
  */
  void format_db_stash(CACHE *content);

  /*
  @brief 
    format shuffle to be placed back in db

  @params 
    content: pointer to shuffle content

  @return
    (void)
  */
  void format_db_shuffle(PIECES *shuffle);

  /*
  @brief 
    overlays content into dest

  @params 
    dest: cache to be overlayed
    content: cache to overlay dest with

  @return
    (void)
  */
  void merge_caches(CACHE *dest, CACHE *content);

  /*
  @brief 
    checks if a piece can fit in specified stash

  @params 
    dest: cache to be checked
    piece: cache to dest with

  @return
    returns 0 if it fits and 1 if it does not
  */
  int check_piece_fit(CACHE *dest, CACHE *piece);

  /*
  @brief 
    checks if stash is filled

  @params 
    stash: cache to be checked

  @return
    returns 1 if stash is complete and 0 if not
  */
  int check_complete_stash(CACHE *stash);

  /*
  @brief 
    fills stash values

  @params 
    stash: cache to be scanned

  @return
    (void)
  */
  void reward_stash(CACHE *stash, int start_idx, int end_idx);

  /*
  @brief 
    searches for a specific type in a cache

  @params 
    stash: cache to be scanned
    type: type of acorn

  @return
    index of type or ERROR_STATUS if type isnt found
  */
  int search_type(CACHE *stash, int type);

  /*
  @brief 
    sets all filled slots to designated type

  @params 
    stash: cache to be filled
    type: type to fill stash with

  @return
    (void)
  */
  void fill_type(CACHE *stash, int type);

  /*
  @brief 
    clears stash of a certain type

  @params 
    stash: cache to be cleared
    type: type to clear stash with

  @return
    (void)
  */
  void clear_type(CACHE *stash, int type);

  /*
  @brief 
    rotate piece clockwise (cw)

  @params 
    stash: cache to be rotated

  @return
    (void)
  */
  void rotate_piece_cw(CACHE *piece);

  /*
  @brief 
    rotate piece counterclockwise (ccw)

  @params 
    stash: cache to be rotated

  @return
    (void)
  */
  void rotate_piece_ccw(CACHE *piece);

  extern CACHE *pieces;

#endif