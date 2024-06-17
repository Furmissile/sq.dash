#include "../include/globals.h"
#include "../include/commands.h"
#include "../include/u_string.h"

#include "../include/embed.h"
#include "../include/buttons.h"
#include "../include/emojis.h"

#include "../include/player.h"
#include "../include/database.h"

#include "../include/items.h"
#include "../include/squirrels.h"
#include "../include/icons.h"

/*
  FILE DETAILS:
    * Initializing new game with welcome screen
*/

struct sd_message
{
  struct sd_author author;
  struct sd_title title;
  struct sd_thumbnail thumbnail;
  struct sd_image image;
};

#define GIT_WELCOME "git_welcome.gif"

void welcome(const struct discord_interaction *event)
{
  struct sd_message message = { 0 };

  create_player_data(event->member->user->id);
  PGresult* search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, event->member->user->id, "color, squirrel");

  ERROR_INTERACTION((search_player == NULL), "Player data couldn't be retrieved!");

  struct sd_player player = (struct sd_player)
  {
    .color = u_strtol( PQgetvalue(search_player, 0, 0) ),
    .squirrel = u_strtol( PQgetvalue(search_player, 0, 1) )
  };
  PQclear(search_player);

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player.color,
    .author = set_author(&message.author, event->member->user),
    .title = set_title(&message.title, "Welcome, %s!", 
        event->member->user->username),
    .thumbnail = set_thumbnail(&message.thumbnail, squirrels[player.squirrel].file.path),
    .image = set_image(&message.image, GIT_WELCOME)
  };

  struct sd_button start = { 0 };
  struct sd_emoji start_emoji = { 0 };

  struct discord_component action_rows = {
    .type = DISCORD_COMPONENT_ACTION_ROW,
    .components = &(struct discord_components) {
      .array = &(struct discord_component)
      {
        .type = DISCORD_COMPONENT_BUTTON,
        .style = DISCORD_BUTTON_SUCCESS,
        .label = set_label(&start, "Start"),
        .custom_id = set_custom_id(&start, "%c.%ld", 
            TYPE_INFO, event->member->user->id),
        .emoji = set_button_emoji(&start_emoji, items[TYPE_ACORNS].file.emoji)
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
      },
      .components = &(struct discord_components) {
        .array = &action_rows,
        .size = 1
      }
    }

  };

  discord_create_interaction_response(client, event->id, event->token, &interaction, NULL);
}