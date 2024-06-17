#include "../include/game.h"
#include "../include/items.h"
#include "../include/icons.h"
#include "../include/emojis.h"


int piece_generation(PIECES *shuffle, int ref_piece, int piece_count)
{
  int piece_idx = -1;

  do {
    int chance = rand() % 100;

    int piece_size = -1;
    int piece_size_range = -1;

    // generate any piece if next OR current are small
    if (ref_piece < 4 || shuffle->current < 4)
    {
      piece_size_range = (chance < 20) ? 1 // 20% for 1
          : (chance < 40) ? 2              // 20% for 2
          : (chance < 70) ? 3 : 4;         // 30% for 3 and 20% for 4
    }
    else {
      piece_size_range = (chance < 50) ? 1 : 2;
    }

    piece_size = rand() % piece_size_range;
    piece_idx = (piece_count % 2) + (2 * piece_size);
  }
  // next cannot equal current OR the other piece
  while (piece_idx == shuffle->current
    || piece_idx == ref_piece);

  return piece_idx;
}

void build_progress_bar(struct sd_field *field, struct sd_game *game)
{
  char* indent = ICON_EMOJI(ICON_INDENT);
  char* active_bar = ICON_EMOJI(ICON_PROGRESS);
  char* inactive_bar = ICON_EMOJI(ICON_INACTIVE);
  
  u_snprintf(field->value, VALUE_SIZE, "\n%s%s%s", indent, indent, fetch_emoji(biomes[game->biome].file.emoji));

  int req_progress = BASE_BIOME + ((game->level -1) * BIOME_INTERVAL);
  int progress_bar = (float)game->progress / req_progress *BAR_SIZE;

  for (int progress_idx = 0; progress_idx < BAR_SIZE; progress_idx++)
  {
    u_snprintf(field->value, VALUE_SIZE, 
        (progress_bar > progress_idx) ? active_bar : inactive_bar);
  }

  int next_biome = (game->biome +1) % BIOME_SIZE;
  u_snprintf(field->value, VALUE_SIZE, fetch_emoji(biomes[next_biome].file.emoji));
}

void decorate_piece(CACHE *piece)
{
  struct tm *info = get_UTC();

  int has_garden_victual = 0;
  int has_christmas_resource = 0;

  for (int slot_idx = 0; slot_idx < SLOTS_SIZE; slot_idx++)
  {
    // chance for certain item types!
    if (piece->cache[slot_idx] != TYPE_EMPTY)
    {
      int chance = rand() % MAX_CHANCE;

      piece->cache[slot_idx] = (chance < COMMON_CHANCE) ? TYPE_ACORNS
          : (chance < RARE_CHANCE) ? TYPE_GOLDEN_ACORNS : TYPE_CONJURED_ACORNS;

      if (has_garden_victual == 0 
        && rand() % MAX_CHANCE > EVENT_CHANCE)
      {        
        piece->cache[slot_idx] = (chance < COMMON_CHANCE) ? TYPE_SPRING_SEEDS
            : (chance < RARE_CHANCE) ? TYPE_SUMMER_CHERRIES : TYPE_FALL_BLUEBERRIES;
        
        has_garden_victual = 1;
      }

      // chance separately for ribboned acorn
      if (info->tm_mon == CHRISTMAS_MONTH
        && has_christmas_resource == 0
        && rand() % MAX_CHANCE > EVENT_CHANCE)
      {
        piece->cache[slot_idx] = (rand() % MAX_CHANCE > 75) ? TYPE_RIBBONED_ACORN : TYPE_COAL;
        has_christmas_resource = 1;
      }
    }
  }

}

void use_leafy_acorn(CACHE *stash, int tree_idx)
{
  // for each adjacent slot
  for (int slice_idx = tree_idx -1; slice_idx < tree_idx +2; slice_idx++)
  {
    // if idx < 0 ? start from end : take remainder
    int acorn_idx = (slice_idx < 0) ? SLOTS_SIZE - abs(slice_idx) : slice_idx % SLOTS_SIZE;

    // overwrite acorn type depending on type already present
    int acorn_type = stash->cache[acorn_idx];

    // only one can be true!!
    if (acorn_type == TYPE_TREE)
      stash->cache[acorn_idx] = TYPE_GOLDEN_ACORNS;
    else if (acorn_type == TYPE_EMPTY)
      stash->cache[acorn_idx] = TYPE_ACORNS;
    else if (acorn_type < TYPE_RIBBONED_ACORN)
      stash->cache[acorn_idx]++;
  }
}


void rotate_piece_cw(CACHE *piece)
{
  // save old stash
  CACHE stash_copy = *piece;

  // reset piece
  *piece = (CACHE) { 0 };

  int stash_idx = 0;

  // shift end values to start values
  for (; stash_idx < 2; stash_idx++)
  {
    piece->cache[stash_idx] = stash_copy.cache[SLOTS_SIZE - 2 + stash_idx];
  }

  // fill in the rest
  for (; stash_idx < SLOTS_SIZE; stash_idx++)
  {
    piece->cache[stash_idx] = stash_copy.cache[stash_idx -2];
  }

}

void rotate_piece_ccw(CACHE *piece)
{
  // save old stash
  CACHE stash_copy = *piece;

  // reset piece
  *piece = (CACHE) { 0 };

  int stash_idx = 0;

  // shift end values to start values
  for (; stash_idx < 2; stash_idx++)
  {
    piece->cache[SLOTS_SIZE -2 + stash_idx] = stash_copy.cache[stash_idx];
  }

  // fill in the rest
  for (; stash_idx < SLOTS_SIZE; stash_idx++)
  {
    piece->cache[stash_idx -2] = stash_copy.cache[stash_idx];
  }
}

void spawn_encounter(struct sd_game *game)
{
  game->section = rand() % biomes[game->biome].sections_size;
  game->encounter = rand() % biomes[game->biome].sections[game->section].encounters_size;

  game->shuffle.current = piece_generation(&game->shuffle, game->shuffle.next, game->piece_count);
  merge_caches(&game->current_piece, &pieces[game->shuffle.current]);
  decorate_piece(&game->current_piece);
}

void shift_pieces(struct sd_game *game)
{
  // merge next into current
  merge_caches(&game->current_piece, &game->next_piece);

  // shift indexes
  game->shuffle.prev = game->shuffle.current;
  game->shuffle.current = game->shuffle.next;

  game->shuffle.next = piece_generation(&game->shuffle, game->shuffle.prev, game->piece_count);

  // clear and set next piece
  game->next_piece = (CACHE) { 0 };
  merge_caches(&game->next_piece, &pieces[game->shuffle.next]);

  game->piece_count++;

  if ((game->shuffle.next == PIECE_CORNER_ONE || game->shuffle.next == PIECE_CENTER_ONE)
    && rand() % MAX_CHANCE > EVENT_CHANCE)
  {
    int slot_idx = (game->shuffle.next == PIECE_CORNER_ONE) ? SLOT_TOP_LEFT : SLOT_TOP_CENTER;

    game->next_piece.cache[slot_idx] = TYPE_LEAFY_ACORN;

    return;
  }

  decorate_piece(&game->next_piece);
}

void set_daily(const struct discord_interaction *event, struct sd_player *player)
{
  PGresult *update_daily = (PGresult*) { 0 };

  struct tm *info = get_UTC();

  if (info->tm_mday != player->tm_mday)
  {
    player->stashes_complete = 0;
    player->tm_mday = info->tm_mday;
  }

  if (player->stashes_complete != ERROR_STATUS
    && player->stashes_complete < DAILY_STASHES)
  {
    player->stashes_complete++;
  }
  
  if (player->stashes_complete == DAILY_STASHES)
  {
    player->stashes_complete = ERROR_STATUS;
    player->daily_complete = 1;
    player->conjured_acorns += DAILY_REWARD;
  }

  // conjured acorns are always updated later on!
  update_daily = SQL_query(update_daily, "update public.player set stashes_complete = %d, tm_mday = %d where user_id = %ld",
      player->stashes_complete, player->tm_mday, event->member->user->id);
  PQclear(update_daily);
}

void spawn_biome_disaster(struct sd_player *player, struct sd_game *game)
{
  CACHE disaster = (CACHE) { 0 };

  merge_caches(&disaster, &pieces[rand() % PIECES_SIZE]);
  fill_type(&disaster, TYPE_CURSE + game->biome);

  int rotate_count = rand() % 4;

  for (int rotate_idx = 0; rotate_idx < rotate_count; rotate_idx++)
  {
    rotate_piece_cw(&disaster);
  }

  int stash_select = rand() % MAX_STASHES;

  while (stash_select == player->button_idx)
  {
    stash_select = rand() % MAX_STASHES;
  }

  game->disaster_stash = stash_select;

  merge_caches(&game->stashes[stash_select], &disaster);
}