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

enum SQ_TOPIC {
  SQ_TOPIC_TRADITIONAL,
  SQ_TOPIC_SPRING,
  SQ_TOPIC_SUMMER,
  SQ_TOPIC_FALL,
  SQ_TOPIC_WINTER,
  SQ_TOPIC_SIZE
};

struct sd_fields
{
  struct sd_field content[4 +1];           // +1 offset for balance field
  struct discord_embed_field array[4 +1];
};

struct sd_buttons
{
  struct sd_button content[4];
  struct sd_emoji emojis[4];
  struct discord_component array[4];
};

struct sd_squirrel_pages
{
  struct sd_button content[5];
  struct sd_emoji reset_emoji;
  struct discord_component array[5];
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
  struct sd_squirrel_pages page_buttons;
};

/*
  FILE DETAILS:
    * purchasing, selecting, or reseting player squirrel
    
    * squirrel page format:
      <command id><button idx><page num>.<user id>

  * if page button is selected:
    * if button_idx == 2:
      - reset squirrel
    * else:
      - turn page
  
  * if squirrel is selected:
    - squirrel = squirrels[page*idx * button_idx]

  * if squirrel is not purchased:
    - purchase squirrel
  * else 
    - set squirrel
*/

static inline void purchase_squirrel(const struct discord_interaction *event, struct sd_footer*footer, struct sd_player *player, int page_idx)
{
  int command_id = event->data->custom_id[0];

  PGresult *update_player = (PGresult*) { 0 };

  if (command_id == TYPE_PAGE_SQUIRRELS && player->button_idx == 2)
  {
    player->squirrel = GRAY_SQUIRREL;

    update_player = SQL_query(update_player, "update public.player set squirrel = %d where user_id = %ld",
        player->squirrel, event->member->user->id);
    
    PQclear(update_player);

    set_footer(footer, squirrels[GRAY_SQUIRREL].file.path, 
        "Your squirrel has been reset!");
  }
  // then a squirrel was purchased or selected
  else if (command_id == TYPE_SQUIRRELS && player->button_idx < 4)
  {
    int squirrel_idx = 4 * page_idx + player->button_idx +1;

    struct sd_squirrel_file squirrel_data = squirrels[squirrel_idx].file;

    // if a squirrel is being purchased
    if (page_idx != SQ_TOPIC_TRADITIONAL && (player->purchased >> squirrel_idx & 1) == 0) 
    {
      player->purchased |= (1 << squirrel_idx);
      player->conjured_acorns -= squirrels[squirrel_idx].cost;

      update_player = SQL_query(update_player, "update public.player set purchased = %d, conjured_acorns = %d where user_id = %ld",
          player->purchased, player->conjured_acorns, event->member->user->id);
      
      PQclear(update_player);

      set_footer(footer, squirrel_data.path, 
          "You have purchased the %s!", squirrel_data.name);

      return;
    }

    player->squirrel = squirrel_idx;

    update_player = SQL_query(update_player, "update public.player set squirrel = %d where user_id = %ld",
        player->squirrel, event->member->user->id);
    
    PQclear(update_player);

    set_footer(footer, squirrel_data.path, 
        "You have selected the %s!", squirrel_data.name);

  }
  else {
    set_footer(footer, icons[ICON_ALERT].path, 
        "Welcome to the Designer Squirrels Shop!");
  }

}

static inline void set_traditional_squirrel_info(struct sd_fields *fields, struct sd_player *player, struct sd_game *game, int page_idx)
{
  char* indent = ICON_EMOJI(ICON_INDENT);
  char* offset_arrow = ICON_EMOJI(ICON_OFFSET_ARROW);
  char* stahr = ICON_EMOJI(ICON_STAHR);

  fields->array[0] = (struct discord_embed_field)
  {
    .name = set_field_name(&fields->content[0], "Progress"),
    .value = set_field_value(&fields->content[0], "%s %s Level: **%d**",
      indent, stahr, game->level)
  };

  if (player->highest_level > 0)
  {
    LABEL highest_level_buf = { 0 };
    char* highest_level = num_str(&highest_level_buf, player->highest_level);

    u_snprintf(fields->array[0].value, VALUE_SIZE, " (%s **%s**)",
        ICON_EMOJI(ICON_CROWN), highest_level);
  }

  for (int field_idx = 1; field_idx < 4 +1; field_idx++)
  {
    int squirrel_idx = 4 * page_idx + field_idx; // no +1 because field_idx starts at 1
    struct sd_squirrel_file squirrel_data = squirrels[squirrel_idx].file;

    fields->array[field_idx] = (struct discord_embed_field)
    {
      .name = set_field_name(&fields->content[field_idx], "%s %s",
        SQUIRREL_EMOJI(squirrel_idx), squirrel_data.name)
    };

    if (player->highest_level < squirrels[squirrel_idx].cost)
    {
      LABEL squirrel_req_buf = { 0 };
      char* squirrel_req = num_str(&squirrel_req_buf, squirrels[squirrel_idx].cost);

      fields->array[field_idx].value = set_field_value(&fields->content[field_idx],
          " %s %s \n"
          " %s *Requires* **%s** %s Levels", 
          offset_arrow, squirrel_data.description, 
          indent, squirrel_req, stahr);
    }
    else {
      fields->array[field_idx].value = set_field_value(&fields->content[field_idx],
          " %s %s \n", 
          offset_arrow, squirrel_data.description);
    }
  }
}

static inline void set_squirrel_info(struct sd_fields *fields, struct sd_player *player, int page_idx)
{
  LABEL conjured_acorns_buf = { 0 };
  char* conjured_acorns = num_str(&conjured_acorns_buf, player->conjured_acorns);

  char* indent = ICON_EMOJI(ICON_INDENT);
  char* conjured_acorn = CACHE_ITEM_EMOJI(TYPE_CONJURED_ACORNS);
  char* offset_arrow = ICON_EMOJI(ICON_OFFSET_ARROW);

  fields->array[0] = (struct discord_embed_field)
  {
    .name = set_field_name(&fields->content[0], "Balance"),
    .value = set_field_value(&fields->content[0], "%s %s Conjured Acorns: **%s**",
      indent, conjured_acorn, conjured_acorns)
  };

  for (int field_idx = 1; field_idx < 4 +1; field_idx++)
  {
    int squirrel_idx = 4 * page_idx + field_idx; // no +1 because field_idx starts at 1
    struct sd_squirrel_file squirrel_data = squirrels[squirrel_idx].file;

    fields->array[field_idx] = (struct discord_embed_field)
    {
      .name = set_field_name(&fields->content[field_idx], "%s %s",
        SQUIRREL_EMOJI(squirrel_idx), squirrel_data.name)
    };

    // if the player has yet to purchase the squirrel
    if ((player->purchased >> squirrel_idx & 1) == 0)
    {
      LABEL squirrel_req_buf = { 0 };
      char* squirrel_req = num_str(&squirrel_req_buf, squirrels[squirrel_idx].cost);

      fields->array[field_idx].value = set_field_value(&fields->content[field_idx],
          " %s %s \n"
          " %s *Costs* **%s** %s Conjured Acorns", 
          offset_arrow, squirrel_data.description, 
          indent, squirrel_req, conjured_acorn);

    }
    else {
      fields->array[field_idx].value = set_field_value(&fields->content[field_idx],
          " %s %s \n"
          " %s %s *Purchased*", 
          offset_arrow, squirrel_data.description,
          indent, ICON_EMOJI(ICON_ALERT) );
    }
  }
}

static inline void set_squirrel_buttons(const struct discord_interaction *event, struct sd_buttons *buttons, struct sd_player *player, int page_idx)
{
  for (int button_idx = 0; button_idx < 4; button_idx++) 
  {
    int squirrel_idx = 4 * page_idx + button_idx +1; // +1 from base squirrel
    struct sd_squirrel_file squirrel_data = squirrels[squirrel_idx].file;

    buttons->array[button_idx] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .custom_id = set_custom_id(&buttons->content[button_idx], "%c%d%d.%ld",
        TYPE_SQUIRRELS, button_idx, page_idx, event->member->user->id),
      .emoji = set_button_emoji(&buttons->emojis[button_idx], squirrel_data.emoji)
    };

    // if player has yet to purchase the squirrel
    if ((player->purchased >> squirrel_idx & 1) == 0)
    {
      buttons->array[button_idx].label = set_label(&buttons->content[button_idx], "Purchase");

      set_button_status(&buttons->array[button_idx], player->conjured_acorns < squirrels[squirrel_idx].cost);
    }
    else
    {
      buttons->array[button_idx].label = set_label(&buttons->content[button_idx], squirrel_data.name);
    
      set_button_active(&buttons->array[button_idx], squirrel_idx == player->squirrel);
    }
  }
}

static inline void set_traditional_squirrel_buttons(const struct discord_interaction *event, struct sd_buttons *buttons, struct sd_player *player, struct sd_game *game, int page_idx)
{
  for (int button_idx = 0; button_idx < 4; button_idx++) 
  {
    int squirrel_idx = 4 * page_idx + button_idx +1; // +1 from base squirrel
    struct sd_squirrel_file squirrel_data = squirrels[squirrel_idx].file;

    buttons->array[button_idx] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .label = set_label(&buttons->content[button_idx], squirrel_data.name),
      .custom_id = set_custom_id(&buttons->content[button_idx], "%c%d%d.%ld",
        TYPE_SQUIRRELS, button_idx, page_idx, event->member->user->id),
      .emoji = set_button_emoji(&buttons->emojis[button_idx], squirrel_data.emoji)
    };

    /*
      if insufficient funds ? disable + SECONDARY
      if the squirrel selected ? disable + SUCCESS
      if the squirrel is available ? enable + PRIMARY
    */

    if (player->highest_level < squirrels[squirrel_idx].cost && game->level < squirrels[squirrel_idx].cost)
    {
      buttons->array[button_idx].style = DISCORD_BUTTON_SECONDARY;
      buttons->array[button_idx].disabled = true;
    }
    else if (squirrel_idx == player->squirrel)
    {
      buttons->array[button_idx].style = DISCORD_BUTTON_SUCCESS;
      buttons->array[button_idx].disabled = true;
    }
    else {
      buttons->array[button_idx].style = DISCORD_BUTTON_PRIMARY;
    }
  }
}

static inline void set_squirrel_page_buttons(const struct discord_interaction *event, struct sd_squirrel_pages *page_buttons, struct sd_player *player, int page_idx)
{
  page_buttons->array[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[0], "%c00.%ld",
        TYPE_PAGE_SQUIRRELS, event->member->user->id),
    .label = set_label(&page_buttons->content[0], "⏮️")
  };

  set_button_status(&page_buttons->array[0], page_idx == 0);

  page_buttons->array[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[1], "%c1%d.%ld", // button_idx is fixed!
        TYPE_PAGE_SQUIRRELS, (page_idx > 0) ? page_idx -1 : 0, event->member->user->id),
    .label = set_label(&page_buttons->content[1], "⏪")
  };

  set_button_status(&page_buttons->array[1], page_idx == 0);

  page_buttons->array[2] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[2], "%c2%d.%ld",
        TYPE_PAGE_SQUIRRELS, page_idx, event->member->user->id),
    .label = set_label(&page_buttons->content[2], "Reset Squirrel"),
    .emoji = set_button_emoji(&page_buttons->reset_emoji, icons[ICON_HELP].emoji)
  };

  set_button_status(&page_buttons->array[2], player->squirrel == 0);

  page_buttons->array[3] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[3], "%c3%d.%ld",
        TYPE_PAGE_SQUIRRELS, (page_idx < SQ_TOPIC_SIZE) ? page_idx +1 : SQ_TOPIC_SIZE -1, event->member->user->id),
    .label = set_label(&page_buttons->content[3], "⏩")
  };

  set_button_status(&page_buttons->array[3], page_idx == SQ_TOPIC_SIZE -1);

  page_buttons->array[4] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[4], "%c4%d.%ld",
        TYPE_PAGE_SQUIRRELS, SQ_TOPIC_SIZE -1, event->member->user->id),
    .label = set_label(&page_buttons->content[4], "⏭️")
  };

  set_button_status(&page_buttons->array[4], page_idx == SQ_TOPIC_SIZE -1);
}

void squirrels_interaction(const struct discord_interaction *event)
{
  struct sd_message message = { 0 };

  PGresult* search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, event->member->user->id, 
      "color, squirrel, purchased, conjured_acorns, highest_level, t_stamp");
  ERROR_INTERACTION((search_player == NULL), "Player data couldn't be retrieved!");

  struct sd_player player = (struct sd_player) 
  {
    .color = u_strtol( PQgetvalue(search_player, 0, 0) ),

    .squirrel = u_strtol( PQgetvalue(search_player, 0, 1) ),
    .purchased = u_strtol( PQgetvalue(search_player, 0, 2) ),
    .conjured_acorns = u_strtol( PQgetvalue(search_player, 0, 3) ),
    .highest_level = u_strtol( PQgetvalue(search_player, 0, 4) ),

    .timestamp = u_strtol( PQgetvalue(search_player, 0, 5) )
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

  char* seasons[5] = {"Traditional", "Spring", "Summer", "Fall", "Winter"};

  // squirrels will always be initiated from a button press!
  // indexes 1 and 2 are numbers!
  player.button_idx = event->data->custom_id[1] -48;
  int page_num = event->data->custom_id[2] -48;

  char* offset_arrow = ICON_EMOJI(ICON_OFFSET_ARROW);

  purchase_squirrel(event, &message.footer, &player, page_num);

  if (page_num == SQ_TOPIC_TRADITIONAL)
  {
    set_description(&message.description, 
        " %s Traditional squirrels become accessible with a certain high score! \n"
        " %s If you restart, the custom will still be active. \n"
        " %s The appearance of your squirrel is cosmetic.",
        offset_arrow, offset_arrow, offset_arrow);

    set_traditional_squirrel_info(&message.fields, &player, &game, page_num);
    set_traditional_squirrel_buttons(event, &message.buttons, &player, &game, page_num);
  }
  else {
    char* conjured_acorn = CACHE_ITEM_EMOJI(TYPE_CONJURED_ACORNS);

    set_description(&message.description, 
        " %s Purchase a squirrel custom with %s *conjured acorns*! \n"
        " %s If you restart, the custom will still be active. \n"
        " %s The appearance of your squirrel is cosmetic.",
        offset_arrow, conjured_acorn, offset_arrow, offset_arrow);

    set_squirrel_info(&message.fields, &player, page_num);
    set_squirrel_buttons(event, &message.buttons, &player, page_num);
  }

  set_squirrel_page_buttons(event, &message.page_buttons, &player, page_num);

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player.color,
    .author = set_author(&message.author, event->member->user),
    .title = set_title(&message.title, "Designer Squirrels (%s List)", seasons[page_num]),
    .thumbnail = set_thumbnail(&message.thumbnail, squirrels[player.squirrel].file.path),

    .description = message.description.value,

    .fields = &(struct discord_embed_fields) {
      .array = message.fields.array,
      .size = 4 +1
    },
    .footer = &message.footer.src 
  };

  struct sd_util_buttons util_buttons = { 0 };
  set_util_buttons(&util_buttons, event, &player, &game);

  struct sd_info_buttons info_buttons = { 0 };
  set_info_buttons(&info_buttons, event, &game);

  struct discord_component action_rows[4] = {
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = message.buttons.array,
        .size = sizeof(message.buttons.array)/sizeof(*message.buttons.array)
      }
    },
    {
      .type = DISCORD_COMPONENT_ACTION_ROW,
      .components = &(struct discord_components) {
        .array = message.page_buttons.array,
        .size = sizeof(message.page_buttons.array)/sizeof(*message.page_buttons.array)
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
        .size = 4
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
}