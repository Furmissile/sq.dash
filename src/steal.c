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
  * stealing another player's treasury

  * if treasury is full ? disable stealing : enable stealing 

  * if there are no available targets
    - send error message

  * find player to steal from
    - pass in message

  * request user data
    - upon request cb, chance if steal can work
    
  * apply mechanic
    - take 1 golden acorn from player
    - add 1 golden acorn to event player
    - send success/fail message
*/

struct sd_fields
{
  struct sd_field content[1 +1];           // +1 for balance
  struct discord_embed_field array[1 +1];
};

struct sd_buttons
{
  struct sd_button content;
  struct sd_emoji stash_emoji;
  struct discord_component button;
};

struct sd_message
{
  struct sd_author author;
  struct sd_title title;
  struct sd_thumbnail thumbnail;
  struct sd_description description;
  struct sd_footer footer;

  struct sd_fields fields;

  struct sd_buttons buttons;
};

static inline void list_info(struct sd_fields *fields, struct sd_player *player, int steal_status)
{
  LABEL conjured_acorns_buf = { 0 };
  char* conjured_acorns = num_str(&conjured_acorns_buf, player->conjured_acorns);

  char* indent = ICON_EMOJI(ICON_INDENT);
  char* conjured_acorn = CACHE_ITEM_EMOJI(TYPE_CONJURED_ACORNS);

  fields->array[0] = (struct discord_embed_field)
  {
    .name = set_field_name(&fields->content[0], "Balance"),
    .value = set_field_value(&fields->content[0], "%s %s Conjured Acorns: **%s**",
      indent, conjured_acorn, conjured_acorns)
  };

  CACHE treasury = (CACHE) { 0 };
  for (int slot_idx = 0; slot_idx < player->treasury; slot_idx++)
  {
    treasury.cache[slot_idx] = TYPE_GOLDEN_ACORNS;
  }

  fields->array[1] = (struct discord_embed_field)
  {
    .name = set_field_name(&fields->content[1], "Treasury"),
    .value = format_cache(&fields->content[1], &treasury)
  };

  if (steal_status == 1)
  {
    char* golden_acorn = CACHE_ITEM_EMOJI(TYPE_GOLDEN_ACORNS);

    u_snprintf(fields->content[1].value, VALUE_SIZE, "\n +1 %s", golden_acorn);
  }
}

static inline void set_buttons(const struct discord_interaction *event, struct sd_buttons *steal_buttons, struct sd_player *player)
{
  steal_buttons->button = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .label = set_label(&steal_buttons->content, "(-%d) Steal", STEAL_COST),
    .custom_id = set_custom_id(&steal_buttons->content, "%c0.%ld",
        TYPE_STEAL, event->member->user->id),
    .emoji = set_button_emoji(&steal_buttons->stash_emoji, items[TYPE_CONJURED_ACORNS].file.emoji)
  };

  set_button_status(&steal_buttons->button, player->treasury == SLOTS_SIZE || player->conjured_acorns < STEAL_COST);
}

void steal_from_member(struct discord *client, struct discord_response *resp, const struct discord_guild_member *member)
{
  const struct discord_interaction *event = resp->keep;

  char content[NAME_SIZE] = { };

  discord_create_message(client, event->channel_id, 
      &(struct discord_create_message)
      {
        .content = u_snprintf(content, NAME_SIZE,
            "**<@%ld>**, **%s** failed to snatch from your treasury!", 
            member->user->id, event->member->user->username)
      },
      NULL);
}

void steal_from_user(struct discord *client, struct discord_response *resp)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  char content[NAME_SIZE] = { };

  discord_create_message(client, event->channel_id, 
      &(struct discord_create_message)
      {
        .content = u_snprintf(content, NAME_SIZE,
            "You have failed to steal from an anonymous treasury!")
      },
      NULL);
}

int steal(const struct discord_interaction *event, struct sd_footer *footer, struct sd_player *player)
{
  PGresult *update_player = (PGresult*) { 0 };

  /*
    check if the player successfully stole and update footer
    if player failed to steal, request player and create message
  */
  PGresult* target_steal = (PGresult*) { 0 };
  target_steal = SQL_query(target_steal, 
      "select user_id from public.player where treasury > 0 and user_id != %ld order by random() LIMIT 1",
      event->member->user->id);

  u64snowflake target_id = u_strtol( PQgetvalue(target_steal, 0, 0) );
  PQclear(target_steal);

  player->conjured_acorns -= STEAL_COST;

  // if steal is successful
  if (rand() % MAX_CHANCE > STEAL_CHANCE)
  {
    player->treasury++;

    set_footer(footer, icons[ICON_TREASURE].path,
        "You have successfully stolen from a player!");

    update_player = SQL_query(update_player, 
        "update public.player set conjured_acorns = %d, treasury = %d where user_id = %ld; \n"
        "update public.player set treasury = treasury -1 where user_id = %ld;",
        player->conjured_acorns, player->treasury, event->member->user->id, target_id);
    PQclear(update_player);
    return 1;
  }

  set_footer(footer, icons[ICON_TREASURE].path, "Steal failed!");

  // if the steal failed, request the player
  struct discord_ret_guild_member ret_member = {
    .done = &steal_from_member,
    .fail = &steal_from_user,
    .keep = event
  };

  // check if guild member
  discord_get_guild_member(client, event->guild_id, target_id, &ret_member);

  update_player = SQL_query(update_player, 
      "update public.player set conjured_acorns = %d where user_id = %ld",
      player->conjured_acorns, event->member->user->id);
  PQclear(update_player);

  return 0;
}


void steal_interaction(const struct discord_interaction *event)
{
  struct sd_message message = { 0 };

  PGresult* search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, event->member->user->id, 
      "conjured_acorns, treasury, color, squirrel, t_stamp");
  ERROR_INTERACTION((search_player == NULL), "Player data couldn't be retrieved!");

  struct sd_player player = (struct sd_player) 
  {
    .conjured_acorns = u_strtol( PQgetvalue(search_player, 0, 0) ),
    .treasury = u_strtol( PQgetvalue(search_player, 0, 1) ),

    .color = u_strtol( PQgetvalue(search_player, 0, 2) ),
    .squirrel = u_strtol( PQgetvalue(search_player, 0, 3) ),

    .timestamp = u_strtol( PQgetvalue(search_player, 0, 4) )
  };
  PQclear(search_player);

  ERROR_INTERACTION((event->message->timestamp < player.timestamp),
      "This appears to be an old session! Please renew your session by sending `/start`.");

  PGresult* search_game = (PGresult*) { 0 };
  search_game = get_game_data(search_game, event->member->user->id, "level");
  ERROR_INTERACTION((search_game == NULL), "Game data couldn't be retrieved!");

  struct sd_game game = (struct sd_game) 
  {
    .level = u_strtol( PQgetvalue(search_game, 0, 0) )
  };
  PQclear(search_game);

  game.biome = (game.level -1) % BIOME_SIZE;

  player.button_idx = event->data->custom_id[1] -48;

  int success_status = 0;
  
  // if the player is asking to steal, search for targets first!
  if (player.button_idx < 6)
  {
    PGresult* check_targets = (PGresult*) { 0 };
    check_targets = SQL_query(check_targets, 
        "select user_id from public.player where treasury > 0 and user_id != %ld",
        event->member->user->id);

    if (PQntuples(check_targets) == 0)
    {
      error_message(event, "There appears to be no targets to steal from!");
      PQclear(check_targets);
      return;
    }

    success_status = steal(event, &message.footer, &player);
  }
  else
    set_footer(&message.footer, icons[ICON_ALERT].path, "Welcome to the Treasury!");

  list_info(&message.fields, &player, success_status);
  set_buttons(event, &message.buttons, &player);

  char* offset_arrow = ICON_EMOJI(ICON_OFFSET_ARROW);

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player.color,
    .author = set_author(&message.author, event->member->user),
    .title = set_title(&message.title, "Treasury"),
    .thumbnail = set_thumbnail(&message.thumbnail, squirrels[player.squirrel].file.path),

    .description = set_description(&message.description, 
        " %s Steal golden acorns from other players' treasuries to fill your own for bonus stash completion rewards! \n"
        " %s When there are more than **4** golden acorns present, your stash completions are doubled!",
        offset_arrow, offset_arrow),

    .fields = &(struct discord_embed_fields) {
      .array = message.fields.array,
      .size = 1 +1
    },
    .footer = &message.footer.src 
  };

  struct sd_util_buttons util_buttons = { 0 };
  set_util_buttons(&util_buttons, event, &player, &game);

  struct sd_info_buttons info_buttons = { 0 };
  set_info_buttons(&info_buttons, event, &game);

  struct discord_component action_rows[3] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = &message.buttons.button,
        .size = 1
      }
    },
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
        .size = 3
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
}