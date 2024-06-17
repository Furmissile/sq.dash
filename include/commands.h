#ifndef COMMAND_H
  #define COMMAND_H

  #include "concord/discord.h" /* discord API */

  #include <string.h>

  #define ERROR_INTERACTION(error, message, ...) \
      if (error) { \
        error_message(event, message, ##__VA_ARGS__); \
        return; \
      }

  /* These definitions should line up with button_types! */
  #define TYPE_FORAGE 'a'
  #define TYPE_ROTATE_CW 'b'
  #define TYPE_ROTATE_CCW 'c'
  #define TYPE_RESTART 'd'
  #define TYPE_INFO 'e'
  #define TYPE_SQUIRRELS 'f'
  #define TYPE_PAGE_SQUIRRELS 'g'
  #define TYPE_STEAL 'h'
  #define TYPE_HELP 'i'
  #define TYPE_BIOME_STORY 'j'
  #define TYPE_LEADERBOARD 'k'

  struct sd_command
  {
    char* name;        /* name of command */
    char command_id;  /* command prefix */

    /* command handle callback */
    void (*func_cb)(const struct discord_interaction *event);
  };

  enum APP_TYPE
  {
    APP_START,
    APP_PLAYER,
    APP_COLOR,

    APP_SIZE
  };

  enum BUTTON_TYPE
  {
    BUTTON_FORAGE,
    BUTTON_INFO,
    BUTTON_SQUIRREL,
    BUTTON_PAGE_SQUIRRELS,
    BUTTON_STEAL,
    BUTTON_HELP,
    BUTTON_BIOME_STORY,
    BUTTON_LEADERBOARD,

    BUTTON_SIZE
  };

  /*
  @brief 
    initializes global bot commands

  @params 
    client: discord bot client
    event: associated start-up data
    data: relevant data associated with message content

  @return
    (void)
  */
  void create_commands(struct discord *client, const struct discord_ready *event);

  /*
  @brief 
  processes interaction command

  @params 
    client: discord bot client
    event: discord interaction event data

  @return
    (void)
  */
  void process_command(struct discord *client, const struct discord_interaction *event);

  /*
  @brief 
    event callbacks

  @params 
    event: discord interaction event data

  @return
    (void)
  */
  void welcome(const struct discord_interaction *event);

  void info_interaction(const struct discord_interaction *event);

  void request_player_interaction(const struct discord_interaction *event);

  void color_interaction(const struct discord_interaction *event);

  void squirrels_interaction(const struct discord_interaction *event);

  void forage_interaction(const struct discord_interaction *event);

  void biome_story_interaction(const struct discord_interaction *event);

  void steal_interaction(const struct discord_interaction *event);

  void help_interaction(const struct discord_interaction *event);
  
  void leaderboard_interaction(const struct discord_interaction *event);

  void leaderboard_interaction(const struct discord_interaction *event);

  /*
  @brief 
    respond to interaction with error message if error occurs

  @params 
    event: discord interaction event data
    format: error message format

  @return
    (void)
  */
  void error_message(const struct discord_interaction *event, char* format, ...);

  /*
  @brief 
    error if user is not found

  @params 
    event: discord interaction event data
    resp: discord response info

  @return
    (void)
  */
  void not_user(struct discord *client, struct discord_response *resp);

  extern struct sd_command *app_commands;
  extern struct sd_command *button_types;

#endif