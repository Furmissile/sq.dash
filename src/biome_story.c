#include "../include/commands.h" /* public command interface */
#include "../include/u_string.h"

#include "../include/embed.h"
#include "../include/buttons.h"
#include "../include/emojis.h"

#include "../include/player.h"
#include "../include/game.h"
#include "../include/database.h"

#include "../include/icons.h"
#include "../include/squirrels.h"
#include "../include/biomes.h"

/*
  FILE DETAILS:
    * displayes biome story of current file
    * Content is discovered if the designated bit index is active
    * Any content not found should be hidden behind a spoiler detailing what encounter is needed to unlock it
    * Otherwise content should be shown italicised wrapped in quotes
*/

enum STORY_TOPIC 
{
  STORY_GRASSLANDS,
  STORY_SEEPING_SANDS,
  STORY_NATURE_END,
  STORY_DEATH_GRIP,
  STORY_LAST_ACORN,

  STORY_TOPIC_SIZE
};

struct sd_fields
{
  struct sd_field content[5];
  struct discord_embed_field array[5];

  int fields_size; // not all fields will always be used!
};

struct sd_message
{
  struct sd_author author;
  struct sd_title title;
  struct sd_thumbnail thumbnail;
  struct sd_image image;

  struct sd_fields fields;
};

static inline void set_info(const struct discord_interaction *event, struct sd_fields *fields, int page_idx)
{
  // set fields size
  fields->fields_size = biomes[page_idx].sections_size;

  // pull out value of biome to show
  char* biome_names[BIOME_SIZE] = { "grasslands", "seeping_sands", "nature_end", "death_grip", "last_acorn" };

  PGresult *search_story = (PGresult*) { 0 };
  search_story = SQL_query(search_story, "select %s from public.biome_story where user_id = %ld",
      biome_names[page_idx], event->member->user->id);
  
  int biome_value = u_strtol( PQgetvalue(search_story, 0, 0) );

  char* bullet = ICON_EMOJI(ICON_BULLET);

  // display context based on biome value for each section with each section's name
  int section_size = biomes[page_idx].sections_size;

  int encounter_bit_idx = 0;

  for (int section_idx = 0; section_idx < section_size; section_idx++)
  {
    struct sd_biome_section *section = &biomes[page_idx].sections[section_idx];

    fields->array[section_idx] = (struct discord_embed_field)
    {
      .name = set_field_name(&fields->content[section_idx], section->name)
    };

    int encounter_size = section->encounters_size;

    // list each encounter based on section's encounter size
    for (int encounter_idx = 0; encounter_idx < encounter_size; encounter_idx++)
    {
      struct sd_encounter *encounter = &section->encounters[encounter_idx];

      // if the player has seen this encounter, show its context
      if ((biome_value >> encounter_bit_idx & 0x01) == 1)
      {
        fields->array[section_idx].value = set_field_value(&fields->content[section_idx], 
            "%s \"*%s*\" \n",
            bullet, encounter->context);
      }
      else {
        // or tell the player what encounter needs to be completed to show this element
        fields->array[section_idx].value = set_field_value(&fields->content[section_idx], 
            "%s || Complete \"%s\" to view element. || \n",
            bullet, encounter->name);
      }

      encounter_bit_idx++;
    }

  }
}

void biome_story_interaction(const struct discord_interaction *event)
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

  int page_idx = event->data->custom_id[2] -48;

  set_info(event, &message.fields, page_idx);

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player.color,
    .author = set_author(&message.author, event->member->user),
    .title = set_title(&message.title, "%s Story", biomes[page_idx].file.name),

    .thumbnail = set_thumbnail(&message.thumbnail, squirrels[player.squirrel].file.path),
    .image = set_image(&message.image, biomes[page_idx].scene_path),

    .fields = &(struct discord_embed_fields)
    {
      .array = message.fields.array,
      .size = message.fields.fields_size
    }
  };

  struct sd_page_buttons page_buttons = { 0 };
  set_page_buttons(&page_buttons, event, TYPE_BIOME_STORY, STORY_TOPIC_SIZE);

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