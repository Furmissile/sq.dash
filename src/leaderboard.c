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
    * fetch top 10 players from database in descending order based on high acorn count
    * request discord user data of each player
    * set data in an array in case users return out of order

  LEADERBOARD SET-UP:
    1. `<username>` <value>
    2. `<username>` <value>
    3. `<username>` <value>
    4. @<your id> <value
    5. `<username>` <value>
    ...

    OR 

    1. `<username>` <value>
    2. `<username>` <value>
    3. `<username>` <value>
    ...

    26. @<your id> <value
    ...
*/

#define LEADERBOARD_SIZE 10

struct sd_row
{
  int row_idx;
  char username[NAME_SIZE];
  unsigned long user_id;
  int value;
};

struct sd_leaderboard
{
  PGresult *lb_query;
  struct sd_player player;

  struct sd_row rows[LEADERBOARD_SIZE];

  struct sd_row player_row; // filled if player isnt top 10
  int is_top_ten;

  int row_counter; // ensures users are placed in order
  int total_rows;
};

struct sd_message
{
  struct sd_author author;
  struct sd_title title;
  struct sd_thumbnail thumbnail;
  struct sd_description description;
  struct sd_footer footer;

  struct sd_leaderboard leaderboard;
};

void create_leaderboard_interaction(const struct discord_interaction *event, struct sd_message *message)
{
  struct sd_player *player = &message->leaderboard.player;

  struct discord_embed embed = (struct discord_embed)
  {
    .color = player->color,
    .author = set_author(&message->author, event->member->user),
    .title = set_title(&message->title, "Acorn Leaderboard"),
    .thumbnail = set_thumbnail(&message->thumbnail, squirrels[player->squirrel].file.path),
    .description = message->description.value,
    .footer = set_footer(&message->footer, icons[ICON_ALERT].path, "Welcome to the Leaderboard!")
  };

  PGresult* search_game = (PGresult*) { 0 };
  search_game = get_game_data(search_game, event->member->user->id, "level");

  if (search_game == NULL)
  {
    error_message(event, "Game data couldn't be retrieved!");
    PQclear(search_game);
    free(message);
    return;
  }

  struct sd_game game = (struct sd_game) 
  {
    .level = u_strtol( PQgetvalue(search_game, 0, 0) )
  };
  PQclear(search_game);

  game.biome = (game.level -1) % BIOME_SIZE;

  struct sd_util_buttons util_buttons = { 0 };
  set_util_buttons(&util_buttons, event, player, &game);

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

  struct discord_edit_original_interaction_response interaction = 
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
  };

  discord_edit_original_interaction_response(client, APPLICATION_ID, event->token, &interaction, NULL);

  free(message);
}

void is_user(struct discord *client, struct discord_response *resp, const struct discord_user *user)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;
  struct sd_message *message = resp->data;

  struct sd_leaderboard *lb_data = &message->leaderboard;

  u_snprintf(lb_data->rows[lb_data->row_counter++].username, NAME_SIZE, user->username);

  if (lb_data->row_counter < lb_data->total_rows)
    return;

  for (int row_idx = 0; row_idx < lb_data->total_rows; row_idx++)
  {
    struct sd_row *row = &lb_data->rows[row_idx];

    LABEL row_value_buf = { 0 };
    char* value = num_str(&row_value_buf, row->value);

    if (event->member->user->id == row->user_id)
    {
      u_snprintf(message->description.value, VALUE_SIZE,
          "**%d**. <@%ld> **%s** \n",
          row->row_idx, row->user_id, value);
      
      lb_data->is_top_ten = 1;
    }
    else {
      u_snprintf(message->description.value, VALUE_SIZE,
          "**%d**. `%s` **%s** \n",
          row->row_idx, row->username, value);
    }
  }

  PQclear(lb_data->lb_query);

  if (lb_data->is_top_ten == 0)
  {
    lb_data->lb_query = SQL_query(lb_data->lb_query,
        "select rank_idx, user_id, highest_level "
        "from (select user_id, highest_level, dense_rank() over (order by highest_level desc) as rank_idx "
        "from public.player) as lb where user_id = %ld",
        event->member->user->id);

    if (PQntuples(lb_data->lb_query) == 0)
    {
      error_message(event, "An internal database error has occurred (L:183)");
      PQclear(lb_data->lb_query);
      free(message);
      return;
    }

    struct sd_row *player_row = &lb_data->player_row;

    *player_row = (struct sd_row)
    {
      .row_idx = u_strtol( PQgetvalue(lb_data->lb_query, 0, 0) ),
      .user_id = u_strtol( PQgetvalue(lb_data->lb_query, 0, 1) ),
      .value = u_strtol( PQgetvalue(lb_data->lb_query, 0, 2) ),
    };

    LABEL row_value_buf = { 0 };
    char* value = num_str(&row_value_buf, player_row->value);

    u_snprintf(message->description.value, VALUE_SIZE,
        "\n**%d**. <@%ld> **%s** \n",
        player_row->row_idx, player_row->user_id, value);
  }

  create_leaderboard_interaction(event, message);
}

void build_leaderboard(struct discord *client, struct discord_response *resp, const struct discord_interaction_response *ret)
{
  (void)ret;
  const struct discord_interaction *event = resp->keep;
  struct sd_message *message = resp->data;

  struct sd_leaderboard *lb_data = &message->leaderboard;

  lb_data->total_rows = PQntuples(lb_data->lb_query);

  for (int row_idx = 0; row_idx < lb_data->total_rows; row_idx++)
  {
    lb_data->rows[row_idx].row_idx = row_idx +1;
    lb_data->rows[row_idx].user_id = u_strtol( PQgetvalue(lb_data->lb_query, row_idx, 0) );
    lb_data->rows[row_idx].value = u_strtol( PQgetvalue(lb_data->lb_query, row_idx, 1) );

    struct discord_ret_user ret_user = {
      .done = &is_user,
      .data = message,
      .keep = event
    };

    discord_get_user(client, lb_data->rows[row_idx].user_id, &ret_user);
  }
}

void leaderboard_interaction(const struct discord_interaction *event)
{
  struct sd_message *message = calloc(1, sizeof(struct sd_message));

  PGresult *search_player = (PGresult*) { 0 };
  search_player = get_player_data(search_player, event->member->user->id, 
      "color, squirrel, highest_level, t_stamp");

  if (search_player == NULL)
  {
    error_message(event, "Player data couldn't be retrieved!");
    PQclear(search_player);
    free(message);
    return;
  }

  message->leaderboard.player = (struct sd_player) 
  {
    .color = u_strtol( PQgetvalue(search_player, 0, 0) ),
    .squirrel = u_strtol( PQgetvalue(search_player, 0, 1) ),
    .highest_level = u_strtol( PQgetvalue(search_player, 0, 2) ),

    .timestamp = u_strtol( PQgetvalue(search_player, 0, 3) )
  };
  PQclear(search_player);

  struct sd_player *player = &message->leaderboard.player;

  if (event->message->timestamp < player->timestamp)
  {
    error_message(event,  "This appears to be an old session! Please renew your session by sending `/start`.");
    free(message);
    return;
  }

  struct discord_ret_interaction_response ret_response = {
    .done = &build_leaderboard,
    .data = message,
    .keep = event
  };

  message->leaderboard.lb_query = (PGresult*) { 0 };

  message->leaderboard.lb_query = SQL_query(message->leaderboard.lb_query, 
      "select user_id, highest_level from public.player where highest_level > 0 order by highest_level desc LIMIT 10");

  if (PQntuples(message->leaderboard.lb_query) == 0)
  {
    PQclear(message->leaderboard.lb_query);
    free(message);
    error_message(event, "There are no entries yet!");
    return;
  }

  discord_create_interaction_response(client, event->id, event->token, 
    &(struct discord_interaction_response)
    {
      .type = DISCORD_INTERACTION_DEFERRED_UPDATE_MESSAGE,

      .data = &(struct discord_interaction_callback_data) 
      {
        .content = "Loading leaderboard..."
      }
    },
    &ret_response);
}