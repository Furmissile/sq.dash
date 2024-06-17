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
    * processes all relevant game interactions under "Forage" button

    * if rotating piece:
      * check rotation direction in custom id[2]

    * restart needs to be selected twice consecutively to verify
    * if restart:
      * if restart == 1:
        - reset game
        end
      * set restart to 1
      
    * if adding a piece:
      * try level up
      * if encounter response:
        - try obtaining context (if chanced and undiscovered)
      * current is moved to selected stash
      * clear current piece
      * check the following:
        * clear disaster debris
        * leafy acorn trees (check first so completion doesnt override acorn)
        * is complete
      * try biome event:
        !! Encounter is based on threat level and increases per biome
        * if can do encounter:
          - set encounter piece to current piece
          end
        * otherwise if can do disaster:
          - select random piece, orientation and stash (that isnt the selected stash)
          - fill piece with biome disaster type
          - disaster piece is moved to stash
          end
      * next is moved to current
      * select a new next piece
*/

struct sd_fields
{
  struct sd_field content[3 +3 +3];           // +3 for progress/threat/update status field
  struct discord_embed_field array[3 +3 +3];
};

struct sd_buttons
{
  struct sd_button content[3 +1];         // +1 for restart button
  struct sd_emoji stash_emoji;            // only on type of emoji is used for all stashes!
  struct discord_component array[3 +1];
};

struct sd_rotate_buttons
{
  struct sd_button content[2];
  struct discord_component array[2];
};

struct sd_message
{
  struct sd_author author;
  struct sd_title title;
  struct sd_thumbnail thumbnail;
  struct sd_description description;
  struct sd_image image;

  struct sd_fields fields;

  struct sd_buttons buttons;
  struct sd_rotate_buttons rotate_buttons;
  struct sd_button restart_button;

  int encounter;
  int section;
};

/* DB FUNCTIONS */
static inline void set_game(const struct discord_interaction *event, struct sd_game *game)
{
  PGresult *search_game = (PGresult*) { 0 };
  search_game = get_game_data(search_game, event->member->user->id, 
      "level, progress, piece_count, unnest(shuffle), biome_time, encounter, section, is_restart, disaster_stash, "
      "unnest(first_stash), unnest(second_stash), unnest(third_stash), unnest(current_piece), unnest(next_piece)");
  ERROR_INTERACTION((search_game == NULL), "Game data couldn't be retrieved!");

  *game = (struct sd_game) 
  {
    .level = u_strtol( PQgetvalue(search_game, 0, 0) ),
    .progress = u_strtol( PQgetvalue(search_game, 0, 1) ),
    .piece_count = u_strtol( PQgetvalue(search_game, 0, 2) ),
    .shuffle = PGscan_shuffle(search_game, 3),
    .biome_time = u_strtol( PQgetvalue(search_game, 0, 4) ),
    .encounter = u_strtol( PQgetvalue(search_game, 0, 5) ),
    .section = u_strtol( PQgetvalue(search_game, 0, 6) ),
    .is_restart = u_strtol( PQgetvalue(search_game, 0, 7) ),
    .disaster_stash = u_strtol( PQgetvalue(search_game, 0, 8) ),

    .stashes = {
      PGscan_stash(search_game, 9),
      PGscan_stash(search_game, 10),
      PGscan_stash(search_game, 11),
    },

    .current_piece = PGscan_stash(search_game, 12),
    .next_piece = PGscan_stash(search_game, 13)
  };
  PQclear(search_game);

  game->biome = (game->level -1) % BIOME_SIZE;

  game->threat = (game->biome_time/TIME_INTERVAL);
  game->encounter_increase = (game->biome_time/TIME_INTERVAL) * ENCOUNTER_INCREASE;
}

static inline void update_game(const struct discord_interaction *event, struct sd_game *game)
{
  format_db_stash(&game->current_piece);
  format_db_stash(&game->next_piece);
  format_db_shuffle(&game->shuffle);

  for (int stash_idx = 0; stash_idx < MAX_STASHES; stash_idx++)
    format_db_stash(&game->stashes[stash_idx]);

  PGresult *update_game = (PGresult*) { 0 };

  update_game = SQL_query(update_game, "update public.game set "
      "progress = %d, "
      "level = %d, "
      "piece_count = %d, "
      "shuffle = '%s', "
      "biome_time = %d, "
      "encounter = %d, "
      "section = %d, "
      "is_restart = %d, "
      "first_stash = '%s', "
      "second_stash = '%s', "
      "third_stash = '%s', "
      "current_piece = '%s', "
      "next_piece = '%s', "
      "disaster_stash = %d "
      "where user_id = %ld",
      game->progress, game->level, game->piece_count, game->shuffle.db_format, game->biome_time,
      game->encounter, game->section, game->is_restart, game->stashes[0].db_format,
      game->stashes[1].db_format, game->stashes[2].db_format, game->current_piece.db_format,
      game->next_piece.db_format, game->disaster_stash,
      event->member->user->id);
  
  PQclear(update_game);
}

static inline void complete_encounter(const struct discord_interaction *event, struct sd_game *game)
{
  char* biome_names[BIOME_SIZE] = { "grasslands", "seeping_sands", "nature_end", "death_grip", "last_acorn" };

  int encounter_biome = (game->biome - game->is_new_biome) % BIOME_SIZE;

  // story data exists if player data exists! which it always should by this point
  PGresult *search_story = (PGresult*) { 0 };
  search_story = SQL_query(search_story, "select %s from public.biome_story where user_id = %ld",
      biome_names[encounter_biome], event->member->user->id);
  
  int biome_value = u_strtol( PQgetvalue(search_story, 0, 0) );
  PQclear(search_story);

  int encounter_idx = game->encounter;
  for (int section_idx = 0; section_idx < game->section; section_idx++)
  {
    // ensure encounter idx is only scanning over player's current section
    encounter_idx += (biomes[encounter_biome].sections[section_idx].encounters_size -1);
  }

  // if encounter idx bit is on or player didn't chance info, this encounter has been seen!
  if ((biome_value >> encounter_idx & 0x01) == 1)
    return;

  // now assume the encounter hasn't been seen!
  game->is_biome_story = 1;

  // switch encounter idx bit on
  biome_value |= (1 << encounter_idx);

  search_story = SQL_query(search_story, "update public.biome_story set %s = %d where user_id = %ld",
      biome_names[encounter_biome], biome_value, event->member->user->id);
  PQclear(search_story);
}

/* MESSAGE ACTIONS */
static inline void add_piece(const struct discord_interaction *event, struct sd_player *player, struct sd_game *game)
{
  if (event->data->custom_id[2] == TYPE_RESTART)
  {
    game->is_restart = 1;
    return;
  }

  merge_caches(&game->stashes[player->button_idx], &game->current_piece);

  memset(game->current_piece.cache, TYPE_EMPTY, sizeof(game->current_piece.cache));

  for (int stash_idx = 0; stash_idx < MAX_STASHES; stash_idx++)
  {
    CACHE *current_stash = &game->stashes[stash_idx];

    // consider TYPE_TREE FIRST so leafy acorn doesnt instantly turn into a tree

    // turn any leafy acorns into tree
    int tree_idx = search_type(current_stash, TYPE_TREE);
    if (tree_idx > ERROR_STATUS)
      use_leafy_acorn(current_stash, tree_idx);

    int leafy_acorn_idx = search_type(current_stash, TYPE_LEAFY_ACORN);
    if (leafy_acorn_idx > ERROR_STATUS)
      current_stash->cache[leafy_acorn_idx] = TYPE_TREE;

    if (check_complete_stash(current_stash) == 1)
    {
      reward_stash(current_stash, 0, SLOTS_SIZE);

      game->progress += current_stash->value;
      player->conjured_acorns += current_stash->conjured_acorns;
      set_daily(event, player);
    }

  }

  PGresult *update_player = (PGresult*) { 0 };
  update_player = SQL_query(update_player, "update public.player set conjured_acorns = %d where user_id = %ld",
      player->conjured_acorns, event->member->user->id);
  PQclear(update_player);
}

static inline void rotate_piece(const struct discord_interaction *event, struct sd_game *game)
{
  if (event->data->custom_id[2] == TYPE_ROTATE_CW)
    rotate_piece_cw(&game->current_piece);
  else 
    rotate_piece_ccw(&game->current_piece);

  format_db_stash(&game->current_piece);

  PGresult *update_stash = (PGresult*) { 0 };
  update_stash = SQL_query(update_stash, "update public.game set current_piece = '%s' where user_id = %ld",
      game->current_piece.db_format, event->member->user->id);
  PQclear(update_stash);
}

static inline void restart_game(const struct discord_interaction *event, struct sd_player *player, struct sd_game *game)
{
  PGresult *end_game = (PGresult*) { 0 };
  if (game->is_restart == 1)
  {
    // set highest biome FIRST
    if (game->level > player->highest_level)
    {
      player->highest_level = game->level;
      end_game = SQL_query(end_game, "update public.player set highest_level = %d where user_id = %ld",
          player->highest_level, event->member->user->id);
      PQclear(end_game);
    }

    // then reset game!
    end_game = SQL_query(end_game, "delete from public.game where user_id = %ld",
        event->member->user->id);
    PQclear(end_game);

    set_game(event, game);

    return;
  }
  
  game->is_restart++;
  end_game = SQL_query(end_game, "update public.game set is_restart = %d where user_id = %ld",
      game->is_restart, event->member->user->id);
  PQclear(end_game);
}

static inline void check_stashes(struct sd_game *game)
{
  // any biome disaster must be cleared immediately!
  if (game->disaster_stash > ERROR_STATUS)
  {
    clear_type(&game->stashes[game->disaster_stash], TYPE_CURSE + game->biome);
    game->disaster_stash = ERROR_STATUS;
  }

  // check each stash for magic acorn or clear stash debris
  for (int stash_idx = 0; stash_idx < MAX_STASHES; stash_idx++)
  {
    if (search_type(&game->stashes[stash_idx], TYPE_POP) > ERROR_STATUS)
      clear_type(&game->stashes[stash_idx], TYPE_POP);
  }
}

static inline void try_level_up(struct sd_player *player, struct sd_game *game)
{
  int req_progress = BASE_BIOME + ((game->level -1) * BIOME_INTERVAL);

  if (game->progress < req_progress)
    return;

  game->progress -= req_progress;
  game->level++;
  game->biome = (game->level -1) % BIOME_SIZE;
  game->is_new_biome = 1;
  game->biome_time = 0;
  game->threat = 0;

  // clear what is left in every stash
  for (int stash_idx = 0; stash_idx < MAX_STASHES; stash_idx++)
  {
    CACHE *current_stash = &game->stashes[stash_idx];

    reward_stash(current_stash, 0, SLOTS_SIZE);

    if (stash_idx != player->button_idx)
    {
      current_stash->value = 0;
      current_stash->conjured_acorns = 0;
    }

  }
}

static inline void detect_event(const struct discord_interaction *event, struct sd_message *message, struct sd_player *player, struct sd_game *game)
{
  // [2] is for specifying command further!
  switch (event->data->custom_id[2])
  {
    case TYPE_ROTATE_CW:
    case TYPE_ROTATE_CCW:
      rotate_piece(event, game);
      break;
    case TYPE_RESTART:
      restart_game(event, player, game);
      break;
    default: // add piece
      add_piece(event, player, game);

      try_level_up(player, game);

      if (game->encounter == ERROR_STATUS
        && game->is_new_biome == 0
        && rand() % MAX_CHANCE > (EVENT_CHANCE - game->encounter_increase))
      {
        spawn_encounter(game);
      }
      else {
        if (rand() % MAX_CHANCE > DISASTER_CHANCE)
          spawn_biome_disaster(player, game);

        shift_pieces(game);

        if (game->encounter > ERROR_STATUS)
        {
          // try story
          if (rand() % MAX_CHANCE > EVENT_CHANCE)
          {
            complete_encounter(event, game);

            // retain encounter and section for printing biome story
            message->encounter = game->encounter;
            message->section = game->section;
          }

          game->encounter = ERROR_STATUS;
          game->section = ERROR_STATUS;
        }
      }

      if (game->biome_time < MAX_BIOME_TIME)
      {
        game->biome_time++;
        game->threat = (game->biome_time/TIME_INTERVAL);
      }

      break;
  }
}

/* MESSAGE FUNCTIONS */
static inline void build_threat_bar(struct sd_field *field, struct sd_game *game)
{
  int threat_level = 0;

  char* corrupt_acorn = ICON_EMOJI(ICON_CORRUPT_ACORN);
  for (; threat_level < game->threat; threat_level++)
  {
    u_snprintf(field->value, VALUE_SIZE, corrupt_acorn);
  }

  char* acorn = CACHE_ITEM_EMOJI(TYPE_ACORNS);
  for (; threat_level < BAR_SIZE; threat_level++)
  {
    u_snprintf(field->value, VALUE_SIZE, acorn);
  }
}

static inline void set_info(struct sd_message *message, struct sd_player *player, struct sd_game *game)
{
  struct sd_fields *fields = &message->fields;

  char* indent = ICON_EMOJI(ICON_INDENT);
  char* alert = ICON_EMOJI(ICON_ALERT);
  char* stahr = ICON_EMOJI(ICON_STAHR);

  fields->array[0] = (struct discord_embed_field)
  {
    .name = set_field_name(&fields->content[0], "Progress")
  };

  u_snprintf(fields->content[0].value, VALUE_SIZE, 
      "%s %s Level: **%d**",
      indent, stahr, game->level);

  if (player->highest_level > 0)
  {
    char* crown = ICON_EMOJI(ICON_CROWN);
    
    u_snprintf(fields->content[0].value, VALUE_SIZE, 
        " (%s**%d**)",
        crown, player->highest_level);
  }

  build_progress_bar(&fields->content[0], game);

  fields->array[0].value = fields->content[0].value;

  fields->array[1] = (struct discord_embed_field)
  {
    .name = set_field_name(&fields->content[1], "%s Threat", alert)
  };

  build_threat_bar(&fields->content[1], game);

  fields->array[1].value = fields->content[1].value;
  
  if (game->is_new_biome == 1)
  {
    struct sd_biome_file next_biome_data = biomes[game->biome].file;

    u_snprintf(fields->content[2].value, VALUE_SIZE, 
        "\n\n%s You have reached %s **%s**!",
        alert, fetch_emoji(next_biome_data.emoji), next_biome_data.name);
  }

  if (game->is_biome_story == 1)
  {
    // if player has reached next biome, story comes from the previous biome! (based on when encounter spawns)
    int story_biome = (game->biome - game->is_new_biome) % BIOME_SIZE;
    u_snprintf(fields->content[2].value, VALUE_SIZE, 
        "\n\nNew element of the story unlocked! \n"
        "%s \"*%s*\"", 
        alert, biomes[story_biome].sections[message->section].encounters[message->encounter].context);
  }

  if (player->daily_complete == 1)
  {
    char* conjured_acorn = CACHE_ITEM_EMOJI(TYPE_CONJURED_ACORNS);

    u_snprintf(fields->content[2].value, VALUE_SIZE,
      "\n\n%s Daily stashes complete!"
      "\n+**%d** %s", 
      ICON_EMOJI(ICON_TREASURE), DAILY_REWARD, conjured_acorn);
  }

  if (game->disaster_stash > ERROR_STATUS)
  {
    char* disaster_icon = CACHE_ITEM_EMOJI(TYPE_CURSE + game->biome);

    u_snprintf(fields->content[2].value, VALUE_SIZE,
        "\n\n %s *Incoming %s*!",
        disaster_icon, items[TYPE_CURSE + game->biome].file.name);
  }

  fields->array[2].value = fields->content[2].value;
}

static inline void set_game_info(struct sd_fields *fields, struct sd_game *game)
{
  fields->array[3] = (struct discord_embed_field)
  {
    .value = format_cache(&fields->content[3], &game->current_piece),
    .Inline = true
  };

  char* alert = ICON_EMOJI(ICON_ALERT);

  if (game->encounter == ERROR_STATUS)
  {
    fields->array[3].name = set_field_name(&fields->content[3], "__Current__");
  }
  else {
    fields->array[3].name = set_field_name(&fields->content[3], "__Encounter__(%s)", alert);
  }

  fields->array[4] = (struct discord_embed_field)
  {
    .name = set_field_name(&fields->content[4], " "),
    .value = set_field_value(&fields->content[4], " "),
    .Inline = true
  };

  fields->array[5] = (struct discord_embed_field)
  {
    .name = set_field_name(&fields->content[5], "__Next__"),
    .value = format_cache(&fields->content[5], &game->next_piece),
    .Inline = true
  };

  char* stash_icon = ICON_EMOJI(ICON_TREASURE);
  char* stash_names[3] = { "I", "II", "III" };

  for (int stash_idx = 0; stash_idx < MAX_STASHES; stash_idx++)
  {
    int field_idx = stash_idx +6;

    fields->array[field_idx] = (struct discord_embed_field)
    {
      .name = set_field_name(&fields->content[field_idx], 
          "%s %s",
          stash_icon, stash_names[stash_idx]),
      .value = format_cache(&fields->content[field_idx], &game->stashes[stash_idx]),
      .Inline = true
    };

    if (game->stashes[stash_idx].value > 0)
    {
      LABEL acorn_count_buf = { 0 };
      char* value = num_str(&acorn_count_buf, game->stashes[stash_idx].value);

      char* acorn_count = CACHE_ITEM_EMOJI(TYPE_ACORN_COUNT);

      u_snprintf(fields->content[field_idx].value, VALUE_SIZE,
          "\n+**%s** %s",
          value, acorn_count);
    }

    if (game->stashes[stash_idx].conjured_acorns > 0)
    {
      char* conjured_acorn = CACHE_ITEM_EMOJI(TYPE_CONJURED_ACORNS);

      u_snprintf(fields->content[field_idx].value, VALUE_SIZE,
          "\n+**%d** %s",
          game->stashes[stash_idx].conjured_acorns, conjured_acorn);
    }
  }

}

static inline void set_stash_buttons(const struct discord_interaction *event, struct sd_buttons *buttons, struct sd_game *game)
{
  char* stash_names[3] = { "I", "II", "III" };

  set_button_emoji(&buttons->stash_emoji, icons[ICON_TREASURE].emoji);

  for (int stash_idx = 0; stash_idx < MAX_STASHES; stash_idx++)
  {
    buttons->array[stash_idx] = (struct discord_component)
    {
      .type = DISCORD_COMPONENT_BUTTON,
      .label = set_label(&buttons->content[stash_idx], stash_names[stash_idx]),
      .custom_id = set_custom_id(&buttons->content[stash_idx], "%c%d.%ld",
          TYPE_FORAGE, stash_idx, event->member->user->id),
      .emoji = &buttons->stash_emoji.src
    };

    set_button_status(&buttons->array[stash_idx], check_piece_fit(&game->stashes[stash_idx], &game->current_piece) == 1);
  }

  buttons->array[3] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&buttons->content[3], "%c4%c.%ld",
        TYPE_FORAGE, TYPE_RESTART, event->member->user->id)
  };

  /*
    0: unpressed
    1: pressed, needs confirmation
  */
  if (game->is_restart == 1)
  {
    buttons->array[3].label = set_label(&buttons->content[3], "restart?");
    buttons->array[3].style = DISCORD_BUTTON_SUCCESS;
  }
  else {
    buttons->array[3].label = set_label(&buttons->content[3], "restart");
    buttons->array[3].style = DISCORD_BUTTON_DANGER;
  }
}

static inline void set_rotate_buttons(const struct discord_interaction *event, struct sd_rotate_buttons *rotate_buttons)
{
  rotate_buttons->array[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = set_label(&rotate_buttons->content[0], "Rotate CCW"),
    .custom_id = set_custom_id(&rotate_buttons->content[0], "%c0%c.%ld",
        TYPE_FORAGE, TYPE_ROTATE_CCW, event->member->user->id)
  };

  rotate_buttons->array[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = set_label(&rotate_buttons->content[1], "Rotate CW"),
    .custom_id = set_custom_id(&rotate_buttons->content[1], "%c1%c.%ld",
        TYPE_FORAGE, TYPE_ROTATE_CW, event->member->user->id)
  };
}

void forage_interaction(const struct discord_interaction *event)
{
  struct sd_message message = { 0 };

  PGresult *search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, event->member->user->id, 
      "color, squirrel, highest_level, tm_mday, stashes_complete, conjured_acorns, t_stamp");
  ERROR_INTERACTION((search_player == NULL), "Player data couldn't be retrieved!");

  struct sd_player player = (struct sd_player) 
  {
    .color = u_strtol( PQgetvalue(search_player, 0, 0) ),
    .squirrel = u_strtol( PQgetvalue(search_player, 0, 1) ),
    .highest_level = u_strtol( PQgetvalue(search_player, 0, 2) ),
    .tm_mday = u_strtol( PQgetvalue(search_player, 0, 3) ),
    .stashes_complete = u_strtol( PQgetvalue(search_player, 0, 4) ),
    .conjured_acorns = u_strtol( PQgetvalue(search_player, 0, 5) ),

    .timestamp = u_strtol( PQgetvalue(search_player, 0, 6) )
  };
  PQclear(search_player);

  ERROR_INTERACTION((event->message->timestamp < player.timestamp),
      "This appears to be an old session! Please renew your session by sending `/start`.");

  player.button_idx = event->data->custom_id[1] -48;

  struct sd_game game = { 0 };
  set_game(event, &game);

  check_stashes(&game);

  if (player.button_idx < 6) // an action was made
    detect_event(event, &message, &player, &game);

  set_info(&message, &player, &game);
  set_game_info(&message.fields, &game);
  set_stash_buttons(event, &message.buttons, &game);
  set_rotate_buttons(event, &message.rotate_buttons);

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player.color,
    .author = set_author(&message.author, event->member->user),
    .image = set_image(&message.image, biomes[game.biome].scene_path),

    .fields = &(struct discord_embed_fields)
    {
      .array = message.fields.array,
      .size = sizeof(message.fields.array)/sizeof(*message.fields.array)
    }
  };

  if (game.encounter > ERROR_STATUS)
  {
    struct sd_encounter *encounter = &biomes[game.biome].sections[game.section].encounters[game.encounter];
    struct sd_biome_section *section = &biomes[game.biome].sections[game.section];

    embed.title = set_title(&message.title, "%s: %s",
        section->name, encounter->name);
    embed.description = set_description(&message.description, encounter->description);
    embed.thumbnail = set_thumbnail(&message.thumbnail, encounter->path);
  }
  else {
    embed.title = set_title(&message.title, "Foraging");
    
    embed.thumbnail = set_thumbnail(&message.thumbnail, 
        (game.is_new_biome == 1) ? icons[ICON_LEVEL_UP].path : squirrels[player.squirrel].file.path);
  }

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
        .array = message.rotate_buttons.array,
        .size = sizeof(message.rotate_buttons.array)/sizeof(*message.rotate_buttons.array)
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

  update_game(event, &game);

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
}