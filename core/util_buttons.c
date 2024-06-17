#include "../include/commands.h" /* command interface */
#include "../include/buttons.h"  /* buttons content */
#include "../include/player.h"   /* player content */

#include "../include/items.h"     /* items content */
#include "../include/icons.h"     /* icons content */
#include "../include/squirrels.h" /* squirrels content */
#include "../include/biomes.h"    /* biomes content*/


void set_util_buttons(struct sd_util_buttons *util_buttons, const struct discord_interaction *event, struct sd_player *player, struct sd_game *game)
{
  util_buttons->array[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SUCCESS,
    .label = set_label(&util_buttons->content[0], "Forage!"),
    .custom_id = set_custom_id(&util_buttons->content[0], "%c6.%ld",
        TYPE_FORAGE, event->member->user->id), 
    .emoji = set_button_emoji(&util_buttons->emojis[0], items[TYPE_ACORNS].file.emoji)
  };

  struct tm *info = get_UTC();

  int sq_page_idx = (info->tm_mday > 28) ? 4 : info->tm_mday/7 +1;

  util_buttons->array[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = set_label(&util_buttons->content[1], "Squirrels"),
    .custom_id = set_custom_id(&util_buttons->content[1], "%c6%d.%ld", // 5 is squirrels per page +1!
        TYPE_SQUIRRELS, sq_page_idx, event->member->user->id),
    .emoji = set_button_emoji(&util_buttons->emojis[1], squirrels[player->squirrel].file.emoji)
  };

  printf("%s \n\n", biomes[game->biome].file.emoji);

  util_buttons->array[2] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = set_label(&util_buttons->content[2], "Info"),
    .custom_id = set_custom_id(&util_buttons->content[2], "%c6.%ld",
        TYPE_INFO, event->member->user->id),
    .emoji = set_button_emoji(&util_buttons->emojis[2], biomes[game->biome].file.emoji)
  };

  util_buttons->array[3] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = set_label(&util_buttons->content[3], "Stealing"),
    .custom_id = set_custom_id(&util_buttons->content[3], "%c6.%ld",
        TYPE_STEAL, event->member->user->id),
    .emoji = set_button_emoji(&util_buttons->emojis[3], squirrels[SQUIRREL_BOOKIE].file.emoji)
  };
}

void set_info_buttons(struct sd_info_buttons *info_buttons, const struct discord_interaction *event, struct sd_game *game)
{
  info_buttons->array[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = set_label(&info_buttons->content[0], "Help"),
    .custom_id = set_custom_id(&info_buttons->content[0], "%c60.%ld",
        TYPE_HELP, event->member->user->id),
    .emoji = set_button_emoji(&info_buttons->emojis[0], icons[ICON_HELP].emoji)
  };

  info_buttons->array[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = set_label(&info_buttons->content[1], "Biome Story"),
    .custom_id = set_custom_id(&info_buttons->content[1], "%c6%d.%ld",
        TYPE_BIOME_STORY, game->biome, event->member->user->id), 
    .emoji = set_button_emoji(&info_buttons->emojis[1], icons[ICON_ALERT].emoji)
  };

  info_buttons->array[2] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .style = DISCORD_BUTTON_SECONDARY,
    .label = set_label(&info_buttons->content[2], "Leaderboard"),
    .custom_id = set_custom_id(&info_buttons->content[2], "%c6.%ld",
        TYPE_LEADERBOARD, event->member->user->id),
    .emoji = set_button_emoji(&info_buttons->emojis[2], icons[ICON_CROWN].emoji)
  };
}

void set_page_buttons(struct sd_page_buttons *page_buttons, const struct discord_interaction *event, char command_id, int topic_size)
{
  int page_idx = event->data->custom_id[2] -48;

  page_buttons->array[0] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[0], "%c00.%ld",
        command_id, event->member->user->id),
    .label = set_label(&page_buttons->content[0], "⏮️")
  };

  set_button_status(&page_buttons->array[0], page_idx == 0);

  page_buttons->array[1] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[1], "%c1%d.%ld", // button_idx is fixed!
        command_id, (page_idx > 0) ? page_idx -1 : 0, event->member->user->id),
    .label = set_label(&page_buttons->content[1], "⏪")
  };

  set_button_status(&page_buttons->array[1], page_idx == 0);

  page_buttons->array[2] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[2], "%c3%d.%ld",
        command_id, (page_idx < topic_size) ? page_idx +1 : topic_size -1, event->member->user->id),
    .label = set_label(&page_buttons->content[2], "⏩")
  };

  set_button_status(&page_buttons->array[2], page_idx == topic_size -1);

  page_buttons->array[3] = (struct discord_component)
  {
    .type = DISCORD_COMPONENT_BUTTON,
    .custom_id = set_custom_id(&page_buttons->content[3], "%c4%d.%ld",
        command_id, topic_size -1, event->member->user->id),
    .label = set_label(&page_buttons->content[3], "⏭️")
  };

  set_button_status(&page_buttons->array[3], page_idx == topic_size -1);
}