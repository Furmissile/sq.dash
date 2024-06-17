#include "../include/biomes.h"


struct sd_biome *biomes = (struct sd_biome[])
{
  { // GRASSLANDS
    .scene_path = "gl_content/grasslands_scene.gif",

    .file = {
      .name = "Grasslands",
      .path = "gl_content/gl_icon.png",

      .emoji = "gl_icon"
    },

    .intro = "Welcome to the %s **Grasslands**! \n"
        "The biome has luscious green forests, flourishing wildlife, and fertile soils. The land is so rich, sometimes the trees drop golden acorns! \n"
        "The main settlement is Imperium City and has been wreaking havoc on the environment due to political corruption.",

    .sections = (struct sd_biome_section[])
    {
      {
        .name = "Enchanted Woods",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Fluffy Unicorn",
            .description = "*The unicorn seems friendly... perhaps you could just sneak that acorn from under him?* \n",
            .path = "gl_content/encounters/unicorn.png",

            .context = "Inhabited by all sorts of mythical creatures."
          },
          {
            .name = "Burly Pupper",
            .description = "*A werewolf has spotted you and begins to chase you!* \n",
            .path = "gl_content/encounters/werewolf.png",

            .context = "Trees spiral in all sorts of directions, radiating ancient magic."
          },
          {
            .name = "Unique Goldfish",
            .description = "*This isn't your everyday goldfish. What's he doin' out of the water?* \n",
            .path = "gl_content/encounters/magical_goldfish.png",

            .context = "Ponds and rivers glow purple with faint reflections of ancient ruins."
          }
        },

        .encounters_size = 3
      },
      {
        .name = "Imperium City",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Rat Infestation",
            .description = "*Rats! They surely don't like you. Who knows what they could be carrying...* \n",
            .path = "gl_content/encounters/city_rat.png",

            .context = "The city is made entirely of white marble."
          },
          {
            .name = "Bold Coyotes",
            .description = "*These coyotes keep inching towards you. They're curious but do you want to take that chance?* \n",
            .path = "gl_content/encounters/coyote.png",

            .context = "Its citizens are known for their curiosity, intellect, and profound fascination with the nearby Enchanted Woods."
          },
          {
            .name = "Rampant Squirrels",
            .description = "*Still cute and fluffy- sort of. He doesn't like other squirrels though.* \n",
            .path = "gl_content/encounters/savage_squirrel.png",

            .context = "Political corruption has led to pollution, high rodent populations, and agitated wildlife."
          }
        },

        .encounters_size = 3
      },
      {
        .name = "Desolate Plains",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Art of Hunting",
            .description = "*A camouflauge snare has caught you! It didn't do much, the flimsy thing.* \n",
            .path = "gl_content/encounters/bandit_snare.png",

            .context = "Once farmland that fed the city's residents."
          },
          {
            .name = "Bullseye",
            .description = "*You spot a bandit hunter in the corner of your eye as he draws his bow!* \n",
            .path = "gl_content/encounters/hunter_detection.png",

            .context = "Now swarms with rebels who are unhappy with the Imperium City's conditions."
          },
          {
            .name = "Suspicious Crops",
            .description = "*Unlike the other crops, this one is sticky...* \n",
            .path = "gl_content/encounters/poisoned_crop.png",

            .context = "Rebels burn farmland, poison crops, and poach local wildlife."
          }
        },

        .encounters_size = 3
      },
      {
        .name = "Witch Swamp",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Is it an Acorn?",
            .description = "*This acorn looks like an acorn and tastes like an acorn. Is it an acorn?* \n",
            .path = "gl_content/encounters/conjured_acorn.png",

            .context = "Witches were once reputable alchemists in the city."
          },
          {
            .name = "Are you you?",
            .description = "*It's a friendly squirrel! Is it?* \n",
            .path = "gl_content/encounters/squirrel_doppleganger.png",

            .context = "Twisted by the growing corruption, the witches started conducting dark rituals on animals."
          },
          {
            .name = "Poison from Below! Above?",
            .description = "*A witch has spotted you and threw a flask!* \n",
            .path = "gl_content/encounters/thrown_flask.png",

            .context = "The witches' practice was outlawed by Imperium City and the witches fled to the swamp as social outcasts."
          }
        },

        .encounters_size = 3
      }
    },

    .sections_size = 4

  },
  { // SEEPING SANDS
    .scene_path = "sp_content/seeping_sands_scene.gif",

    .file = {
      .name = "Seeping Sands",
      .path = "sp_content/sp_icon.png",

      .emoji = "sp_icon"
    },

    .intro = "Welcome to the %s **Seeping Sands**! \n"
        "The biome is a vast desert wasteland with little to no vegetation that sees frequent sandstorms. "
        "The main settlement here is the Forgotten City whose agriculture malpractice caused the fertile soil to erode.",

    .sections = (struct sd_biome_section[])
    {
      {
        .name = "Sandy Shores",
        .encounters = (struct sd_encounter[])
        { 
          { 
            .name = "Desparate Bobcat",
            .description = "*There's an acorn next to the feasting bobcat.* \n",
            .path = "sp_content/encounters/bobcat.png",

            .context = "The desert is also devoid of prey causing predators to become desperate."
          },
          {
            .name = "Sleeping Cobra",
            .description = "*You've caught the attention of this cobra!* \n",
            .path = "sp_content/encounters/cobra.png",

            .context = "These areas see a dense population of snakes."
          },
          {
            .name = "Distracting Vibrations",
            .description = "*The rattling of the rattlesnake's tail catches your attention and you lock eyes...* \n",
            .path = "sp_content/encounters/rattlesnake.png",

            .context = "Deadly snakes camouflage with the sands."
          }
        },

        .encounters_size = 3
      },
      {
        .name = "Forgotten City",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Vicious Land Lobsters",
            .description = "*A scorpion finds you and charges to attack you!* \n",
            .path = "sp_content/encounters/scorpion.png",

            .context = "The city is built entirely of sandstone and partially overridden with sand."
          },
          {
            .name = "Unhappy Camels",
            .description = "*This camel doesn't look too happy. Watch out for the spit!* \n",
            .path = "sp_content/encounters/angry_camel.png",

            .context = "A once prosperous city for trade went to ruin due to growing internal economic problems."
          },
          {
            .name = "Starving Coyotes",
            .description = "*You've accidentally approached a pack of coyotes!* \n",
            .path = "sp_content/encounters/coyote.png",

            .context = "The locals have resorted to poaching, leaving little for other animals."
          }
        },

        .encounters_size = 3
      },
      {
        .name = "Golden Dunes",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Desert Scavengers",
            .description = "*A pack of hyenas are giving you the death stare...* \n",
            .path = "sp_content/encounters/hyena.png",

            .context = "The vast golden dunes spanning the majority of the desert."
          },
          {
            .name = "The Curious Jackal",
            .description = "*This jackal doesn't seem threatened nor scared. He's, curious?* \n",
            .path = "sp_content/encounters/jackal.png",

            .context = "Animals who don't come in contact with people are often curious, but not all."
          },
          {
            .name = "Desert Puppers",
            .description = "*They look a bit hungry. Maybe not a friendly foe...* \n",
            .path = "sp_content/encounters/wild_dog.png",

            .context = "The whole desert has felt the effects of the impoverished city."
          }
        },

        .encounters_size = 3
      },
      {
        .name = "Ancient Wastelands",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Here Comes the Calvary!",
            .description = "*A living skeleton? A horse? Better hope it's not that unicorn...* \n",
            .path = "sp_content/encounters/skeletal_horse.png",

            .context = "These wastelands are where many ancient battles took place."
          },
          {
            .name = "Squirrel Meets Squirrel",
            .description = "*What looks to be a skeletal squirrel locks eyes with you... Well? You can't just ignore it!* \n",
            .path = "sp_content/encounters/skeletal_squirrel.png",

            .context = "Locals believe that these remains were risen by witches."
          }
        },

        .encounters_size = 2
      },
      {
        .name = "Wormhole",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Shifting Doom",
            .description = "*A massive worm is shifting through the sands...* \n",
            .path = "sp_content/encounters/death_worm.png",

            .context = "Massive worms shift in the sands and feel the vibrations of footsteps."
          },
          {
            .name = "Blood-sucking Fiends!",
            .description = "*Leeches! Leeches everywhere! Small, but best not to let too many latch on!* \n",
            .path = "sp_content/encounters/leech.png",

            .context = "Leeches accompany the worms in large numbers..."
          }
        },

        .encounters_size = 2
      }
    },

    .sections_size = 5
  },
  { // NATURE'S END
    .scene_path = "ne_content/nature_end_scene.gif",

    .file = {
      .name = "Nature's End",
      .path = "ne_content/ne_icon.png",
      .emoji = "ne_icon"
    },

    .intro = "Welcome to %s **Nature's End**! \n"
        "This chaotic biome is characterized by gloomy skies, heavy and unpredictable rain storms, and earthquakes. It gets its name from the neighboring corruption consuming the wildlife. \n"
        "The main settlement here is Light's Refuge.",

    .sections = (struct sd_biome_section[])
    {
      {
        .name = "Light's Refuge",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "The Kind Beast",
            .description = "*This bear doesn't seem one bit intimidated! Perhaps he's friendly?* \n",
            .path = "ne_content/encounters/bear.png",

            .context = "A local oracle resides here to heal the forest."
          },
          {
            .name = "Curious Fox",
            .description = "*The fox looks at you and sits expectantly.* \n",
            .path = "ne_content/encounters/fox.png",

            .context = "The animals that roam here are very friendly."
          },
          {
            .name = "Santa's Good Boi",
            .description = "*Looks like Santa may have lost one! He seems nice...* \n",
            .path = "ne_content/encounters/reindeer.png",

            .context = "Although this section of pine forest is protected, the oracle is struggling to maintain the light."
          }
        },

        .encounters_size = 3
      },
      {
        .name = "Nature's Dying Light",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Desparate Elk",
            .description = "*This elk doesn't appear to be aggressive. Perhaps he's just hungry?* \n",
            .path = "ne_content/encounters/elk.png",

            .context = "The border between the protected and corrupt pine forests."
          },
          {
            .name = "Threatened Moose",
            .description = "*A defensive moose detects you and charges at you!* \n",
            .path = "ne_content/encounters/moose.png",

            .context = "The presence of the corruption causes these animals to be on edge and untrusting."
          },
          {
            .name = "Spooked Wolf",
            .description = "*A twig you stepped on startled a wolf! He's approaching...* \n",
            .path = "ne_content/encounters/wolf.png",

            .context = "Trees are a mix of pine and corrupt trees and the grass has a purple hue."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Breached Woods",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Collapsed Caves",
            .description = "*This bear doesn't seem too happy his home was destroyed...* \n",
            .path = "ne_content/encounters/angry_bear.png",

            .context = "The effects of the corruption have compromised many of the natural structures."
          },
          {
            .name = "Consumed Elk",
            .description = "*Taken by the growing corruption, his mind isn't his own!* \n",
            .path = "ne_content/encounters/consumed_elk.png",

            .context = "Animals will be left with purple marks when they eat corrupt matter."
          },
          {
            .name = "Corrupt Squirrel",
            .description = "*This one doesn't want to shake your paw... You're his snack!* \n",
            .path = "ne_content/encounters/corrupt_squirrel.png",

            .context = "Once the animals have been consumed by the corruption, its mind is no longer its own."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Ancient Ruins",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Sewage Rats",
            .description = "*[dry heaves] Those rats STINK! Where have they been, the sewers?* \n",
            .path = "ne_content/encounters/city_rat.png",

            .context = "Once a colony of Imperium City, it fell to the corruption roughly 63 years ago."
          },
          {
            .name = "Aggressive Trash Pandas",
            .description = "*Is it growling? Is it smiling? Does it want your acorns?* \n",
            .path = "ne_content/encounters/raccoon.png",

            .context = "While this area remains somewhat intact, many animals reside here to buy themselves time to escape the corruption."
          },
          {
            .name = "Ancient Squirrel",
            .description = "*This squirrel has seen a bit... Can he see?* \n",
            .path = "ne_content/encounters/skeletal_squirrel.png",

            .context = "Not only can the corruption consume the living, but it can also raise the deceased."
          }
        },

        .encounters_size = 3
      }
    },

    .sections_size = 4
  },
  { // DEATH'S GRIP
    .scene_path = "dg_content/death_grip_scene.gif",

    .file = {
      .name = "Death's Grip",
      .path = "dg_content/dg_icon.png",

      .emoji = "dg_icon"
    },

    .intro = "Welcome to %s **Death's Grip**! \n"
        "This biome is a desolate tundra with little life. It gets its name from its naturally unforgiving climate and the growing threat of corruption. What was once a white and bright tundra is now riddled with necrotic energy and heavy fog. \n"
        "The main settlement here is the Kingdom of Ice.",

    .sections = (struct sd_biome_section[])
    {
      { 
        .name = "Boreal Forest",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Fluffy Bunn- uh, Foxes",
            .description = "*An arctic fox's head pops above the snow and locks eyes with you.* \n",
            .path = "dg_content/encounters/arctic_fox.png",

            .context = "The average height of snow for this area is roughly 3 feet."
          },
          {
            .name = "Sneaky Puppers",
            .description = "*You hear snow patting and realize a wolf is stalking you!* \n",
            .path = "dg_content/encounters/arctic_wolf.png",

            .context = "The animals are hostile from hunger, no protection from the elements, and the neighboring corruption."
          },
          {
            .name = "Waddle Waddle",
            .description = "*This curious penguin approaches you with intrigument.* \n",
            .path = "dg_content/encounters/penguin.png",

            .context = "The curious animals will expect something in return for their bravery."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Necrotic Lakes",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Purple Energy",
            .description = "*That- that's a squirrel? So dark. So smokey. So... deadly...* \n",
            .path = "dg_content/encounters/dark_skeletal_squirrel.png",

            .context = "These lakes are dense with black mist."
          },
          {
            .name = "Squirrel-o-mancer",
            .description = "*Does he raise- tiny fluffballs of doom? Should you find out?* \n",
            .path = "dg_content/encounters/necro-squirrel.png",

            .context = "As you draw closer to the corruption, these corrupt animals develop strategy and critical thinking."
          },
          {
            .name = "Re-foraging",
            .description = "*This poor guy was in the unfortunate hands of a necro-squirrel.* \n",
            .path = "dg_content/encounters/undead_squirrel.png",

            .context = "Some were unfortunate enough to be raised back into the hell they wanted to leave."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Kingdom of Ice",
        .encounters = (struct sd_encounter[])
        {
          {
            .name = "Desperate for Warmth",
            .description = "*This squirrel has been wandering aimlessly for hours. But he finds a warm companion...* \n",
            .path = "dg_content/encounters/frost_squirrel.png",

            .context = "Built with packed snow and hardened ice supported by boreal beams."
          },
          {
            .name = "Nice Teddy Bear",
            .description = "*This polar bear appears to be a bit lonely...* \n",
            .path = "dg_content/encounters/polar_bear.png",

            .context = "The animals that manage to survive here are far and few between."
          },
          {
            .name = "Definitely a Fluffy Bunny",
            .description = "*He's so cute, small and squishy you could just-* \n",
            .path = "dg_content/encounters/snowshoe_hare.png",

            .context = "There's an unusually large bunny population."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Lost City of Honor",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Bony Teddy Bear",
            .description = "*Those claws look sharp and it doesn't look happy...* \n",
            .path = "dg_content/encounters/skeletal_bear.png",

            .context = "Closest to the corruption, this city was the first to fall."
          },
          {
            .name = "Future Me?",
            .description = "*We bare uncanny resemblance, but it prefers to be alone.* \n",
            .path = "dg_content/encounters/skeletal_squirrel.png",

            .context = "Some animals here must have been raised by necromancers and their behavior tells us they retain their memories."
          },
          {
            .name = "Not so Fluffy...",
            .description = "*Can it see? Can it hear? What do you do?* \n",
            .path = "dg_content/encounters/skeletal_wolf.png",

            .context = "Necromancers prefer ruins to prevent their rituals from being found."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Death's Valley",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Less Fluffy Bunn- uh, Fox",
            .description = "*This fox has been at the mercy of some necromancers... Not friendly.* \n",
            .path = "dg_content/encounters/consumed_fox.png",

            .context = "Animals here are used by necromancers to do their bidding."
          },
          {
            .name = "Nature's Dilemma",
            .description = "*So cute but scary at the same!* \n",
            .path = "dg_content/encounters/consumed_hare.png",

            .context = "Even the cutest of animals fall to the corruption."
          },
          {
            .name = "Angry Teddy Bear",
            .description = "*This polar bear doesn't look too happy about his situation...* \n",
            .path = "dg_content/encounters/consumed_polar_bear.png",

            .context = "What few animals manage to survive here are in a perpetual state of fear."
          }
        },

        .encounters_size = 3
      }
    },

    .sections_size = 5
  },
  { // THE LAST ACORN
    .scene_path = "la_content/la_scene.gif",

    .file = {
      .name = "The Last Acorn",
      .path = "la_content/la_icon.png",

      .emoji = "la_icon"
    },

    .intro = "Welcome to the %s **The Last Acorn**! \n"
        "This biome looks identical to the Grasslands, but it's characterized by dense purple fog, humming dark energy, the blood of the sacrificed coursing through the dirt, and crazed squirrels who once inhabited the area. Its name came from the squirrel colonies because squirrels are lured in by conjured acorns and it's the last acorn they ever see. \n"
        "There is no settlement here.",

    .sections = (struct sd_biome_section[]) 
    {
      { 
        .name = "Weeping Woods",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Dark Pupper Energy",
            .description = "*This pupper is different from the rest... he's riddled with dark energy!* \n",
            .path = "la_content/encounters/wolf_familiar.png",

            .context = "These animals are born and raised by the corruption."
          },
          {
            .name = "Fox Zoomies",
            .description = "*The foxes notices you, locks eyes with you, pupils grow large...* \n",
            .path = "la_content/encounters/fox_familiar.png",
            
            .context = "The corruption trains these familiars to be unsuspectingly deadly predators."
          },
          {
            .name = "The Less Fortunate",
            .description = "*Was once a happy squirrel, but was unfortunate enough to stumble upon this corruption...* \n",
            .path = "la_content/encounters/possessed_squirrel.png",
            
            .context = "This squirrel must have eaten a seemingly normal acorn..."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Shed of Light",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Harmless Fluff Ball",
            .description = "*What a lonely little hare! He starts to approach you...* \n",
            .path = "la_content/encounters/woodland_hare.png",
            
            .context = "The only spot the corruption didn't take for unknown reasons."
          },
          {
            .name = "Happy Elk",
            .description = "*This elk is happy. He's surrounded by food and other elk!* \n",
            .path = "la_content/encounters/elk.png",
            
            .context = "Animals here are happier than those in the oracle's protection."
          },
          {
            .name = "Snoozing Bear",
            .description = "*This bear seems content in his slumber. But there's an acorn behind him!* \n",
            .path = "la_content/encounters/bear.png",
            
            .context = "The oracle has been seeking answers as to what protects this little area."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Haunted Pasture",
        .encounters = (struct sd_encounter[])
        {
          {
            .name = "The Acorn Dilemma",
            .description = "*This acorn looks like an acorn and tastes like an acorn. Is it an acorn?* \n",
            .path = "la_content/encounters/conjured_acorn.png",
            
            .context = "Illusions are created by the witches to trap unsuspecting animals."
          },
          {
            .name = "Random Goldfish",
            .description = "*This goldfish is flopping around on the ground far away from any body of water...* \n",
            .path = "la_content/encounters/conjured_goldfish.png",
            
            .context = "Witches here and can recreate your memories as if they were reality."
          },
          {
            .name = "It's a Normal Squirrel, Right?",
            .description = "*A suspicious-looking squirrel is feasting on some victuals.* \n",
            .path = "la_content/encounters/squirrel_doppleganger.png",
            
            .context = "The necromancers will use the powers of the corruption to revive the deceased and the witches disguise them."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Plain of Embers",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Flaming Equine",
            .description = "*It looks peaceful despite its appearance.* \n",
            .path = "la_content/encounters/nightmare.png",
            
            .context = "These creatures are neither animal nor familiar, but hellspawn."
          },
          {
            .name = "A fellow Bunny?",
            .description = "*But he's just A BUNNY!* \n",
            .path = "la_content/encounters/molten_bunny.png",
            
            .context = "The grass is hot to the touch and releases sparks."
          },
          {
            .name = "To Touch Or Not To Touch",
            .description = "*It moves without a trace! But it's so tempting to touch it...* \n",
            .path = "la_content/encounters/living_dark_flame.png",
            
            .context = "Hellspawn come from a portal that connects Earth to Hell."
          }
        },

        .encounters_size = 3
      },
      { 
        .name = "Death's Locus",
        .encounters = (struct sd_encounter[])
        { 
          {
            .name = "Squirrel Power",
            .description = "*This squirrel has learned to wield the corruptive powers!* \n",
            .path = "la_content/encounters/dark_squirrel_mage.png",
            
            .context = "These squirrels draw off the corruption's power with the intent of dominating the world."
          },
          {
            .name = "Squirrel Fanatic",
            .description = "*Brainwashed by the corruptive forces, this squirrel isn't natural!* \n",
            .path = "la_content/encounters/squirrel_cultist.png",
            
            .context = "Any human or animal that enters the corruption never makes it out in one piece."
          },
          {
            .name = "The Talking Squirrel",
            .description = "*It speaks but its mouth doesn't move? What is it saying?* \n",
            .path = "la_content/encounters/familiar_skeletal_squirrel.png",
            
            .context = "Mind control can become so powerful that some animals can chant. But what are they saying?"
          }
        },

        .encounters_size = 3
      }
    },

    .sections_size = 5
  }
};