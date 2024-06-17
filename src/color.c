#include "../include/globals.h"  /* ERROR STATUS */
#include "../include/commands.h" /* public commands interface */
#include "../include/u_string.h"

#include "../include/embed.h"
#include "../include/buttons.h"

#include "../include/player.h"
#include "../include/database.h"

/*
  FILE DETAILS:
    * takes "color" field option from interaction and parses into color
*/

struct sd_message
{
  struct sd_author author;
  struct sd_title title;
};

/* Checks user input for hex value accuracy */
int is_color(char* input)
{
  if (strlen(input) != 6) {
    return ERROR_STATUS;
  } 

  for (int i = 0; i < 6; i++)
  {
    switch (input[i])
    {
      case '1': case '2': case '3': case '4': case '5': case '6': case '7': case '8': case '9': case '0':
      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f':
        continue;
      default:
        return ERROR_STATUS;
    }
  }

  return 0;
}

void color_interaction(const struct discord_interaction *event) 
{
  struct sd_message message = { 0 };

  // check input
  char* input = event->data->options->array[0].value;
  LABEL color_buf = { 0 };

  char* color = lowercase(&color_buf, input);

  ERROR_INTERACTION( ( is_color(color) == ERROR_STATUS ), "Invalid input. Please try again!");

  // load player
  create_player_data(event->member->user->id);
  PGresult* search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, event->member->user->id, "color");

  ERROR_INTERACTION((search_player == NULL), "Player data couldn't be retrieved!");

  struct sd_player player = (struct sd_player) 
  {
    .color = u_strtol( PQgetvalue(search_player, 0, 0) ),
  };
  PQclear(search_player);

  player.color = strtol(color, NULL, 16);

  struct discord_embed embed = {
    .color = player.color,
    .author = set_author(&message.author, event->member->user),

    .title = set_title(&message.title, "Color successfully changed to #%s!", color)
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

  PGresult *update_player = (PGresult*) { 0 };
  update_player = SQL_query(update_player, "update public.player set color = %d where user_id = %ld \n", 
      player.color, event->member->user->id);
  PQclear(update_player);
}