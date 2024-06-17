#ifndef ITEMS_H
  #define ITEMS_H

  enum ITEM_TYPES 
  {
    TYPE_EMPTY,
    TYPE_ACORN_COUNT,
    TYPE_ACORNS,
    TYPE_GOLDEN_ACORNS,
    TYPE_CONJURED_ACORNS,

    TYPE_SPRING_SEEDS,
    TYPE_SUMMER_CHERRIES,
    TYPE_FALL_BLUEBERRIES,
    TYPE_RIBBONED_ACORN,
    TYPE_COAL,

    TYPE_LEAFY_ACORN,
    TYPE_TREE,
    TYPE_POP,

    TYPE_CURSE,
    TYPE_HEAT_WAVE,
    TYPE_RAINSTORM,
    TYPE_BLIZZARD,
    TYPE_ARCANE_BLAST,


    ITEMS_SIZE
  };

  struct sd_item_file
  {
    char* name;
    char* path;
    char* emoji;
  };

  struct sd_stash_item
  {
    struct sd_item_file file;
    
    int value;
  };

  extern struct sd_stash_item *items;

#endif