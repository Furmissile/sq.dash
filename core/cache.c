#include "../include/cache.h"
#include "../include/emojis.h"
#include "../include/items.h"
#include "../include/icons.h"  /* indent emoji */


char* format_cache(struct sd_field *dest, CACHE *content)
{
  snprintf(dest->value, VALUE_SIZE, "%s%s%s \n%s%s%s \n%s%s%s",
      CACHE_ITEM_EMOJI(content->cache[SLOT_TOP_LEFT]),
      CACHE_ITEM_EMOJI(content->cache[SLOT_TOP_CENTER]),
      CACHE_ITEM_EMOJI(content->cache[SLOT_TOP_RIGHT]),
      CACHE_ITEM_EMOJI(content->cache[SLOT_MID_LEFT]),
      ICON_EMOJI(ICON_INDENT),
      CACHE_ITEM_EMOJI(content->cache[SLOT_MID_RIGHT]),
      CACHE_ITEM_EMOJI(content->cache[SLOT_BOTTOM_LEFT]),
      CACHE_ITEM_EMOJI(content->cache[SLOT_BOTTOM_CENTER]),
      CACHE_ITEM_EMOJI(content->cache[SLOT_BOTTOM_RIGHT])
  );

  return dest->value;
}

void print_cache(CACHE *content)
{
  for (int stash_idx = 0; stash_idx < SLOTS_SIZE; stash_idx++)
  {
    printf("[%d]%s \n", 
        content->cache[stash_idx], CACHE_ITEM_EMOJI(content->cache[stash_idx]));
  }
  printf("\n\n");
}

void format_db_stash(CACHE *content)
{
  snprintf(content->db_format, NAME_SIZE, "{%d,%d,%d,%d,%d,%d,%d,%d}",
      content->cache[SLOT_TOP_LEFT], // NOTE: these formats are NOT in order!
      content->cache[SLOT_TOP_CENTER],
      content->cache[SLOT_TOP_RIGHT],
      content->cache[SLOT_MID_RIGHT],

      content->cache[SLOT_BOTTOM_RIGHT],
      content->cache[SLOT_BOTTOM_CENTER],
      content->cache[SLOT_BOTTOM_LEFT],
      content->cache[SLOT_MID_LEFT]
  );
}


void format_db_shuffle(PIECES *shuffle)
{
  snprintf(shuffle->db_format, NAME_SIZE, "{%d,%d,%d}",
    shuffle->prev, shuffle->current, shuffle->next);
}

void merge_caches(CACHE *dest, CACHE *content)
{
  // merge pie indexes ONLY if piece is not empty to not overwrite cache!
  for (int slot_idx = 0; slot_idx < SLOTS_SIZE; slot_idx++)
  {
    if (content->cache[slot_idx] != TYPE_EMPTY)
      dest->cache[slot_idx] = content->cache[slot_idx];
  }
}

int check_piece_fit(CACHE *dest, CACHE *piece)
{
  // if a slot of piece or dest is filled, this piece will not fit
  for (int slot_idx = 0; slot_idx < SLOTS_SIZE; slot_idx++)
  {
    int acorn_type = dest->cache[slot_idx];
    int piece_type = piece->cache[slot_idx];

    if (piece_type != TYPE_EMPTY
      && acorn_type != TYPE_EMPTY
      && acorn_type < TYPE_CURSE
      && acorn_type != TYPE_POP)
    {
      return 1;
    }
  }

  return 0;
}

int check_complete_stash(CACHE *stash)
{
  // if any slot is empty (disaster and silhouette are empty), the stash is not complete
  for (int slot_idx = 0; slot_idx < SLOTS_SIZE; slot_idx++)
  {
    int acorn_type = stash->cache[slot_idx];

    // TYPE_POP is not checked here because it's impossible to get in a completed stash!
    if (acorn_type == TYPE_EMPTY
      || acorn_type >= TYPE_CURSE
      || acorn_type == TYPE_POP)
      return 0;
  }

  return 1;
}

void reward_stash(CACHE *stash, int start_idx, int end_idx)
{
  for (int slot_idx = start_idx; slot_idx < end_idx; slot_idx++)
  {
    int acorn_idx = (slot_idx < 0) ? SLOTS_SIZE - abs(slot_idx) : slot_idx % SLOTS_SIZE;
    int acorn_type = stash->cache[acorn_idx];

    if (acorn_type == TYPE_CONJURED_ACORNS)
      stash->conjured_acorns++;

    stash->value += items[acorn_type].value;

    // automatically set pop animation if slot is filled
    if (acorn_type < TYPE_CURSE
      && acorn_type != TYPE_EMPTY)
      stash->cache[acorn_idx] = TYPE_POP;
  }
}

int search_type(CACHE *stash, int type)
{
  for (int slot_idx = 0; slot_idx < SLOTS_SIZE; slot_idx++)
  {
    int acorn_type = stash->cache[slot_idx];

    if (acorn_type == type)
      return slot_idx;
  }

  return ERROR_STATUS;
}

void fill_type(CACHE *stash, int type)
{
  for (int slot_idx = 0; slot_idx < SLOTS_SIZE; slot_idx++)
  {
    int acorn_type = stash->cache[slot_idx];

    if (acorn_type != TYPE_EMPTY)
      stash->cache[slot_idx] = type;
  }
}

void clear_type(CACHE *stash, int type)
{
  for (int slot_idx = 0; slot_idx < SLOTS_SIZE; slot_idx++)
  {
    int acorn_type = stash->cache[slot_idx];

    if (acorn_type == type)
      stash->cache[slot_idx] = TYPE_EMPTY;
  }
}