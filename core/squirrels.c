#include "../include/squirrels.h" /* squirrels content */


struct sd_squirrel *squirrels = (struct sd_squirrel[])
{
  // BASE SQUIRREL
  {
    .file = {
      .name = "Gray Squirrel",
      .path = "squirrels/gray_squirrel.png",

      .description = "*Your traditional squirrel. Vicious and ferocious.*",

      .emoji = "gray_squirrel"
    }
  },
  // TRADITIONAL SQUIRRELS
  {
    .cost = 5,
    .file = {
      .name = "Skeletal Squirrel",
      .path = "squirrels/skeletal_squirrel.png",

      .description = "*Seen one too many moons...*",

      .emoji = "skeletal_squirrel"
    }
  },
  {
    .cost = 10,
    .file = {
      .name = "Squirrel Bookie",
      .path = "squirrels/squirrel_bookie.png",

      .description = "*He got the numbers if you got the coin.*",

      .emoji = "squirrel_bookie"
    }
  },
  {
    .cost = 15,
    .file = {
      .name = "Angelic Squirrel",
      .path = "squirrels/angelic_squirrel.png",

      .description = "*100% pure holiness... and maybe a whisper of greed.*",

      .emoji = "angelic_squirrel"
    }
  },
  {
    .cost = 25,
    .file = {
      .name = "King Squirrel",
      .path = "squirrels/king_squirrel.png",

      .description = "*Your royal fuzziness. Stands proud amongst his peers.*",

      .emoji = "king_squirrel"
    }
  },
  // SPRING
  {
    .cost = FIRST_SQUIRREL,
    .file = {
      .name = "Floral Squirrel",
      .path = "squirrels/spring_squirrels/floral_squirrel.png",

      .description = "*Stuck his nose in one too many flowers.*",

      .emoji = "floral_squirrel"
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .file = {
      .name = "Golden Squirrel",
      .path = "squirrels/spring_squirrels/golden_squirrel.png",

      .description = "*Did he eat a golden acorn?*",

      .emoji = "golden_squirrel"
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .file = {
      .name = "Black Squirrel",
      .path = "squirrels/spring_squirrels/black_squirrel.png",

      .description = "*Something about him is unsettling...*",

      .emoji = "black_squirrel"
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .file = {
      .name = "Acron",
      .path = "squirrels/spring_squirrels/acron.png",

      .description = "*Always searching for the next golden acorn.*",

      .emoji = "acron"
    }
  },

  // SUMMER
  {
    .cost = FIRST_SQUIRREL,
    .file = {
      .name = "Floaty Squirrel",
      .path = "squirrels/summer_squirrels/floaty_squirrel.png",

      .description = "*Soaking up the sun.*",

      .emoji = "floaty_squirrel"
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .file = {
      .name = "Beach Squirrel",
      .path = "squirrels/summer_squirrels/beach_squirrel.png",

      .description = "*What happened at the beach?*",

      .emoji = "beach_squirrel"
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .file = {
      .name = "Squirrel Pirate",
      .path = "squirrels/summer_squirrels/squirrel_pirate.png",

      .description = "*Proper party crasher.*",

      .emoji = "squirrel_pirate"
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .file = {
      .name = "Veggie Snatcher",
      .path = "squirrels/summer_squirrels/veggie_snatcher.png",

      .description = "*Raided more gardens than you can count!*",

      .emoji = "veggie_snatcher"
    }
  },

  // FALL
  {
    .cost = FIRST_SQUIRREL,
    .file = {
      .name = "Red Squirrel",
      .path = "squirrels/fall_squirrels/red_squirrel.png",

      .description = "*Blends with the Fall leaves.*",

      .emoji = "red_squirrel"
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .file = {
      .name = "Squirrel Alchemist",
      .path = "squirrels/fall_squirrels/squirrel_alchemist.png",

      .description = "*Ready to create the next enchanted acorn.*",

      .emoji = "squirrel_alchemist"
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .file = {
      .name = "Demonic Squirrel",
      .path = "squirrels/fall_squirrels/demonic_squirrel.png",

      .description = "*Fresh from the depths of Hell.*",

      .emoji = "demonic_squirrel"
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .file = {
      .name = "Archwizard Flooftail",
      .path = "squirrels/fall_squirrels/archwizard_flooftail.png",

      .description = "*Sourcerer with powers of an unknown origin.*",

      .emoji = "archwizard_flooftail"
    }
  },

  // WINTER
  {
    .cost = FIRST_SQUIRREL,
    .file = {
      .name = "Frost Squirrel",
      .path = "squirrels/winter_squirrels/frost_squirrel.png",

      .description = "*Loves the snow a little too much.*",

      .emoji = "frost_squirrel"
    }
  },
  {
    .cost = SECOND_SQUIRREL,
    .file = {
      .name = "Hoarder Squirrel",
      .path = "squirrels/winter_squirrels/hoarder_squirrel.png",

      .description = "*No one will take his acorns...*",

      .emoji = "hoarder_squirrel"
    }
  },
  {
    .cost = THIRD_SQUIRREL,
    .file = {
      .name = "Santa Squirrel",
      .path = "squirrels/winter_squirrels/santa_squirrel.png",

      .description = "*The inventor of acorn.*",

      .emoji = "santa_squirrel"
    }
  },
  {
    .cost = FOURTH_SQUIRREL,
    .file = {
      .name = "Infurno",
      .path = "squirrels/winter_squirrels/infurno.png",

      .description = "*Fluffy jellybe- spawn of Hell.*",

      .emoji = "infurno"
    }
  },
};