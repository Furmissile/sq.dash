#include "../include/buttons.h"


char* set_label(struct sd_button *button, const char* label, ...)
{
  va_list args;
  size_t physical_size = strlen(button->label);

  va_start(args, label);

  vsnprintf(button->label + physical_size, NAME_SIZE - physical_size, label, args);

  va_end(args);

  return button->label;
}

char* set_custom_id(struct sd_button *button, const char* custom_id, ...)
{
  va_list args;
  size_t physical_size = strlen(button->custom_id);

  va_start(args, custom_id);

  vsnprintf(button->custom_id + physical_size, NAME_SIZE - physical_size, custom_id, args);

  va_end(args);

  return button->custom_id;
}

void set_button_status(struct discord_component *button, int threshold)
{
  if (threshold)
  {
    button->style = DISCORD_BUTTON_SECONDARY;
    button->disabled = true;
  }
  else {
    button->style = DISCORD_BUTTON_PRIMARY;
  }
}

void set_button_active(struct discord_component *button, int threshold)
{
  if (threshold)
  {
    button->style = DISCORD_BUTTON_SUCCESS;
    button->disabled = true;
  }
  else {
    button->style = DISCORD_BUTTON_PRIMARY;
  }
}