#include "../include/globals.h"
#include "../include/commands.h"
#include "../include/u_string.h"

#include "../include/embed.h"
#include "../include/buttons.h"
#include "../include/emojis.h"

#include "../include/player.h"
#include "../include/game.h"
#include "../include/database.h"

#include "../include/items.h"
#include "../include/squirrels.h"
#include "../include/icons.h"
#include "../include/biomes.h"

/*
  FILE DETAILS:
    * displays player info
    
    * if target id:
      - this is an app selection
      - player is requesting another user
      - fetch user info and display
    * otherwise this must be a button press:
      - user is requestion own info
*/

struct sd_message
{
  struct sd_author author;
  struct sd_title title;
  struct sd_thumbnail thumbnail;

  struct sd_field field;
};

static inline void list_info(struct sd_field *field, struct sd_player *player, struct sd_game *game)
{
  char* indent = ICON_EMOJI(ICON_INDENT);
  char* stahr = ICON_EMOJI(ICON_STAHR);

  u_snprintf(field->value, VALUE_SIZE, 
      "%s %s Level: **%d**",
      indent, stahr, game->level);

  if (player->highest_level > 0)
  {
    char* crown = ICON_EMOJI(ICON_CROWN);
    
    u_snprintf(field->value, VALUE_SIZE, 
        " (%s**%d**)",
        crown, player->highest_level);
  }

  build_progress_bar(field, game);

  struct sd_squirrel_file squirrel = squirrels[player->squirrel].file;

  struct tm *info = get_UTC();

  LABEL conjured_acorns_buf = { 0 };
  char* conjured_acorns = num_str(&conjured_acorns_buf, player->conjured_acorns);

  u_snprintf(field->value, VALUE_SIZE, 
      "\n %s %s Squirrel: **%s**"
      "\n %s %s Conjured Acorns: **%s**",
      indent, fetch_emoji(squirrel.emoji), squirrel.name,
      indent, CACHE_ITEM_EMOJI(TYPE_CONJURED_ACORNS), conjured_acorns);

  if (player->stashes_complete != ERROR_STATUS)
    u_snprintf(field->value, VALUE_SIZE, 
        "\n %s %s Daily Stashes: **%d**/%d (**%d** hours left)",
        indent, ICON_EMOJI(ICON_TREASURE), player->stashes_complete, DAILY_STASHES, 24 - info->tm_hour);
  else
    u_snprintf(field->value, VALUE_SIZE, 
        "\n %s %s *Daily completed!* (**%d** hours until reset)",
        indent, ICON_EMOJI(ICON_TREASURE), 24 - info->tm_hour);
}

void info_interaction(const struct discord_interaction *event)
{
  struct sd_message message = { 0 };

  PGresult* search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, event->member->user->id, 
      "highest_level, conjured_acorns, stashes_complete, color, squirrel, t_stamp");
  ERROR_INTERACTION((search_player == NULL), "Player data couldn't be retrieved!");

  struct sd_player player = (struct sd_player) 
  {
    .highest_level = u_strtol( PQgetvalue(search_player, 0, 0) ),
    .conjured_acorns = u_strtol( PQgetvalue(search_player, 0, 1) ),
    .stashes_complete = u_strtol( PQgetvalue(search_player, 0, 2) ),

    .color = u_strtol( PQgetvalue(search_player, 0, 3) ),
    .squirrel = u_strtol( PQgetvalue(search_player, 0, 4) ),

    .timestamp = u_strtol( PQgetvalue(search_player, 0, 5) )
  };
  PQclear(search_player);
      
  ERROR_INTERACTION((event->message->timestamp < player.timestamp),
      "This appears to be an old session! Please renew your session by sending `/start`.");

  player.timestamp = event->message->timestamp;

  PGresult* search_game = (PGresult*) { 0 };
  search_game = get_game_data(search_game, event->member->user->id, "level, progress");
  ERROR_INTERACTION((search_game == NULL), "Game data couldn't be retrieved!");

  struct sd_game game = (struct sd_game) 
  {
    .level = u_strtol( PQgetvalue(search_game, 0, 0) ),
    .progress = u_strtol( PQgetvalue(search_game, 0, 1) )
  };
  PQclear(search_game);

  game.biome = (game.level -1) % BIOME_SIZE;

  list_info(&message.field, &player, &game);

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player.color,
    .author = set_author(&message.author, event->member->user),
    .title = set_title(&message.title, "Player Info"),
    .thumbnail = set_thumbnail(&message.thumbnail, squirrels[player.squirrel].file.path),

    .fields = &(struct discord_embed_fields)
    {
      .array = &(struct discord_embed_field)
      {
        .name = set_field_name(&message.field, "General Info"),
        .value = message.field.value
      },
      .size = 1
    }
  };

  struct sd_util_buttons util_buttons = { 0 };
  set_util_buttons(&util_buttons, event, &player, &game);

  struct sd_info_buttons info_buttons = { 0 };
  set_info_buttons(&info_buttons, event, &game);

  struct discord_component action_rows[2] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = util_buttons.array,
        .size = sizeof(util_buttons.array)/sizeof(*util_buttons.array)
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = info_buttons.array,
        .size = sizeof(info_buttons.array)/sizeof(*info_buttons.array)
      }
    }
  };

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_UPDATE_MESSAGE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &embed,
        .size = 1
      },
      .components = &(struct discord_components) {
        .array = action_rows,
        .size = 2
      }
    }

  };

  PGresult *update_player = (PGresult*) { 0 };
  update_player = SQL_query(update_player, "update public.player set t_stamp = %ld where user_id = %ld",
      player.timestamp, event->member->user->id);
  PQclear(update_player);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
}

void fetch_player_info(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  const struct discord_interaction *event = resp->keep;

  struct sd_message message = { 0 };

  PGresult* search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, user->id, 
      "highest_level, conjured_acorns, stashes_complete, color, squirrel, t_stamp");
  ERROR_INTERACTION((search_player == NULL), "Player data couldn't be retrieved!");

  struct sd_player player = (struct sd_player) 
  {
    .highest_level = u_strtol( PQgetvalue(search_player, 0, 0) ),
    .conjured_acorns = u_strtol( PQgetvalue(search_player, 0, 1) ),
    .stashes_complete = u_strtol( PQgetvalue(search_player, 0, 2) ),

    .color = u_strtol( PQgetvalue(search_player, 0, 3) ),
    .squirrel = u_strtol( PQgetvalue(search_player, 0, 4) ),

    .timestamp = u_strtol( PQgetvalue(search_player, 0, 5) )
  };
  PQclear(search_player);

  PGresult* search_game = (PGresult*) { 0 };
  search_game = get_game_data(search_game, user->id, "level, progress");
  ERROR_INTERACTION((search_game == NULL), "Game data couldn't be retrieved!");

  struct sd_game game = (struct sd_game) 
  {
    .level = u_strtol( PQgetvalue(search_game, 0, 0) ),
    .progress = u_strtol( PQgetvalue(search_game, 0, 1) )
  };
  PQclear(search_game);

  game.biome = (game.level -1) % BIOME_SIZE;

  list_info(&message.field, &player, &game);

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player.color,
    .author = set_author(&message.author, user),
    .title = set_title(&message.title, "Player Info"),
    .thumbnail = set_thumbnail(&message.thumbnail, squirrels[player.squirrel].file.path),

    .fields = &(struct discord_embed_fields)
    {
      .array = &(struct discord_embed_field)
      {
        .name = set_field_name(&message.field, "General Info"),
        .value = message.field.value
      },
      .size = 1
    }
  };

  struct discord_interaction_response interaction = 
  {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) 
    {
      .embeds = &(struct discord_embeds) 
      {
        .array = &embed,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
}

void request_player_interaction(const struct discord_interaction *event)
{
  struct discord_ret_user ret_user = {
    .done = &fetch_player_info,
    .fail = &not_user,
    .keep = event
  };

  discord_get_user(client, event->data->target_id, &ret_user);
}