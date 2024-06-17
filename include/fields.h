#ifndef FIELDS_H
  #define FIELDS_H

  #include "globals.h" /* NAME_SIZE and VALUE_SIZE */

  #include "concord/discord.h" /* discord API */

  #include <string.h> /* string formatting */
  #include <stdarg.h> /* variadic functions */

  struct sd_field
  {
    struct discord_embed_field src;
    char name[NAME_SIZE];
    char value[VALUE_SIZE];
  };

  struct sd_fields;

  /*
  @brief 
    sets field name

  @params 
    field: field content
    format: string formatter

  @return
    (void)
  */
  char* set_field_name(struct sd_field *field, const char* format, ...);

  /*
  @brief 
    sets field value

  @params 
    field: field content
    format: string formatter

  @return
    (void)
  */
  char* set_field_value(struct sd_field *field, const char* format, ...);

#endif