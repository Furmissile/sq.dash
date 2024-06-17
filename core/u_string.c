#include "../include/u_string.h" /* string formatting */


char* u_snprintf(char* buffer, size_t size, char* format, ...)
{
  va_list args;

  va_start(args, format);

  vsnprintf(buffer + strlen(buffer), size - strlen(buffer), format, args);

  va_end(args);

  return buffer;
}

long u_strtol(const char* str)
{
  if (!str)
    return ERROR_STATUS;

  for (size_t i = 0; i < strlen(str); i++)
    if (str[i] -48 < 0
      || str[i] -48 > 9)
      return -1;

  return strtol(str, NULL, 10);
}

char* num_str(LABEL *dest, long input) 
{
  LABEL tmp_buffer = { 0 };

  snprintf(tmp_buffer.value, NAME_SIZE, "%ld", input);

  size_t tmp_buf_size = strlen(tmp_buffer.value);

  size_t is_negative = (input < 0) ? 1 : 0;

  // if num is less than 1000, return number
  if (tmp_buf_size < 4 + is_negative) {
    snprintf(dest->value, NAME_SIZE, "%s", tmp_buffer.value);
  }
  else {
    //define number offset by remainder of 3
    size_t first_set_n = (tmp_buf_size % 3 == 0) ? 3 : tmp_buf_size % 3;

    // if comma is second character and the number is negative, move up one set!
    if (is_negative && first_set_n == 1)
      first_set_n += 3;

    size_t buffer_offset = strlen(dest->value);

    //apply offset and add comma
    for (size_t tmp_offset = 0; tmp_offset < first_set_n; tmp_offset++)
      dest->value[buffer_offset++] = tmp_buffer.value[tmp_offset];
    dest->value[buffer_offset++] = ',';

    //while there are still characters, add 3 numbers and a comma
    for (size_t tmp_offset = 0; tmp_offset < tmp_buf_size -first_set_n; tmp_offset++) {
      dest->value[buffer_offset++] = tmp_buffer.value[tmp_offset + first_set_n];

      //do not apply comma if at end of string
      if (((tmp_offset +1) % 3 == 0) && (tmp_offset + first_set_n != tmp_buf_size -1))
        dest->value[buffer_offset++] = ',';
    }
  }

  return dest->value;
}

char* trim_buffer(LABEL *dest, char* input, char separator)
{
  LABEL tmp_buffer = { 0 };

  snprintf(tmp_buffer.value, NAME_SIZE, "%s", input);
  int tmp_buffer_size = strlen(tmp_buffer.value);

  int tmp_buffer_idx = 0;
  while (tmp_buffer.value[tmp_buffer_idx] != separator
    && tmp_buffer_idx < tmp_buffer_size)
    tmp_buffer_idx++;

  if (tmp_buffer_idx == tmp_buffer_size) {
    LOG_FORMAT_ERROR("TRIM_BUFFER:: NO SEPARATOR \n");
    return NULL;
  }

  tmp_buffer_idx++;

  int buffer_idx = 0;
  while (tmp_buffer.value[tmp_buffer_idx] != separator)
  {
    if (tmp_buffer_idx == tmp_buffer_size)
      break;
    dest->value[buffer_idx++] = tmp_buffer.value[tmp_buffer_idx++];
  }

  return dest->value;
}

char* lowercase(LABEL *dest, char* input)
{
  size_t input_size = strlen(input);

  // only set letters to lowercase!
  for (size_t i = 0; i < input_size; i++)
    dest->value[i] = (input[i] > 64 && input[i] < 91) ? input[i] + 32 : input[i];

  return dest->value;
}

void print_interaction_content(struct discord_interaction_response *resp)
{
  char values[16384] = { };
  discord_interaction_response_to_json(values, sizeof(values), resp);
  printf("%s \n", values);
}

struct tm* get_UTC(void)
{
  time_t rawtime;
  struct tm *info;
  time(&rawtime);
  info = gmtime(&rawtime);

  return info;
}