#ifndef BIOMES_H
  #define BIOMES_H

  enum BIOME 
  {
    GRASSLANDS,
    SEEPING_SANDS,
    NATURE_END,
    DEATH_GRIP,
    LAST_ACORN,
    BIOME_SIZE
  };

  struct sd_encounter
  {
    char* name;
    char* path;
    char* description;
    char* context;
  };

  struct sd_biome_section
  {
    char* name;

    struct sd_encounter *encounters;
    int encounters_size;
  };

  struct sd_biome_file 
  {
    char* name;
    char* emoji;
    char* path;
  };

  struct sd_biome
  {
    char* intro;
    char* scene_path;
    struct sd_biome_file file;

    struct sd_biome_section *sections;
    int sections_size;
  };

  extern struct sd_biome *biomes;

#endif