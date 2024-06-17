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

/*
  FILE DETAILS:
    * displays help information based on page index from custom id[2]
*/

enum HELP_TOPIC
{
  HELP_BASICS,
  HELP_GAMEPLAY,
  HELP_ITEMS,
  HELP_MESSAGE_COLOR,

  HELP_REPORTING_ISSUES,
  HELP_TOPIC_SIZE
};

struct sd_fields
{
  struct sd_field content;
  struct discord_embed_field field;
};

struct sd_message
{
  struct sd_author author;
  struct sd_title title;
  struct sd_thumbnail thumbnail;
  struct sd_footer footer;

  struct sd_fields fields;
};

static inline void detect_topic(struct sd_fields *fields, int page_idx)
{
  char* bullet = ICON_EMOJI(ICON_BULLET);
  char* acorn = CACHE_ITEM_EMOJI(TYPE_ACORNS);
  char* conjured_acorn = CACHE_ITEM_EMOJI(TYPE_CONJURED_ACORNS);

  switch (page_idx)
  {
    case HELP_BASICS:
      fields->field = (struct discord_embed_field)
      {
        .name = set_field_name(&fields->content, "%s Bot Controls", acorn),
        .value = set_field_value(&fields->content,
              "%s Each button (I, II, and III) translates the current piece to its respective stash. \n"
              "%s Rotate the current piece *clockwise* (CW) or *counterclockwise* (CCW). \n"
              "%s Looking for info on another player? View their profile under `Apps` and press `Player`.",
              bullet, bullet, bullet)
      };
      break;
    case HELP_GAMEPLAY:
      char* acorn_count = CACHE_ITEM_EMOJI(TYPE_ACORN_COUNT);
      
      fields->field = (struct discord_embed_field)
      {
        .name = set_field_name(&fields->content, "%s Gameplay", acorn),
        .value = set_field_value(&fields->content,
            "%s Earn %s *acorn count* and level up to the next biome by completing or filling a stash. \n"
            "%s Disasters have a chance to strike one of your stashes! \n"
            "%s Encounters have a chance to give a new piece instead of your next piece. \n"
            "%s Threat increases with every piece placed. Increased threat causes more frequent encounters! \n"
            "%s Encounters also have a chance to unlock an element of the biome's story! See `Biome Story` for details.",
            bullet, acorn_count,
            bullet,
            bullet,
            bullet,
            bullet)
      };
      break;
    case HELP_ITEMS:
      char* golden_acorn = CACHE_ITEM_EMOJI(TYPE_GOLDEN_ACORNS);
      char* leafy_acorn = CACHE_ITEM_EMOJI(TYPE_LEAFY_ACORN);

      char* seeds = CACHE_ITEM_EMOJI(TYPE_SPRING_SEEDS);
      char* cherries = CACHE_ITEM_EMOJI(TYPE_SUMMER_CHERRIES);
      char* blueberries = CACHE_ITEM_EMOJI(TYPE_FALL_BLUEBERRIES);

      fields->field = (struct discord_embed_field)
      {
        .name = set_field_name(&fields->content, "%s Item Types", acorn),
        .value = set_field_value(&fields->content,
            "%s %s *Acorn*, %s *golden acorn*, and %s *conjured acorn* are the primary score builders. \n"
            "%s %s *Leafy acorns* will upgrade adjacent slots and turn into a %s *golden acorn*. \n"
            "%s Find %s *seeds*, %s *cherries*, or %s *blueberries* in pieces for additional boosts!",
            bullet, acorn, golden_acorn, conjured_acorn,
            bullet, leafy_acorn, golden_acorn,
            bullet, seeds, cherries, blueberries,
            bullet)
      };
      break;
    case HELP_MESSAGE_COLOR:
      fields->field = (struct discord_embed_field)
      {
        .name = set_field_name(&fields->content, "%s Changing Message Color", acorn),
        .value = set_field_value(&fields->content,
            "%sEmbed color can be changed by inputting a hex value into "COLOR_ID". \n"
            "%s Looking for a hex color? Check out this [**link**](https://htmlcolorcodes.com/color-picker/) for reference!",
            bullet, bullet)
      };
      break;
    case HELP_REPORTING_ISSUES:
      fields->field = (struct discord_embed_field)
      {
        .name = set_field_name(&fields->content, "%s Reporting Issues", acorn),
        .value = set_field_value(&fields->content,
            "%s Have a burning question or experience a bug? Feel free to join the [support server](https://discord.gg/RhhapTYeBN)! \n"
            "%s If a bug was found, please screenshot the message immediately before reporting it! Otherwise, the bug may not be handled. \n"
            "%s Please refer to the [license](https://github.com/Furmissile/sq.dash/blob/main/LICENSE) to use this project. "
                "If unauthorized copies are encountered, please report it to <#1195785157177782292> [here](https://discord.gg/RhhapTYeBN)!",
            bullet, bullet, bullet)
      };
      break;
  }

}

void help_interaction(const struct discord_interaction *event)
{
  struct sd_message message = { 0 };

  PGresult* search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, event->member->user->id, 
      "color, squirrel, t_stamp");
  ERROR_INTERACTION((search_player == NULL), "Player data couldn't be retrieved!");

  struct sd_player player = (struct sd_player) 
  {
    .color = u_strtol( PQgetvalue(search_player, 0, 0) ),
    .squirrel = u_strtol( PQgetvalue(search_player, 0, 1) ),

    .timestamp = u_strtol( PQgetvalue(search_player, 0, 2) )
  };
  PQclear(search_player);

  PGresult* search_game = (PGresult*) { 0 };
  search_game = get_game_data(search_game, event->member->user->id, "level");
  ERROR_INTERACTION((search_game == NULL), "Game data couldn't be retrieved!");

  struct sd_game game = (struct sd_game) 
  {
    .level = u_strtol( PQgetvalue(search_game, 0, 0) )
  };
  PQclear(search_game);

  game.biome = (game.level -1) % BIOME_SIZE;

  int page_idx = event->data->custom_id[2] -48;

  detect_topic(&message.fields, page_idx);

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player.color,
    .author = set_author(&message.author, event->member->user),
    .title = set_title(&message.title, "Help (%d of %d)", page_idx +1, HELP_TOPIC_SIZE),
    .thumbnail = set_thumbnail(&message.thumbnail, squirrels[player.squirrel].file.path),

    .fields = &(struct discord_embed_fields) {
      .array = &message.fields.field,
      .size = 1
    },

    .footer = set_footer(&message.footer, icons[ICON_ALERT].path, "Welcome to help info!") 
  };

  struct sd_page_buttons page_buttons = { 0 };
  set_page_buttons(&page_buttons, event, TYPE_HELP, HELP_TOPIC_SIZE);

  struct sd_util_buttons util_buttons = { 0 };
  set_util_buttons(&util_buttons, event, &player, &game);

  struct sd_info_buttons info_buttons = { 0 };
  set_info_buttons(&info_buttons, event, &game);

  struct discord_component action_rows[3] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = page_buttons.array,
        .size = sizeof(page_buttons.array)/sizeof(*page_buttons.array)
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