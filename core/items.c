#include "../include/items.h"

struct sd_stash_item *items = (struct sd_stash_item[])
{
  /*
    BASE ACORNS
  */
  { // TYPE_EMPTY
    .file = {
      .emoji = "empty_acorn",
      .path = "items/empty_acorn.png"
    }
  },
  { // TYPE_ACORN_COUNT
    .file = {
      .name = "Acorn Count",
      .path = "items/acorn_count.png",

      .emoji = "acorn_count"
    }
  },
  { // TYPE_ACORNS
    .file = {
      .name = "Acorns",
      .path = "items/acorn.png",

      .emoji = "acorn"
    },

    .value = 1
  },
  { // TYPE_GOLDEN_ACORNS
    .file = {
      .name = "Golden Acorns",
      .path = "items/golden_acorn.png",

      .emoji = "golden_acorn"
    },

    .value = 3
  },
  { // TYPE_CONJURED_ACORNS
    .file = {
      .name = "Conjured Acorn",
      .path = "items/conjured_acorn.png",

      .emoji = "conjured_acorn"
    },
    
    .value = 5
  },

  /*
    SEASONAL ITEMS
  */
  { // TYPE_SPRING_SEEDS
    .file = {
      .name = "Seeds",
      .path = "victuals/seed_victuals.png",

      .emoji = "seed_victuals"
    },

    .value = 6
  },
  { // TYPE_SUMMER_CHERRIES
    .file = {
      .name = "Cherries",
      .path = "victuals/cherry_victuals.png",

      .emoji = "cherry_victuals"
    },

    .value = 7
  },
  { // TYPE_FALL_BLUEBERRIES
    .file = {
      .name = "Blueberries",
      .path = "victuals/blueberry_victuals.png",

      .emoji = "blueberry_victuals"
    },

    .value = 8
  },
  { // TYPE_RIBBONED_ACORN
    .file = {
      .name = "Ribboned Acorn",
      .path = "items/ribboned_acorn.png",

      .emoji = "ribboned_acorn"
    },

    .value = 9
  },
  { // TYPE_COAL
    .file = {
      .name = "Coal",
      .path = "items/coal.png",

      .emoji = "coal"
    }
  },

  /*
    SPECIAL ITEMS
  */
  { // TYPE_LEAFY_ACORN
    .file = {
      .name = "Leafy Acorn",
      .path = "items/leafy_acorn.png",

      .emoji = "leafy_acorn"
    },

    .value = 3
  },
  { // TYPE_TREE
    .file = {
      .name = "Tree",
      .path = "items/tree.png",

      .emoji = "tree"
    },

    .value = 3
  },
  { // TYPE_POP
    .file = {
      .name = "Popped!",
      .path = "items/pop.gif",

      .emoji = "pop"
    }
  },

  /*
    DISASTER ITEMS
  */
  { // TYPE_CURSE
    .file = {
      .name = "Curse",
      .path = "event_acorns/cursed_acorn.png",

      .emoji = "cursed_acorn"
    }
  },
  { // TYPE_HEAT_WAVE
    .file = {
      .name = "Heat Wave",
      .path = "event_acorns/flaming_acorn.png",

      .emoji = "flaming_acorn"
    }
  },
  { // TYPE_RAINSTORM
    .file = {
      .name = "Rainstorm",
      .path = "event_acorns/energy.png",

      .emoji = "energy"
    }
  },
  { // TYPE_FROZEN_ACORN
    .file = {
      .name = "Blizzard",
      .path = "event_acorns/frozen_acorn.png",

      .emoji = "frozen_acorn"
    }
  },
  { // TYPE_ARCANE_BLAST
    .file = {
      .name = "Arcane Blast",
      .path = "event_acorns/arcane_acorn.png",

      .emoji = "arcane_acorn"
    }
  }
};