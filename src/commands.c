#include "../include/u_string.h"
#include "../include/commands.h"

/*
  FILE DETAILS:
    * app command callbacks should be made based on the name of the interaction command
    * button callbacks should be made based on the first character of the button custom id
*/

struct sd_command *app_commands = (struct sd_command[])
{
  {
    .name = "start",
    .func_cb = &welcome
  },
  {
    .name = "Player",
    .func_cb = &request_player_interaction
  },
  {
    .name = "color",
    .func_cb = &color_interaction
  }
};

struct sd_command *button_types = (struct sd_command[])
{
  { // BUTTON_ADD_PIECE
    .func_cb = &forage_interaction,
    .command_id = TYPE_FORAGE
  },
  { // BUTTON_INFO
    .func_cb = &info_interaction,
    .command_id = TYPE_INFO
  },
  { // BUTTON_SQUIRREL
    .func_cb = &squirrels_interaction,
    .command_id = TYPE_SQUIRRELS
  },
  { // BUTTON_PAGE_SQUIRRELS
    .func_cb = &squirrels_interaction,
    .command_id = TYPE_PAGE_SQUIRRELS
  },
  { // BUTTON_STEAL
    .func_cb = &steal_interaction,
    .command_id = TYPE_STEAL
  },
  { // BUTTON_HELP
    .func_cb = &help_interaction,
    .command_id = TYPE_HELP
  },
  { // BUTTON_BIOME_STORY
    .func_cb = &biome_story_interaction,
    .command_id = TYPE_BIOME_STORY
  },
  { // BUTTON_LEADERBOARD
    .func_cb = &leaderboard_interaction,
    .command_id = TYPE_LEADERBOARD
  }
};

void create_commands(struct discord *client, const struct discord_ready *event)
{
  (void)event;

  struct discord_create_global_application_command commands[] =
  {
    {
      .name = "start",
      .description = "Begin your adventure here!",
      .type = DISCORD_APPLICATION_CHAT_INPUT
    },
    { // user preferences
      .name = "Player",
      .type = DISCORD_APPLICATION_USER,
      .description = " "
    },
    {
      .name = "color",
      .description = "Set the color of your embed!",
      .type = DISCORD_APPLICATION_CHAT_INPUT,
      
      .options = &(struct discord_application_command_options)
      {
        .array = (struct discord_application_command_option[])
        {
          {
            .type = DISCORD_APPLICATION_OPTION_STRING,
            .name = "color",
            .description = "Enter a hex value (e.g. 3e4af2)",
            .required = true
          }
        },
        .size = 1
      }
    }
  };

  for (int i = 0; i < (int)(sizeof(commands)/sizeof(*commands)); i++)
    discord_create_global_application_command(client, APPLICATION_ID, &commands[i], NULL);
}

void process_command(struct discord *client, const struct discord_interaction *event)
{
  (void)client;

  if (event->data->custom_id)
  {
    LABEL user_id_buf = { 0 };
    char* user_id_str = trim_buffer(&user_id_buf, event->data->custom_id, '.');

    unsigned long user_id = u_strtol(user_id_str);
    unsigned long event_user_id = (event->member) ? event->member->user->id : event->user->id;
  
    // then check user ids to deliver appropriate error message
    if (user_id != event_user_id) 
    {
      error_message(event, "This message belongs to someone else! Please send `/start` to begin your own adventure!");
      return;
    }

    int cmd_idx = 0;
    for (; cmd_idx < BUTTON_SIZE; cmd_idx++)
    {
      if (button_types[cmd_idx].command_id == event->data->custom_id[0])
        break;
    }

    if (cmd_idx < 0 || cmd_idx > BUTTON_SIZE -1)
    {
      error_message(event, "A command error has occurred!");
      return;
    }

    (*button_types[cmd_idx].func_cb)(event);
  }
  else {
    int app_idx = 0;
    for (app_idx = 0; app_idx < APP_SIZE; app_idx++)
      if (strcmp(event->data->name, app_commands[app_idx].name) == 0)
        break;
    
    if (app_idx == APP_SIZE)
      error_message(event, "It appears this command has been deprecated!");
    else
      (*app_commands[app_idx].func_cb)(event);
  }
}

void error_message(const struct discord_interaction *event, char* format, ...) 
{
  char buffer[1024] = { };

  va_list args;

  va_start(args, format);

  vsnprintf(buffer, sizeof(buffer), format, args);

  va_end(args);

  struct discord_interaction_response error_message = {
    .type = DISCORD_INTERACTION_CHANNEL_MESSAGE_WITH_SOURCE,

    .data = &(struct discord_interaction_callback_data) { 
      .flags = DISCORD_MESSAGE_EPHEMERAL,
      .content = buffer
    }
  };

  discord_create_interaction_response(client, event->id, event->token, &error_message, NULL);
}

void not_user(struct discord *client, struct discord_response *resp)
{
  (void)client;
  const struct discord_interaction *event = resp->keep;

  error_message(event, "This is not a valid user!");
}