#ifndef GLOBALS_H
  #define GLOBALS_H

  #include "concord/discord.h" /* discord client */

  #include <libpq-fe.h> /* PGconn */

  #define GIT_PATH "https://github.com/Furmissile/sq.dash/blob/main/src_assets/%s?raw=true"

  #define SUCCESS_STATUS 0
  #define ERROR_STATUS -1

  #define BETA_BOT_ID 1048439491607674930
  #define MAIN_BOT_ID 905163158149287936
  
  #define APPLICATION_ID BETA_BOT_ID

  #define GUILD_ID 905167903224123473
  #define ARCHIVE_ID 1164893962297933924

  #define OWNER_ID 582648847881338961

  #define NAME_SIZE 128
  #define LINK_SIZE 256
  #define VALUE_SIZE 2048

  // differences between beta and main automatic!!
  #if (APPLICATION_ID == MAIN_BOT_ID)
    #define STATUS_CHANNEL 1046635264883294259

    #define START_ID "</start:1182163458133864571>"
    #define COLOR_ID "</color:1182163461715808276>"
    #define SCURRY_CREATE_ID "</scurry_create:1182163462688886926>"
    #define SCURRY_DISBAND_ID "</scurry_disband:1182163544687530096>"
    #define SCURRY_KICK_ID "</scurry_kick:1182163543408246935>"
    #define SCURRY_RENAME_ID "</scurry_rename:1182163545723523072>"
  #else
    #define STATUS_CHANNEL 1049184118954528888

    #define START_ID "</start:1178372476045103194>"
    #define COLOR_ID "</color:1089659248595714111>"
    #define SCURRY_CREATE_ID "</scurry_create:1089659500916650004>"
    #define SCURRY_DISBAND_ID "</scurry_disband:1179508464465215529>"
    #define SCURRY_KICK_ID "</scurry_kick:1179508461730537504>"
    #define SCURRY_RENAME_ID "</scurry_rename:1179414106051186688>"
  #endif

  extern PGconn *conn;
  extern struct discord *client;

#endif