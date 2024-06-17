#ifndef EMOJIS_H
  #define EMOJIS_H

  #include "globals.h"

  #include <libpq-fe.h> /* access emojis */
  #include "concord/discord.h" /* discord fields */

  #include <stdarg.h> /* variadic functions */
  #include <string.h>

  extern PGresult *sd_emojis;

  enum DB_EMOJI // emojis are a string too!
  {
    DB_GUILD_ID,
    DB_EMOJI_ID,
    DB_EMOJI_NAME,
    DB_EMOJI_LITERAL,
    DB_EMOJI_SIZE
  };

  struct sd_emoji
  {
    struct discord_emoji src;   /* discord_emoji pointer */
    char emoji_name[NAME_SIZE]; /* discord_emoji content */
  };

  #define CACHE_ITEM_EMOJI(type_idx) fetch_emoji(items[type_idx].file.emoji)

  #define SQUIRREL_EMOJI(squirrel_idx) fetch_emoji(squirrels[squirrel_idx].file.emoji)

  #define ICON_EMOJI(icon_idx) fetch_emoji(icons[icon_idx].emoji)

  /*
  @brief 
    retrieves emoji literal

  @params 
    name: name of emoji

  @return
    emoji literal in form <:emoji_name:emoji_id>
  */
  char* fetch_emoji(const char* name);

  /*
  @brief 
    retrieves emoji id

  @params 
    name: name of emoji

  @return
    emoji id or ERROR_STATUS if failed
  */
  u64snowflake fetch_emoji_id(const char* name);

  /*
  @brief 
    sets button emoji

  @params 
    emoji: emoji content
    name: name of emoji

  @return
    pointer to emoji content
  */
  struct discord_emoji* set_button_emoji(struct sd_emoji *emoji, char* name);

  /*
  @brief 
    updates emojis table if change occurs

  @params 
    client: discord client
    event: pointer to content of emoji update event

  @return
    (void)
  */
  void emoji_update(struct discord *client, const struct discord_guild_emojis_update *event);

  /*
  @brief 
    initializes emojis on startup
    (sync table if bot is offline)

  @params 
    client: discord client
    resp: response callback data
    guild: pointer to specified guild content

  @return
    (void)
  */
  void add_emojis(struct discord *client, struct discord_response *resp, const struct discord_guild *guild);

#endif