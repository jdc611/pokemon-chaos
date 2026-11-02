[SPECIES_GLE_AWAKENED_SLAKING] =
    {
        .baseHP        = 150,
        .baseAttack    = 160,
        .baseDefense   = 100,
        .baseSpeed     = 100,
        .baseSpAttack  = 95,
        .baseSpDefense = 65,
        .types = MON_TYPES(TYPE_NORMAL),
        .catchRate = 45,
    #if P_UPDATED_EXP_YIELDS >= GEN_8
        .expYield = 285,
    #elif P_UPDATED_EXP_YIELDS >= GEN_5
        .expYield = 252,
    #else
        .expYield = 210,
    #endif
        .evYield_HP = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_TRUANT, ABILITY_NONE, ABILITY_VITAL_SPIRIT },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("Awk.Slaking"),
        .cryId = CRY_SLAKING,
        .natDexNum = NATIONAL_DEX_SLAKING,
        .categoryName = _("Lazy"),
        .height = 20,
        .weight = 1305,
        .description = COMPOUND_STRING(
            "Hordes of Slaking gather around trees\n"
            "when fruits come into season. They wait\n"
            "around patiently for ripened fruits to fall\n"
            "out of the trees."),
        .pokemonScale = 256,
        .pokemonOffset = 2,
        .trainerScale = 300,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_Slaking,
        .frontPicSize = P_GBA_STYLE_SPECIES_GFX ? MON_COORDS_SIZE(64, 48) : MON_COORDS_SIZE(64, 56),
        .frontPicYOffset = P_GBA_STYLE_SPECIES_GFX ? 8 : 7,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(0, 22),
            ANIMCMD_FRAME(1, 22),
            ANIMCMD_FRAME(0, 22),
            ANIMCMD_FRAME(1, 22),
            ANIMCMD_FRAME(0, 22),
            ANIMCMD_FRAME(1, 22),
            ANIMCMD_FRAME(0, 22),
        ),
        .frontAnimId = ANIM_V_SQUISH_AND_BOUNCE_SLOW,
        .backPic = gMonBackPic_Slaking,
        .backPicSize = P_GBA_STYLE_SPECIES_GFX ? MON_COORDS_SIZE(64, 48) : MON_COORDS_SIZE(64, 56),
        .backPicYOffset = P_GBA_STYLE_SPECIES_GFX ? 8 : 6,
        .backAnimId = BACK_ANIM_H_SHAKE,
        .palette = sGleAwakenedSlakingPalette,
        .shinyPalette = sGleAwakenedSlakingShinyPalette,
        .iconSprite = gMonIcon_Slaking,
        .iconPalIndex = P_GBA_STYLE_SPECIES_ICONS ? 1 : 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 6, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Slaking)
        OVERWORLD(
            sPicTable_Slaking,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Slaking,
            gShinyOverworldPalette_Slaking
        )
        .levelUpLearnset = sGleAwakenedSlakingMoves,
        .teachableLearnset = sSlakingTeachableLearnset,
    },

[SPECIES_GLE_RAZORSTONE_KLEAVOR] =
    {
        .baseHP        = 70,
        .baseAttack    = 135,
        .baseDefense   = 95,
        .baseSpeed     = 85,
        .baseSpAttack  = 45,
        .baseSpDefense = 70,
        .types = MON_TYPES(TYPE_BUG, TYPE_ROCK),
        .catchRate = 15,
        .expYield = 175,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_BUG),
        .abilities = { ABILITY_TECHNICIAN, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BROWN,
        .speciesName = _("RazorKleavor"),
        .cryId = CRY_KLEAVOR,
        .natDexNum = NATIONAL_DEX_KLEAVOR,
        .categoryName = _("Axe"),
        .height = 18,
        .weight = 890,
        .description = COMPOUND_STRING(
            "A violent creature that fells trees with\n"
            "its crude axes and shields itself with hard\n"
            "stone. Should one encounter this Pokémon\n"
            "in the wild, one's only recourse is to flee."),
        .pokemonScale = 267,
        .pokemonOffset = 2,
        .trainerScale = 286,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_Kleavor,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_Kleavor,
        .backPicSize = MON_COORDS_SIZE(64, 56),
        .backPicYOffset = 4,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleRazorstoneKleavorPalette,
        .shinyPalette = sGleRazorstoneKleavorShinyPalette,
        .iconSprite = gMonIcon_Kleavor,
        .iconPalIndex = 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(2, 12, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Kleavor)
        OVERWORLD(
            sPicTable_Kleavor,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Kleavor,
            gShinyOverworldPalette_Kleavor
        )
        .levelUpLearnset = sGleRazorstoneKleavorMoves,
        .teachableLearnset = sKleavorTeachableLearnset,
    },

[SPECIES_GLE_UNCROWNED_BISHARP] =
    {
        .baseHP        = 85,
        .baseAttack    = 145,
        .baseDefense   = 110,
        .baseSpeed     = 90,
        .baseSpAttack  = 60,
        .baseSpDefense = 90,
        .types = MON_TYPES(TYPE_DARK, TYPE_STEEL),
        .catchRate = 45,
        .expYield = 172,
        .evYield_Attack = 2,
        .itemRare = ITEM_LEADERS_CREST,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = 35,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_HUMAN_LIKE),
        .abilities = { ABILITY_SHARPNESS, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("Unc.Bisharp"),
        .cryId = CRY_BISHARP,
        .natDexNum = NATIONAL_DEX_BISHARP,
        .categoryName = _("Sword Blade"),
        .height = 16,
        .weight = 700,
        .description = COMPOUND_STRING(
            "This pitiless Pokémon commands a group\n"
            "of Pawniard to hound prey into immobility.\n"
            "It battles to become the boss, but will be\n"
            "driven from the group if it loses."),
        .pokemonScale = 259,
        .pokemonOffset = 1,
        .trainerScale = 296,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_Bisharp,
        .frontPicSize = MON_COORDS_SIZE(48, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 15),
            ANIMCMD_FRAME(0, 15),
        ),
        .frontAnimId = ANIM_H_STRETCH,
        .backPic = gMonBackPic_Bisharp,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 3,
        .backAnimId = BACK_ANIM_GROW_STUTTER,
        .palette = sGleUncrownedBisharpPalette,
        .shinyPalette = sGleUncrownedBisharpShinyPalette,
        .iconSprite = gMonIcon_Bisharp,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-2, 14, SHADOW_SIZE_M)
        FOOTPRINT(Bisharp)
        OVERWORLD(
            sPicTable_Bisharp,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Bisharp,
            gShinyOverworldPalette_Bisharp
        )
        .levelUpLearnset = sGleUncrownedBisharpMoves,
        .teachableLearnset = sBisharpTeachableLearnset,
    #if P_GEN_9_CROSS_EVOS
        .evolutions = EVOLUTION({EVO_LEVEL, 0, SPECIES_KINGAMBIT, CONDITIONS({IF_DEFEAT_X_WITH_ITEMS, SPECIES_BISHARP, ITEM_LEADERS_CREST, 3})}, {EVO_ITEM, ITEM_LINKING_CORD, SPECIES_KINGAMBIT}),
    #endif
    },

[SPECIES_GLE_OVERCHARGED_ELECTIVIRE] =
    {
        .baseHP        = 85,
        .baseAttack    = 143,
        .baseDefense   = 85,
        .baseSpeed     = 112,
        .baseSpAttack  = 95,
        .baseSpDefense = 95,
        .types = MON_TYPES(TYPE_ELECTRIC),
        .catchRate = 30,
    #if P_UPDATED_EXP_YIELDS >= GEN_8
        .expYield = 270,
    #elif P_UPDATED_EXP_YIELDS >= GEN_5
        .expYield = 243,
    #else
        .expYield = 199,
    #endif
        .evYield_Attack = 3,
        .itemRare = ITEM_ELECTIRIZER,
        .genderRatio = PERCENT_FEMALE(25),
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_HUMAN_LIKE),
        .abilities = { ABILITY_TOUGH_CLAWS, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("Ovr.Electiv"),
        .cryId = CRY_ELECTIVIRE,
        .natDexNum = NATIONAL_DEX_ELECTIVIRE,
        .categoryName = _("Thunderbolt"),
        .height = 18,
        .weight = 1386,
        .description = COMPOUND_STRING(
            "When it gets excited, it thumps its chest.\n"
            "With every thud, thunder roars, electric\n"
            "sparks shower all around and blue sparks\n"
            "begin to crackle between its horns."),
        .pokemonScale = 267,
        .pokemonOffset = 2,
        .trainerScale = 286,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_Electivire,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 50),
            ANIMCMD_FRAME(1, 15),
            ANIMCMD_FRAME(0, 10),
        ),
        .frontAnimId = ANIM_GLOW_YELLOW,
        .backPic = gMonBackPic_Electivire,
        .backPicSize = MON_COORDS_SIZE(64, 48),
        .backPicYOffset = 9,
        .backAnimId = BACK_ANIM_SHAKE_FLASH_YELLOW,
        .palette = sGleOverchargedElectivirePalette,
        .shinyPalette = sGleOverchargedElectivireShinyPalette,
        .iconSprite = gMonIcon_Electivire,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-3, 11, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Electivire)
        OVERWORLD(
            sPicTable_Electivire,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Electivire,
            gShinyOverworldPalette_Electivire
        )
        .levelUpLearnset = sGleOverchargedElectivireMoves,
        .teachableLearnset = sElectivireTeachableLearnset,
    },

[SPECIES_GLE_ANCIENT_RILLABOOM] =
    {
        .baseHP        = 110,
        .baseAttack    = 145,
        .baseDefense   = 105,
        .baseSpeed     = 95,
        .baseSpAttack  = 60,
        .baseSpDefense = 85,
        .types = MON_TYPES(TYPE_GRASS),
        .catchRate = 45,
        .expYield = 265,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(12.5),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_GRASS),
        .abilities = { ABILITY_GRASSY_SURGE, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Anc.Rillabm"),
        .cryId = CRY_RILLABOOM,
        .natDexNum = NATIONAL_DEX_RILLABOOM,
        .categoryName = _("Drummer"),
        .height = 21,
        .weight = 900,
        .description = COMPOUND_STRING(
            "By drumming, it taps into the power of\n"
            "its special tree stump. The roots of the\n"
            "stump follow its direction in battle."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 365,
        .trainerOffset = 7,
        .frontPic = gMonFrontPic_Rillaboom,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 3,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_Rillaboom,
        .backPicSize = MON_COORDS_SIZE(64, 56),
        .backPicYOffset = 4,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleAncientRillaboomPalette,
        .shinyPalette = sGleAncientRillaboomShinyPalette,
        .iconSprite = gMonIcon_Rillaboom,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(2, 8, SHADOW_SIZE_L)
        FOOTPRINT(Rillaboom)
        OVERWORLD(
            sPicTable_Rillaboom,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Rillaboom,
            gShinyOverworldPalette_Rillaboom
        )
        .levelUpLearnset = sGleAncientRillaboomMoves,
        .teachableLearnset = sRillaboomTeachableLearnset,

    },

[SPECIES_GLE_FROSTBOUND_BAXCALIBUR] =
    {
        .baseHP        = 115,
        .baseAttack    = 145,
        .baseDefense   = 92,
        .baseSpeed     = 87,
        .baseSpAttack  = 75,
        .baseSpDefense = 86,
        .types = MON_TYPES(TYPE_DRAGON, TYPE_ICE),
        .catchRate = 10,
        .expYield = 300,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 40,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_DRAGON, EGG_GROUP_MINERAL),
        .abilities = { ABILITY_SLUSH_RUSH, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("FrostBaxcal"),
        .cryId = CRY_BAXCALIBUR,
        .natDexNum = NATIONAL_DEX_BAXCALIBUR,
        .categoryName = _("Ice Dragon"),
        .height = 21,
        .weight = 2100,
        .description = COMPOUND_STRING(
            "It launches itself into battle by\n"
            "flipping upside down and spewing\n"
            "frigid air from its mouth. It finishes\n"
            "opponents off with its dorsal blade."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Baxcalibur,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_Baxcalibur,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 5,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleFrostboundBaxcaliburPalette,
        .shinyPalette = sGleFrostboundBaxcaliburShinyPalette,
        .iconSprite = gMonIcon_Baxcalibur,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(5, 12, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Baxcalibur)
        OVERWORLD(
            sPicTable_Baxcalibur,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Baxcalibur,
            gShinyOverworldPalette_Baxcalibur
        )
        .levelUpLearnset = sGleFrostboundBaxcaliburMoves,
        .teachableLearnset = sBaxcaliburTeachableLearnset,

    },

[SPECIES_GLE_THUNDERLORD_LUXRAY] =
    {
        .baseHP        = 95,
        .baseAttack    = 120,
        .baseDefense   = 95,
        .baseSpeed     = 120,
        .baseSpAttack  = 95,
        .baseSpDefense = 90,
        .types = MON_TYPES(TYPE_ELECTRIC, TYPE_DARK),
        .catchRate = 45,
    #if P_UPDATED_EXP_YIELDS >= GEN_8
        .expYield = 262,
    #elif P_UPDATED_EXP_YIELDS >= GEN_5
        .expYield = 235,
    #else
        .expYield = 194,
    #endif
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_STRONG_JAW, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("ThndrLuxray"),
        .cryId = CRY_LUXRAY,
        .natDexNum = NATIONAL_DEX_LUXRAY,
        .categoryName = _("Gleam Eyes"),
        .height = 14,
        .weight = 420,
        .description = COMPOUND_STRING(
            "It has eyes which can see through\n"
            "anything. Luxray's ability to see\n"
            "through objects comes in handy when\n"
            "it's scouting for danger."),
        .pokemonScale = 265,
        .pokemonOffset = 2,
        .trainerScale = 262,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Luxray,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 2,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(0, 15),
            ANIMCMD_FRAME(1, 20),
            ANIMCMD_FRAME(0, 15),
            ANIMCMD_FRAME(1, 20),
            ANIMCMD_FRAME(0, 15),
        ),
        .frontAnimId = ANIM_GLOW_YELLOW,
        .backPic = gMonBackPic_Luxray,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 0,
        .backAnimId = BACK_ANIM_SHRINK_GROW_VIBRATE,
        .palette = sGleThunderlordLuxrayPalette,
        .shinyPalette = sGleThunderlordLuxrayShinyPalette,
        .iconSprite = gMonIcon_Luxray,
        .iconPalIndex = 0,
#if P_GENDER_DIFFERENCES
        .frontPicFemale = gMonFrontPic_LuxrayF,
        .frontPicSizeFemale = MON_COORDS_SIZE(64, 64),
        .backPicFemale = gMonBackPic_LuxrayF,
        .backPicSizeFemale = MON_COORDS_SIZE(64, 64),
#endif //P_GENDER_DIFFERENCES
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-1, 10, SHADOW_SIZE_L)
        FOOTPRINT(Luxray)
        OVERWORLD(
            sPicTable_Luxray,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Luxray,
            gShinyOverworldPalette_Luxray
        )
        OVERWORLD_FEMALE(
            sPicTable_LuxrayF,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following
        )
        .levelUpLearnset = sGleThunderlordLuxrayMoves,
        .teachableLearnset = sLuxrayTeachableLearnset,
    },

[SPECIES_GLE_ETERNAL_HERO_PALAFIN] =
    {
        .baseHP        = 100,
        .baseAttack    = 160,
        .baseDefense   = 97,
        .baseSpeed     = 100,
        .baseSpAttack  = 106,
        .baseSpDefense = 87,
        .types = MON_TYPES(TYPE_WATER),
        .catchRate = 45,
        .expYield = 228,
        .evYield_HP = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 40,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD, EGG_GROUP_WATER_2),
        .abilities = { ABILITY_WATER_VEIL, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Etr.Palafin"),
        .cryId = CRY_PALAFIN_HERO,
        .natDexNum = NATIONAL_DEX_PALAFIN,
        .categoryName = _("Hero"),
        .height = 18,
        .weight = 974,
        .description = COMPOUND_STRING(
            "This Pokémon's ancient genes have\n"
            "awakened. It is now so extraordinarily\n"
            "strong that it can easily lift a\n"
            "cruise ship with one fin."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_PalafinHero,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        /*.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,*/
        .backPic = gMonBackPic_PalafinHero,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 1,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleEternalHeroPalafinPalette,
        .shinyPalette = sGleEternalHeroPalafinShinyPalette,
        .iconSprite = gMonIcon_PalafinHero,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(1, 13, SHADOW_SIZE_M)
        FOOTPRINT(Palafin)
        OVERWORLD(
            sPicTable_PalafinHero,
            SIZE_32x32,
            SHADOW_SIZE_S,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_PalafinHero,
            gShinyOverworldPalette_PalafinHero
        )
        .levelUpLearnset = sGleEternalHeroPalafinMoves,
        .teachableLearnset = sPalafinTeachableLearnset,

    },

[SPECIES_GLE_ABYSSAL_DONDOZO] =
    {
        .baseHP        = 150,
        .baseAttack    = 100,
        .baseDefense   = 115,
        .baseSpeed     = 35,
        .baseSpAttack  = 65,
        .baseSpDefense = 65,
        .types = MON_TYPES(TYPE_WATER),
        .catchRate = 25,
        .expYield = 265,
        .evYield_HP = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 40,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_2),
        .abilities = { ABILITY_REGENERATOR, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Aby.Dondozo"),
        .cryId = CRY_DONDOZO,
        .natDexNum = NATIONAL_DEX_DONDOZO,
        .categoryName = _("Big Catfish"),
        .height = 120,
        .weight = 2200,
        .description = COMPOUND_STRING(
            "It treats Tatsugiri like its boss\n"
            "and follows it loyally. Though\n"
            "powerful, Dondozo is apparently not very\n"
            "smart."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Dondozo,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .enemyMonElevation = 1,
        .backPic = gMonBackPic_Dondozo,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 11,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleAbyssalDondozoPalette,
        .shinyPalette = sGleAbyssalDondozoShinyPalette,
        .iconSprite = gMonIcon_Dondozo,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-1, 8, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Dondozo)
        OVERWORLD(
            sPicTable_Dondozo,
            SIZE_64x64,
            SHADOW_SIZE_L,
            TRACKS_NONE,
            sAnimTable_Following,
            gOverworldPalette_Dondozo,
            gShinyOverworldPalette_Dondozo
        )
        .levelUpLearnset = sGleAbyssalDondozoMoves,
        .teachableLearnset = sDondozoTeachableLearnset,
        .eggMoveLearnset = sDondozoEggMoveLearnset,
    },

[SPECIES_GLE_REVENANT_ANNIHILAPE] =
    {
        .baseHP        = 120,
        .baseAttack    = 125,
        .baseDefense   = 105,
        .baseSpeed     = 105,
        .baseSpAttack  = 50,
        .baseSpDefense = 110,
        .types = MON_TYPES(TYPE_FIGHTING, TYPE_GHOST),
        .catchRate = 45,
        .expYield = 268,
        .evYield_Attack = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FIELD),
        .abilities = { ABILITY_STAMINA, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_GRAY,
        .noFlip = TRUE,
        .speciesName = _("Rev.Annhlap"),
        .cryId = CRY_ANNIHILAPE,
        .natDexNum = NATIONAL_DEX_ANNIHILAPE,
        .categoryName = _("Rage Monkey"),
        .height = 12,
        .weight = 560,
        .description = COMPOUND_STRING(
            "When its anger rose beyond a\n"
            "critical point, this Pokémon gained power\n"
            "that is unfettered by the limits of\n"
            "its physical body."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Annihilape,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 0,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_Annihilape,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 1,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleRevenantAnnihilapePalette,
        .shinyPalette = sGleRevenantAnnihilapeShinyPalette,
        .iconSprite = gMonIcon_Annihilape,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 13, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Annihilape)
        OVERWORLD(
            sPicTable_Annihilape,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Annihilape,
            gShinyOverworldPalette_Annihilape
        )
        .levelUpLearnset = sGleRevenantAnnihilapeMoves,
        .teachableLearnset = sAnnihilapeTeachableLearnset,
    },

[SPECIES_GLE_ROYAL_CORVIKNIGHT] =
    {
        .baseHP        = 110,
        .baseAttack    = 115,
        .baseDefense   = 135,
        .baseSpeed     = 82,
        .baseSpAttack  = 53,
        .baseSpDefense = 105,
        .types = MON_TYPES(TYPE_FLYING, TYPE_STEEL),
        .catchRate = 45,
        .expYield = 248,
        .evYield_Defense = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING),
        .abilities = { ABILITY_ROCK_HEAD, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_PURPLE,
        .speciesName = _("RoyalCorvik"),
        .cryId = CRY_CORVIKNIGHT,
        .natDexNum = NATIONAL_DEX_CORVIKNIGHT,
        .categoryName = _("Raven"),
        .height = 22,
        .weight = 750,
        .description = COMPOUND_STRING(
            "This Pokémon reigns supreme in the skies\n"
            "of the Galar region. The black luster of\n"
            "its steel body could drive terror into\n"
            "the heart of any foe."),
        .pokemonScale = 256,
        .pokemonOffset = 0,
        .trainerScale = 348,
        .trainerOffset = 6,
        .frontPic = gMonFrontPic_Corviknight,
        .frontPicSize = MON_COORDS_SIZE(56, 64),
        .frontPicYOffset = 2,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(0, 1),
            ANIMCMD_FRAME(1, 50),
            ANIMCMD_FRAME(0, 1),
        ),
        .frontAnimId = ANIM_H_STRETCH_FAR_SLOW,
        .backPic = gMonBackPic_Corviknight,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 3,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleRoyalCorviknightPalette,
        .shinyPalette = sGleRoyalCorviknightShinyPalette,
        .iconSprite = gMonIcon_Corviknight,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-1, 9, SHADOW_SIZE_L)
        FOOTPRINT(Corviknight)
        OVERWORLD(
            sPicTable_Corviknight,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Corviknight,
            gShinyOverworldPalette_Corviknight
        )
        .levelUpLearnset = sGleRoyalCorviknightMoves,
        .teachableLearnset = sCorviknightTeachableLearnset,

    },

[SPECIES_GLE_STORMBORN_KILOWATTREL] =
    {
        .baseHP        = 75,
        .baseAttack    = 70,
        .baseDefense   = 65,
        .baseSpeed     = 160,
        .baseSpAttack  = 155,
        .baseSpDefense = 65,
        .types = MON_TYPES(TYPE_ELECTRIC, TYPE_FLYING),
        .catchRate = 90,
        .expYield = 172,
        .evYield_Speed = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1, EGG_GROUP_FLYING),
        .abilities = { ABILITY_DRIZZLE, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_YELLOW,
        .speciesName = _("StrmKilowat"),
        .cryId = CRY_KILOWATTREL,
        .natDexNum = NATIONAL_DEX_KILOWATTREL,
        .categoryName = _("Frigatebird"),
        .height = 14,
        .weight = 386,
        .description = COMPOUND_STRING(
            "It uses its throat sac to store\n"
            "electricity generated by its wings.\n"
            "There's hardly any oil in its feathers,\n"
            "so it is a poor swimmer."),
        .pokemonScale = 356,
        .pokemonOffset = 17,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Kilowattrel,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 6,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_Kilowattrel,
        .backPicSize = MON_COORDS_SIZE(64, 64),
        .backPicYOffset = 7,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleStormbornKilowattrelPalette,
        .shinyPalette = sGleStormbornKilowattrelShinyPalette,
        .iconSprite = gMonIcon_Kilowattrel,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-6, 6, SHADOW_SIZE_M)
        FOOTPRINT(Kilowattrel)
        OVERWORLD(
            sPicTable_Kilowattrel,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_NONE,
            sAnimTable_Following,
            gOverworldPalette_Kilowattrel,
            gShinyOverworldPalette_Kilowattrel
        )
        .levelUpLearnset = sGleStormbornKilowattrelMoves,
        .teachableLearnset = sKilowattrelTeachableLearnset,
    },

[SPECIES_GLE_PHOENIX_TALONFLAME] =
    {
        .baseHP        = 95,
        .baseAttack    = 125,
        .baseDefense   = 85,
        .baseSpeed     = 140,
        .baseSpAttack  = 90,
        .baseSpDefense = 85,
        .types = MON_TYPES(TYPE_FIRE, TYPE_FLYING),
        .catchRate = 45,
        .expYield = 175,
        .evYield_Speed = 3,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 15,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_FLYING),
        .abilities = { ABILITY_MAGIC_GUARD, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_RED,
        .speciesName = _("PhnxTalonfl"),
        .cryId = CRY_TALONFLAME,
        .natDexNum = NATIONAL_DEX_TALONFLAME,
        .categoryName = _("Scorching"),
        .height = 12,
        .weight = 245,
        .description = COMPOUND_STRING(
            "In the fever of an exciting battle, it\n"
            "showers embers from the gaps between its\n"
            "feathers and takes to the air. It finishes\n"
            "its prey off with a colossal kick."),
        .pokemonScale = 282,
        .pokemonOffset = 4,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Talonflame,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 1,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 55),
            ANIMCMD_FRAME(0, 5),
        ),
        .frontAnimId = ANIM_V_SLIDE_WOBBLE,
        .enemyMonElevation = 7,
        .backPic = gMonBackPic_Talonflame,
        .backPicSize = MON_COORDS_SIZE(64, 56),
        .backPicYOffset = 5,
        .backAnimId = BACK_ANIM_SHRINK_GROW_VIBRATE,
        .palette = sGlePhoenixTalonflamePalette,
        .shinyPalette = sGlePhoenixTalonflameShinyPalette,
        .iconSprite = gMonIcon_Talonflame,
        .iconPalIndex = 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-2, 17, SHADOW_SIZE_M)
        FOOTPRINT(Talonflame)
        OVERWORLD(
            sPicTable_Talonflame,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Talonflame,
            gShinyOverworldPalette_Talonflame
        )
        .levelUpLearnset = sGlePhoenixTalonflameMoves,
        .teachableLearnset = sTalonflameTeachableLearnset,
    },

[SPECIES_GLE_ABYSSAL_TOXAPEX] =
    {
        .baseHP        = 85,
        .baseAttack    = 63,
        .baseDefense   = 160,
        .baseSpeed     = 35,
        .baseSpAttack  = 53,
        .baseSpDefense = 155,
        .types = MON_TYPES(TYPE_POISON, TYPE_WATER),
        .catchRate = 75,
        .expYield = 173,
        .evYield_Defense = 2,
        .itemRare = ITEM_POISON_BARB,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_WATER_1),
        .abilities = { ABILITY_FUR_COAT, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLUE,
        .speciesName = _("Aby.Toxapex"),
        .cryId = CRY_TOXAPEX,
        .natDexNum = NATIONAL_DEX_TOXAPEX,
        .categoryName = _("Brutal Star"),
        .height = 7,
        .weight = 145,
        .description = COMPOUND_STRING(
            "Those attacked by Toxapex's poison will\n"
            "suffer intense pain for three days and\n"
            "three nights. Post-recovery, there will be\n"
            "some aftereffects."),
        .pokemonScale = 365,
        .pokemonOffset = 12,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Toxapex,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 2,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 30),
            ANIMCMD_FRAME(0, 1),
		),
        .frontAnimId = ANIM_SHAKE_GLOW_PURPLE_SLOW,
        .backPic = gMonBackPic_Toxapex,
        .backPicSize = MON_COORDS_SIZE(64, 56),
        .backPicYOffset = 8,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleAbyssalToxapexPalette,
        .shinyPalette = sGleAbyssalToxapexShinyPalette,
        .iconSprite = gMonIcon_Toxapex,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 6, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Toxapex)
        OVERWORLD(
            sPicTable_Toxapex,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Toxapex,
            gShinyOverworldPalette_Toxapex
        )
        .levelUpLearnset = sGleAbyssalToxapexMoves,
        .teachableLearnset = sToxapexTeachableLearnset,
    },

[SPECIES_GLE_OVERDRIVE_TOXTRICITY] =
    {
        .baseHP        = 85,
        .baseAttack    = 98,
        .baseDefense   = 80,
        .baseSpeed     = 115,
        .baseSpAttack  = 155,
        .baseSpDefense = 80,
        .types = MON_TYPES(TYPE_ELECTRIC, TYPE_POISON),
        .catchRate = 45,
        .expYield = 176,
        .evYield_SpAttack = 2,
        .genderRatio = PERCENT_FEMALE(50),
        .eggCycles = 25,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_HUMAN_LIKE),
        .abilities = { ABILITY_PUNK_ROCK, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_PURPLE,
        .speciesName = _("Ovr.Toxtric"),
        .cryId = CRY_TOXTRICITY_AMPED,
        .natDexNum = NATIONAL_DEX_TOXTRICITY,
        .categoryName = _("Punk"),
        .height = 16,
        .weight = 400,
        .description = COMPOUND_STRING(
            "When this Pokémon sounds as if it's\n"
            "strumming a guitar, it's actually clawing\n"
            "at the protrusions on its chest to\n"
            "generate electricity."),
        .pokemonScale = 259,
        .pokemonOffset = 1,
        .trainerScale = 296,
        .trainerOffset = 1,
        .frontPic = gMonFrontPic_ToxtricityAmped,
        .frontPicSize = MON_COORDS_SIZE(48, 64),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_SingleFramePlaceHolder,
        //.frontAnimId = ANIM_V_SQUISH_AND_BOUNCE,
        .backPic = gMonBackPic_ToxtricityAmped,
        .backPicSize = MON_COORDS_SIZE(56, 64),
        .backPicYOffset = 0,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleOverdriveToxtricityPalette,
        .shinyPalette = sGleOverdriveToxtricityShinyPalette,
        .iconSprite = gMonIcon_ToxtricityAmped,
        .iconPalIndex = 2,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-6, 13, SHADOW_SIZE_M)
        FOOTPRINT(Toxtricity)
        OVERWORLD(
            sPicTable_ToxtricityAmped,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_ToxtricityAmped,
            gShinyOverworldPalette_ToxtricityAmped
        )
        .levelUpLearnset = sGleOverdriveToxtricityMoves,
        .teachableLearnset = sToxtricityAmpedTeachableLearnset,

    },

[SPECIES_GLE_VENOM_QUEEN_SALAZZLE] =
    {
        .baseHP        = 95,
        .baseAttack    = 64,
        .baseDefense   = 75,
        .baseSpeed     = 136,
        .baseSpAttack  = 135,
        .baseSpDefense = 95,
        .types = MON_TYPES(TYPE_POISON, TYPE_FIRE),
        .catchRate = 45,
        .expYield = 168,
        .evYield_Speed = 2,
        .itemRare = ITEM_SMOKE_BALL,
        .genderRatio = MON_FEMALE,
        .eggCycles = 20,
        .friendship = STANDARD_FRIENDSHIP,
        .growthRate = GROWTH_MEDIUM_FAST,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_MONSTER, EGG_GROUP_DRAGON),
        .abilities = { ABILITY_CORROSION, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_BLACK,
        .speciesName = _("VenomQueen"),
        .cryId = CRY_SALAZZLE,
        .natDexNum = NATIONAL_DEX_SALAZZLE,
        .categoryName = _("Toxic Lizard"),
        .height = 12,
        .weight = 222,
        .description = gSalazzlePokedexText,
        .pokemonScale = 282,
        .pokemonOffset = 4,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Salazzle,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 1,
        .frontAnimFrames = sAnims_Salazzle,
        .frontAnimId = ANIM_GROW_VIBRATE,
        .backPic = gMonBackPic_Salazzle,
        .backPicSize = MON_COORDS_SIZE(48, 64),
        .backPicYOffset = 3,
        //.backAnimId = BACK_ANIM_NONE,
        .palette = sGleVenomQueenSalazzlePalette,
        .shinyPalette = sGleVenomQueenSalazzleShinyPalette,
        .iconSprite = gMonIcon_Salazzle,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(-4, 9, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Salazzle)
        OVERWORLD(
            sPicTable_Salazzle,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Salazzle,
            gShinyOverworldPalette_Salazzle
        )
        .levelUpLearnset = sGleVenomQueenSalazzleMoves,
        .teachableLearnset = sSalazzleTeachableLearnset,

    },

[SPECIES_GLE_GUARDIAN_CELEBI] =
    {
        .baseHP        = 100,
        .baseAttack    = 100,
        .baseDefense   = 100,
        .baseSpeed     = 100,
        .baseSpAttack  = 100,
        .baseSpDefense = 100,
        .types = MON_TYPES(TYPE_PSYCHIC, TYPE_GRASS),
        .catchRate = 45,
    #if P_UPDATED_EXP_YIELDS >= GEN_8
        .expYield = 300,
    #elif P_UPDATED_EXP_YIELDS >= GEN_5
        .expYield = 270,
    #else
        .expYield = 64,
    #endif
        .evYield_HP = 3,
        .itemCommon = ITEM_LUM_BERRY,
        .itemRare = ITEM_LUM_BERRY,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 120,
        .friendship = 100,
        .growthRate = GROWTH_MEDIUM_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_NO_EGGS_DISCOVERED),
        .abilities = { ABILITY_WONDER_GUARD, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_GREEN,
        .speciesName = _("Gdn.Celebi"),
        .cryId = CRY_CELEBI,
        .natDexNum = NATIONAL_DEX_CELEBI,
        .categoryName = _("Time Travel"),
        .height = 6,
        .weight = 50,
        .description = COMPOUND_STRING(
            "This Pokémon came from the future by\n"
            "crossing over time. It is thought that so\n"
            "long as Celebi appears, a bright and\n"
            "shining future awaits us."),
        .pokemonScale = 393,
        .pokemonOffset = -10,
        .trainerScale = 256,
        .trainerOffset = 0,
        .frontPic = gMonFrontPic_Celebi,
        .frontPicSize = MON_COORDS_SIZE(40, 40),
        .frontPicYOffset = P_GBA_STYLE_SPECIES_GFX ? 14 : 12,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 30),
            ANIMCMD_FRAME(0, 30),
            ANIMCMD_FRAME(1, 30),
            ANIMCMD_FRAME(0, 30),
        ),
        .frontAnimId = P_GBA_STYLE_SPECIES_GFX ? ANIM_RISING_WOBBLE : ANIM_H_SLIDE_WOBBLE,
        .enemyMonElevation = 15,
        .backPic = gMonBackPic_Celebi,
        .backPicSize = P_GBA_STYLE_SPECIES_GFX ? MON_COORDS_SIZE(48, 48) : MON_COORDS_SIZE(48, 56),
        .backPicYOffset = P_GBA_STYLE_SPECIES_GFX ? 8 : 7,
        .backAnimId = BACK_ANIM_SHAKE_GLOW_GREEN,
        .palette = gMonPalette_Celebi,
        .shinyPalette = gMonShinyPalette_Celebi,
        .iconSprite = gMonIcon_Celebi,
        .iconPalIndex = 1,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(0, 14, SHADOW_SIZE_S)
        FOOTPRINT(Celebi)
        OVERWORLD(
            sPicTable_Celebi,
            SIZE_32x32,
            SHADOW_SIZE_M,
            TRACKS_NONE,
            sAnimTable_Following,
            gOverworldPalette_Celebi,
            gShinyOverworldPalette_Celebi
        )
        .isMythical = TRUE,
        .isFrontierBanned = TRUE,
        .perfectIVCount = LEGENDARY_PERFECT_IV_COUNT,
        .levelUpLearnset = sGleGuardianCelebiMoves,
        .teachableLearnset = sCelebiTeachableLearnset,
    },

[SPECIES_GLE_AWAKENED_REGIGIGAS] =
    {
        .baseHP        = 110,
        .baseAttack    = 160,
        .baseDefense   = 110,
        .baseSpeed     = 100,
        .baseSpAttack  = 80,
        .baseSpDefense = 110,
        .types = MON_TYPES(TYPE_NORMAL),
        .catchRate = 3,
    #if P_UPDATED_EXP_YIELDS >= GEN_8
        .expYield = 335,
    #elif P_UPDATED_EXP_YIELDS >= GEN_5
        .expYield = 302,
    #else
        .expYield = 220,
    #endif
        .evYield_Attack = 3,
        .genderRatio = MON_GENDERLESS,
        .eggCycles = 120,
        .friendship = 0,
        .growthRate = GROWTH_SLOW,
        .eggGroups = MON_EGG_GROUPS(EGG_GROUP_NO_EGGS_DISCOVERED),
        .abilities = { ABILITY_CLEAR_BODY, ABILITY_NONE, ABILITY_NONE },
        .bodyColor = BODY_COLOR_WHITE,
        .speciesName = _("Awk.Regigas"),
        .cryId = CRY_REGIGIGAS,
        .natDexNum = NATIONAL_DEX_REGIGIGAS,
        .categoryName = _("Colossal"),
        .height = 37,
        .weight = 4200,
        .description = COMPOUND_STRING(
            "There is an enduring legend that\n"
            "states this Pokémon shaped Regirock,\n"
            "Regice, and Registeel out of clay, ice,\n"
            "and magma."),
        .pokemonScale = 256,
        .pokemonOffset = 2,
        .trainerScale = 610,
        .trainerOffset = 17,
        .frontPic = gMonFrontPic_Regigigas,
        .frontPicSize = MON_COORDS_SIZE(64, 64),
        .frontPicYOffset = 4,
        .frontAnimFrames = ANIM_FRAMES(
            ANIMCMD_FRAME(1, 40),
            ANIMCMD_FRAME(0, 5),
        ),
        .frontAnimId = ANIM_V_SHAKE,
        .backPic = gMonBackPic_Regigigas,
        .backPicSize = MON_COORDS_SIZE(64, 40),
        .backPicYOffset = 13,
        .backAnimId = BACK_ANIM_V_SHAKE_LOW,
        .palette = gMonPalette_Regigigas,
        .shinyPalette = gMonShinyPalette_Regigigas,
        .iconSprite = gMonIcon_Regigigas,
        .iconPalIndex = 0,
        .pokemonJumpType = PKMN_JUMP_TYPE_NONE,
        SHADOW(3, 10, SHADOW_SIZE_XL_BATTLE_ONLY)
        FOOTPRINT(Regigigas)
        OVERWORLD(
            sPicTable_Regigigas,
            SIZE_64x64,
            SHADOW_SIZE_M,
            TRACKS_FOOT,
            sAnimTable_Following,
            gOverworldPalette_Regigigas,
            gShinyOverworldPalette_Regigigas
        )
        .isSubLegendary = TRUE,
        .perfectIVCount = LEGENDARY_PERFECT_IV_COUNT,
        .levelUpLearnset = sGleAwakenedRegigigasMoves,
        .teachableLearnset = sRegigigasTeachableLearnset,
    },
