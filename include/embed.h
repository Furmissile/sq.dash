#ifndef EMBED_H
  #define EMBED_H

  #include "globals.h" /* LINK_SIZE and NAME_SIZE */

  #include "concord/discord.h" /* discord API */

  #include <stdarg.h> /* variadic functions */
  #include <string.h> /* string formatting */

  struct sd_message; /* message declaration (to be defined in command files...) */

  struct sd_author
  {
    struct discord_embed_author src;
    char username[NAME_SIZE];
    char avatar_url[LINK_SIZE];
  };

  struct sd_title
  {
    char value[NAME_SIZE];
  };

  struct sd_description
  {
    char value[VALUE_SIZE];
  };

  struct sd_thumbnail
  {
    struct discord_embed_thumbnail src;
    char icon_url[LINK_SIZE];
  };

  struct sd_image
  {
    struct discord_embed_image src;
    char icon_url[LINK_SIZE];
  };

  struct sd_footer
  {
    struct discord_embed_footer src;
    char description[NAME_SIZE];
    char icon_url[LINK_SIZE];
  };

  /*
  @brief 
    sets message author

  @params 
    author: author content
    event: discord interaction event data
    user: user (author) data

  @return
    pointer to author content
  */
  struct discord_embed_author* set_author(struct sd_author *author, const struct discord_user *user);

  /*
  @brief 
    sets message title

  @params 
    title: title content
    format: discord interaction event data

  @return
    pointer to author content
  */
  char* set_title(struct sd_title *title, char* format, ...);

  /*
  @brief 
    sets message description

  @params 
    description: description content
    format: discord interaction event data

  @return
    pointer to description content
  */
  char* set_description(struct sd_description *description, char* format, ...);
  
  /*
  @brief 
    sets message image

  @params 
    image: image content
    path: file path of image

  @return
    pointer to image content
  */
  struct discord_embed_image* set_image(struct sd_image *image, const char* path);

  /*
  @brief 
    sets message thumbnail

  @params 
    thumbnail: thumbnail content
    path: file path of thumbnail

  @return
    pointer to thumbnail content
  */
  struct discord_embed_thumbnail* set_thumbnail(struct sd_thumbnail *thumbnail, const char* path);

  /*
  @brief 
    sets message footer

  @params 
    thumbnail: footer content
    path: file path of footer icon
    format: footer text format

  @return
    pointer to thumbnail content
  */
  struct discord_embed_footer* set_footer(struct sd_footer *footer, const char* path, const char* format, ...);

#endif