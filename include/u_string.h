#ifndef U_STRING_H
  #define U_STRING_H

  #include "globals.h"

  #include "concord/discord.h" /* discord client */
  
  #include <string.h> /* string formatting */
  #include <stdlib.h> /* exit() */
  #include <stdarg.h> /* variadic functions */

  #define LOG_FORMAT_ERROR(format, ...) \
      FILE *fp = fopen("crash.log", "a"); \
      fprintf(fp, format "\n", ##__VA_ARGS__); \
      fclose(fp); \
      printf(format "\n", ##__VA_ARGS__);

  typedef struct { char value[NAME_SIZE]; } LABEL;

  typedef struct { char value[VALUE_SIZE]; } DESCRIPTION;

  /*
  @brief 
    format a string buffer

  @params 
    buffer: buffer to write to
    size: size of buffer
    format: string formatter

  @return
    pointer to buffer content
  */
  char* u_snprintf(char* buffer, size_t size, char* format, ...);

  /*
  @brief 
    converts a string to an integer (if only a number is present)

  @params 
    str: string

  @return
    integer or ERROR_STATUS if failed to read string
  */
  long u_strtol(const char* str);

  /*
  @brief 
    converts a number to a thousand-separated format 

  @params 
    dest: LABEL to format str to
    input: number to format

  @return
    dest content
  */
  char* num_str(LABEL *dest, long input);

  /*
  @brief 
    trims content out of a buffer based on a separator

  @params 
    dest: LABEL to format str to
    input: string input
    separator: start/stop indicator

  @return
    resulting string
  */
  char* trim_buffer(LABEL *dest, char* input, char separator);

  /*
  @brief 
    sets all letters (A-Z) to lowercase

  @params 
    dest: LABEL to format str to
    input: string input

  @return
    resulting string
  */
  char* lowercase(LABEL *dest, char* input);

  /*
  @brief 
    prints content of interaction response as raw json

  @params 
    resp: interaction response data

  @return
    (void)
  */
  void print_interaction_content(struct discord_interaction_response *resp);

  /*
  @brief 
    tells time

  @params 
    (void)

  @return
    pointer to tm struct content
  */
  struct tm* get_UTC(void);
  
#endif