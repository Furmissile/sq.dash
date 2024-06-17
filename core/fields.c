#include "../include/globals.h"  /* NAME_SIZE and VALUE_SIZE */
#include "../include/fields.h"


char* set_field_name(struct sd_field *field, const char* format, ...)
{
  size_t physical_size = strlen(field->name);
  va_list args;

  va_start(args, format);

  vsnprintf(field->name + physical_size, NAME_SIZE - physical_size, format, args);

  va_end(args);

  return field->name;
}

char* set_field_value(struct sd_field *field, const char* format, ...)
{
  size_t physical_size = strlen(field->value);
  va_list args;

  va_start(args, format);

  vsnprintf(field->value + physical_size, VALUE_SIZE - physical_size, format, args);

  va_end(args);

  return field->value;
}