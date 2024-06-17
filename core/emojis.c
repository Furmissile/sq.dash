#include "../include/emojis.h"
#include "../include/icons.h"
#include "../include/u_string.h"
#include "../include/database.h"


char* fetch_emoji(const char* name)
{
  char* emoji = PQgetvalue(sd_emojis, 0, DB_EMOJI_NAME);
  int rows = PQntuples(sd_emojis);

  for (int emoji_idx = 0; emoji_idx < rows; emoji_idx++)
  {
    emoji = PQgetvalue(sd_emojis, emoji_idx, DB_EMOJI_NAME);

    if (emoji[0] != name[0])
      continue;

    if (strcmp(emoji, name) == 0)
      return PQgetvalue(sd_emojis, emoji_idx, DB_EMOJI_LITERAL);
  }

  return "Err";
}

u64snowflake fetch_emoji_id(const char* name)
{
  char* emoji = PQgetvalue(sd_emojis, 0, DB_EMOJI_NAME);
  int rows = PQntuples(sd_emojis);

  for (int emoji_idx = 0; emoji_idx < rows; emoji_idx++)
  {
    emoji = PQgetvalue(sd_emojis, emoji_idx, DB_EMOJI_NAME);
    if (emoji[0] != name[0])
      continue;

    if (strcmp(emoji, name) == 0)
      return u_strtol( PQgetvalue(sd_emojis, emoji_idx, DB_EMOJI_ID) );
  }

  return ERROR_STATUS;
}

struct discord_emoji* set_button_emoji(struct sd_emoji *emoji, char* name)
{
  u64snowflake emoji_id = fetch_emoji_id(name);

  if (emoji_id == (u64snowflake)ERROR_STATUS)
  {
    emoji->src = (struct discord_emoji)
    {
      .name = icons[ICON_INDENT].emoji,
      .id = fetch_emoji_id(icons[ICON_INDENT].emoji)
    };

    LOG_FORMAT_ERROR("'%s' doesn't exist!", name);
  }
  else {
    emoji->src = (struct discord_emoji)
    {
      .name = u_snprintf(emoji->emoji_name, NAME_SIZE, name),
      .id = fetch_emoji_id(name)
    };
  }

  return &emoji->src;
}

void emoji_update(struct discord *client, const struct discord_guild_emojis_update *event)
{
  (void)event;
  
  // reset emojis
  struct discord_ret_guild ret = {
    .done = &add_emojis
  };

  discord_get_guild(client, GUILD_ID, &ret);
  discord_get_guild(client, ARCHIVE_ID, &ret);
}

void add_emojis(struct discord *client, struct discord_response *resp, const struct discord_guild *guild)
{
  (void)client;
  (void)resp;

  if (guild->emojis->size == 0) {
      printf("No emojis in guild \n\n");
      return;
  }

  // clear table on bot startup NOT per guild
  PGresult *clear_emojis = (PGresult*) { 0 };
  clear_emojis = SQL_query(clear_emojis, "delete from public.emojis where guild_id = %ld", guild->id);
  PQclear(clear_emojis);

  // add all emojis to db by mass insertion
  PGresult *fill_emojis = (PGresult*) { 0 };
  char emoji_cmd[16384] = { };

  struct discord_emoji *first_emoji = &guild->emojis->array[0];

  u_snprintf(emoji_cmd, sizeof(emoji_cmd), 
      "insert into public.emojis(guild_id, emoji_id, emoji_name, emoji_literal) values (%ld, %ld, '%s', '<%s%s:%ld>')",
      guild->id, first_emoji->id, first_emoji->name, (first_emoji->animated) ? "a:" : ":", first_emoji->name, first_emoji->id);

  for (int i = 1; i < guild->emojis->size; i++)
  {
    struct discord_emoji *emoji = &guild->emojis->array[i];

    u_snprintf(emoji_cmd, sizeof(emoji_cmd), ", (%ld, %ld, '%s', '<%s%s:%ld>') ",
        guild->id, emoji->id, emoji->name, (emoji->animated) ? "a:" : ":", emoji->name, emoji->id);
  }

  fill_emojis = SQL_query(fill_emojis, emoji_cmd);
  PQclear(fill_emojis);

  PQclear(sd_emojis);
  sd_emojis = SQL_query(sd_emojis, "select * from public.emojis order by emoji_name");
}