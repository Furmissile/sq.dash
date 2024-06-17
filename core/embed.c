#include "../include/embed.h"
#include "../include/u_string.h"


struct discord_embed_author* set_author(struct sd_author *author, const struct discord_user *user)
{
  author->src = (struct discord_embed_author)
  {
    .name = u_snprintf(author->username, NAME_SIZE, user->username),
    .icon_url = u_snprintf(author->avatar_url, LINK_SIZE, 
        "https://cdn.discordapp.com/avatars/%lu/%s.png",
        user->id, user->avatar)
  };

  return &author->src;
}

char* set_title(struct sd_title *title, char* format, ...)
{
  va_list args;

  va_start(args, format);

  vsnprintf(title->value, NAME_SIZE, format, args);

  va_end(args);

  return title->value;
}

char* set_description(struct sd_description *description, char* format, ...)
{
  va_list args;

  va_start(args, format);

  vsnprintf(description->value, VALUE_SIZE, format, args);

  va_end(args);

  return description->value;
}

struct discord_embed_image* set_image(struct sd_image *image, const char* path)
{
  image->src = (struct discord_embed_image) {
    .url = u_snprintf(image->icon_url, LINK_SIZE, GIT_PATH, path)
  };

  return &image->src;
}

struct discord_embed_thumbnail* set_thumbnail(struct sd_thumbnail *thumbnail, const char* path)
{
  thumbnail->src = (struct discord_embed_thumbnail) {
    .url = u_snprintf(thumbnail->icon_url, LINK_SIZE, GIT_PATH, path)
  };

  return &thumbnail->src;
}

struct discord_embed_footer* set_footer(struct sd_footer *footer, const char* path, const char* format, ...)
{
  va_list args;

  va_start(args, format);

  vsnprintf(footer->description, NAME_SIZE, format, args);

  va_end(args);

  footer->src = (struct discord_embed_footer)
  {
    .text = footer->description,
    .icon_url = u_snprintf(footer->icon_url, LINK_SIZE, GIT_PATH, path)
  };

  return &footer->src;
}