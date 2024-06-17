#ifndef ICONS_H
  #define ICONS_H

  enum MISC 
  {
    ICON_ADD,
    ICON_CROWN,
    ICON_ALERT,
    ICON_HELP,
    ICON_TREASURE,
    ICON_INDENT,
    ICON_WEATHER,
    ICON_STAHR,
    ICON_OFFSET_ARROW,
    ICON_BULLET,

    ICON_INACTIVE,
    ICON_PROGRESS,

    ICON_CORRUPT_ACORN,
    ICON_LEVEL_UP,

    ICONS_SIZE
  };

  struct sd_icon
  {
    char* name;
    char* emoji;
    char* path;
  };

  extern struct sd_icon *icons;

#endif