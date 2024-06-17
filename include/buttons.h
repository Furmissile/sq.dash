#ifndef BUTTONS_H
  #define BUTTONS_H

  #include "globals.h" /* NAME_SIZE */
  #include "game.h"
  #include "emojis.h"
  #include "player.h"

  #include "concord/discord.h"

  #include <stdarg.h> /* variadic functions */
  #include <string.h> /* string formatting */

  struct sd_button
  {
    struct discord_component src;
    char custom_id[NAME_SIZE];
    char label[NAME_SIZE];
  };

  struct sd_buttons;

  struct sd_util_buttons
  {
    struct sd_button content[4];
    struct sd_emoji emojis[4];
    struct discord_component array[4];
  };

  struct sd_info_buttons
  {
    struct sd_button content[3];
    struct sd_emoji emojis[3];
    struct discord_component array[3];
  };

  struct sd_page_buttons
  {
    struct sd_button content[4];
    struct sd_emoji emojis[4];
    struct discord_component array[4];
  };

  /*
  @brief 
    sets button label

  @params 
    field: button content
    format: string formatter

  @return
    pointer to button content label
  */
  char* set_label(struct sd_button *button, const char* label, ...);

  /*
  @brief 
    sets button custom id

  @params 
    field: button content
    format: string formatter

  @return
    pointer to button content custom id
  */
  char* set_custom_id(struct sd_button *button, const char* custom_id, ...);
  
  /*
  @brief 
    set main command (util) buttons
    (appears in multiple commands without change)

  @params 
    util_buttons: pointer to array of component sources
    event: discord interaction content
    player: player content
    game: player's game content

  @return
    (void)
  */
  void set_util_buttons(struct sd_util_buttons *util_buttons, const struct discord_interaction *event, struct sd_player *player, struct sd_game *game);

  /*
  @brief 
    set help buttons
    (appears in multiple commands without change)

  @params 
    info_buttons: pointer to array of component sources
    event: discord interaction content
    game: player's game content

  @return
    (void)
  */
  void set_info_buttons(struct sd_info_buttons *info_buttons, const struct discord_interaction *event, struct sd_game *game);

  /*
  @brief 
    set page buttons
    (appears in multiple commands without change)

  @params 
    page_buttons: pointer to array of component sources
    event: discord interaction content
    command_id: command id for custom id
    topic_size: pages length

  @return
    (void)
  */
  void set_page_buttons(struct sd_page_buttons *page_buttons, const struct discord_interaction *event, char command_id, int topic_size);

  /*
  @brief 
    activate or deactivate a button

  @params 
    button: action button
    threshold: condition when to disable

  @return
    (void)
  */
  void set_button_status(struct discord_component *button, int threshold);

  /*
  @brief 
    activate a button based on what player has

  @params 
    button: action button
    threshold: condition when to disable

  @return
    (void)
  */
  void set_button_active(struct discord_component *button, int threshold);
  
#endif