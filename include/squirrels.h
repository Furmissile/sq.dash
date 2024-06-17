#ifndef SQUIRRELS_H
  #define SQUIRRELS_H

  #define FIRST_SQUIRREL 50
  #define SECOND_SQUIRREL 100
  #define THIRD_SQUIRREL 250
  #define FOURTH_SQUIRREL 500

  enum SQUIRRELS
  {
    GRAY_SQUIRREL,

    SKELETAL_SQUIRREL,
    SQUIRREL_BOOKIE,
    ANGELIC_SQUIRREL,
    KING_SQUIRREL,

    FLORAL_SQUIRREL,
    GOLDEN_SQUIRREL,
    BLACK_SQUIRREL,
    ACRON_SQUIRREL,

    FLOATY_SQUIRREL,
    BEACH_SQUIRREL,
    PIRATE_SQUIRREL,
    VEGGIE_SQUIRREL,

    RED_SQUIRREL,
    ALCHEMIST_SQUIRREL,
    DEMONIC_SQUIRREL,
    WIZARD_SQUIRREL,

    FROST_SQUIRREL,
    HOARDER_SQUIRREL,
    SANTA_SQUIRREL,
    INFURNO_SQUIRREL,
    
    SQUIRREL_SIZE
  };

  struct sd_squirrel_file
  {
    char* name;
    char* emoji;
    char* path;
    char* description;
  };

  struct sd_squirrel
  {
    struct sd_squirrel_file file;

    int cost;
  };

  extern struct sd_squirrel *squirrels;

#endif