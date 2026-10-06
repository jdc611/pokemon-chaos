CeladonCity_DepartmentStore_4F_Frlg_MapScripts::
	.byte 0

CeladonCity_DepartmentStore_4F_EventScript_Man::
	msgbox CeladonCity_DepartmentStore_4F_Text_GettingPokeDollAsPresent, MSGBOX_NPC
	end

CeladonCity_DepartmentStore_4F_EventScript_Youngster::
	msgbox CeladonCity_DepartmentStore_4F_Text_CanRunAwayWithPokeDoll, MSGBOX_NPC
	end

CeladonCity_DepartmentStore_4F_EventScript_FloorSign::
	msgbox CeladonCity_DepartmentStore_4F_Text_FloorSign, MSGBOX_SIGN
	end

CeladonCity_DepartmentStore_4F_EventScript_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart CeladonCity_DepartmentStore_4F_Items
	msgbox gText_PleaseComeAgain
	release
	end

	.align 2
CeladonCity_DepartmentStore_4F_Items::
	.2byte ITEM_POKE_DOLL
	.2byte ITEM_RETRO_MAIL
	.2byte ITEM_FIRE_STONE
	.2byte ITEM_THUNDER_STONE
	.2byte ITEM_WATER_STONE
	.2byte ITEM_LEAF_STONE
	.2byte ITEM_LINKING_CORD
	.2byte ITEM_SUN_STONE
	.2byte ITEM_MOON_STONE
	.2byte ITEM_SHINY_STONE
	.2byte ITEM_DUSK_STONE
	.2byte ITEM_DAWN_STONE
	.2byte ITEM_ICE_STONE
	.2byte ITEM_METAL_COAT
	.2byte ITEM_KINGS_ROCK
	.2byte ITEM_UPGRADE
	.2byte ITEM_DUBIOUS_DISC
	.2byte ITEM_RAZOR_CLAW
	.2byte ITEM_RAZOR_FANG
	.2byte ITEM_PROTECTOR
	.2byte ITEM_REAPER_CLOTH
	.2byte ITEM_PRISM_SCALE
	.2byte ITEM_ELECTIRIZER
	.2byte ITEM_MAGMARIZER
	.2byte ITEM_SACHET
	.2byte ITEM_WHIPPED_DREAM
	.2byte ITEM_ABILITY_CAPSULE
	.2byte ITEM_NONE
	release
	end

CeladonCity_DepartmentStore_4F_Text_GettingPokeDollAsPresent::
	.string "I'm getting a present for my\n"
	.string "girlfriend.\p"
	.string "I guess a POKé DOLL will be it.\n"
	.string "It's the popular thing.$"

CeladonCity_DepartmentStore_4F_Text_CanRunAwayWithPokeDoll::
	.string "I heard something useful.\p"
	.string "If a wild POKéMON appears, you can\n"
	.string "distract it with a POKé DOLL.\p"
	.string "You can run away while the wild\n"
	.string "POKéMON is distracted.$"

CeladonCity_DepartmentStore_4F_Text_FloorSign::
	.string "Express yourself with gifts!\n"
	.string "4F: WISE MAN GIFTS\p"
	.string "Evolution Special!\n"
	.string "Element STONES on sale now!$"

ChaosCeladonSpecialStock::
 lock
 faceplayer
 goto_if_unset FLAG_BADGE06_GET, ChaosCeladonSpecialStock_Locked
 msgbox ChaosCeladonSpecialStock_Show, MSGBOX_YESNO
 goto_if_eq VAR_RESULT, NO, ChaosCeladonSpecialStock_End
 msgbox ChaosCeladonSpecialStock_Welcome, MSGBOX_DEFAULT
 pokemart ChaosCeladonSpecialStock_Items
 msgbox gText_PleaseComeAgain
ChaosCeladonSpecialStock_End::
 release
 end
ChaosCeladonSpecialStock_Locked::
 msgbox ChaosCeladonSpecialStock_Badge, MSGBOX_DEFAULT
 goto ChaosCeladonSpecialStock_End
ChaosCeladonSpecialStock_Badge::
 .string "I keep our special stock for Trainers\nwith a real MARSH BADGE.\pBeat SABRINA, then come show me!$"
ChaosCeladonSpecialStock_Show::
 .string "Would you like to show me your\nMARSH BADGE and see our special stock?$"
ChaosCeladonSpecialStock_Welcome::
 .string "A MARSH BADGE! Nicely done!\pAbility Patches and selected Mega\nStones are now available to you.\pSome rare stones are earned elsewhere.\nWe don't sell those here.$"
 .align 2
ChaosCeladonSpecialStock_Items::
 .2byte ITEM_ABILITY_PATCH
 .2byte ITEM_CHARIZARDITE_X
 .2byte ITEM_CHARIZARDITE_Y
 .2byte ITEM_BLASTOISINITE
 .2byte ITEM_RAICHUNITE_X
 .2byte ITEM_RAICHUNITE_Y
 .2byte ITEM_CLEFABLITE
 .2byte ITEM_VICTREEBELITE
 .2byte ITEM_SLOWBRONITE
 .2byte ITEM_GENGARITE
 .2byte ITEM_KANGASKHANITE
 .2byte ITEM_STARMINITE
 .2byte ITEM_SCIZORITE
 .2byte ITEM_PINSIRITE
 .2byte ITEM_AERODACTYLITE
 .2byte ITEM_DRAGONINITE
 .2byte ITEM_MEWTWONITE_X
 .2byte ITEM_MEWTWONITE_Y
 .2byte ITEM_MEGANIUMITE
 .2byte ITEM_FERALIGITE
 .2byte ITEM_AMPHAROSITE
 .2byte ITEM_HERACRONITE
 .2byte ITEM_SKARMORITE
 .2byte ITEM_TYRANITARITE
 .2byte ITEM_SCEPTILITE
 .2byte ITEM_BLAZIKENITE
 .2byte ITEM_SWAMPERTITE
 .2byte ITEM_GARDEVOIRITE
 .2byte ITEM_GALLADITE
 .2byte ITEM_SABLENITE
 .2byte ITEM_MAWILITE
 .2byte ITEM_AGGRONITE
 .2byte ITEM_MEDICHAMITE
 .2byte ITEM_SHARPEDONITE
 .2byte ITEM_CAMERUPTITE
 .2byte ITEM_ALTARIANITE
 .2byte ITEM_BANETTITE
 .2byte ITEM_CHIMECHITE
 .2byte ITEM_ABSOLITE
 .2byte ITEM_ABSOLITE_Z
 .2byte ITEM_GLALITITE
 .2byte ITEM_FROSLASSITE
 .2byte ITEM_SALAMENCITE
 .2byte ITEM_METAGROSSITE
 .2byte ITEM_LATIASITE
 .2byte ITEM_LATIOSITE
 .2byte ITEM_STARAPTITE
 .2byte ITEM_LOPUNNITE
 .2byte ITEM_GARCHOMPITE_Z
 .2byte ITEM_LUCARIONITE
 .2byte ITEM_LUCARIONITE_Z
 .2byte ITEM_ABOMASITE
 .2byte ITEM_HEATRANITE
 .2byte ITEM_DARKRANITE
 .2byte ITEM_EMBOARITE
 .2byte ITEM_EXCADRITE
 .2byte ITEM_AUDINITE
 .2byte ITEM_SCOLIPITE
 .2byte ITEM_SCRAFTINITE
 .2byte ITEM_EELEKTROSSITE
 .2byte ITEM_CHANDELURITE
 .2byte ITEM_GOLURKITE
 .2byte ITEM_CHESNAUGHTITE
 .2byte ITEM_DELPHOXITE
 .2byte ITEM_GRENINJITE
 .2byte ITEM_PYROARITE
 .2byte ITEM_FLOETTITE
 .2byte ITEM_MEOWSTICITE
 .2byte ITEM_MALAMARITE
 .2byte ITEM_BARBARACITE
 .2byte ITEM_DRAGALGITE
 .2byte ITEM_HAWLUCHANITE
 .2byte ITEM_ZYGARDITE
 .2byte ITEM_DIANCITE
 .2byte ITEM_CRABOMINITE
 .2byte ITEM_GOLISOPITE
 .2byte ITEM_DRAMPANITE
 .2byte ITEM_MAGEARNITE
 .2byte ITEM_ZERAORITE
 .2byte ITEM_FALINKSITE
 .2byte ITEM_SCOVILLAINITE
 .2byte ITEM_GLIMMORANITE
 .2byte ITEM_TATSUGIRINITE
 .2byte ITEM_BAXCALIBRITE
 .2byte ITEM_NONE
