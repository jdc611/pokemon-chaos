LavenderTown_VolunteerPokemonHouse_Frlg_MapScripts::
	.byte 0

LavenderTown_VolunteerPokemonHouse_EventScript_MrFuji::
	lock
	faceplayer
	goto_if_set FLAG_GOT_POKE_FLUTE, LavenderTown_VolunteerPokemonHouse_EventScript_AlreadyHavePokeFlute
	msgbox LavenderTown_VolunteerPokemonHouse_Text_IdLikeYouToHaveThis
	checkitemspace ITEM_POKE_FLUTE
	goto_if_eq VAR_RESULT, FALSE, LavenderTown_VolunteerPokemonHouse_EventScript_NoRoomForPokeFlute
	setflag FLAG_GOT_POKE_FLUTE
	giveitem_msg LavenderTown_VolunteerPokemonHouse_Text_ReceivedPokeFluteFromMrFuji, ITEM_POKE_FLUTE, 1, MUS_RG_OBTAIN_KEY_ITEM
	msgbox LavenderTown_VolunteerPokemonHouse_Text_ExplainPokeFlute
	release
	end

LavenderTown_VolunteerPokemonHouse_EventScript_AlreadyHavePokeFlute::
	msgbox LavenderTown_VolunteerPokemonHouse_Text_HasPokeFluteHelpedYou
	release
	end

LavenderTown_VolunteerPokemonHouse_EventScript_NoRoomForPokeFlute::
	msgbox LavenderTown_VolunteerPokemonHouse_Text_MustMakeRoomForThis
	release
	end

LavenderTown_VolunteerPokemonHouse_EventScript_LittleGirl::
	lock
	faceplayer
	famechecker FAMECHECKER_MRFUJI, 0
	msgbox LavenderTown_VolunteerPokemonHouse_Text_MrFujiLooksAfterOrphanedMons
	release
	end

LavenderTown_VolunteerPokemonHouse_EventScript_LittleBoy::
	msgbox LavenderTown_VolunteerPokemonHouse_Text_MonsNiceToHug, MSGBOX_NPC
	end

LavenderTown_VolunteerPokemonHouse_EventScript_Youngster::
	lock
	faceplayer
	goto_if_set FLAG_RESCUED_MR_FUJI, LavenderTown_VolunteerPokemonHouse_EventScript_YoungsterFujiBack
	msgbox LavenderTown_VolunteerPokemonHouse_Text_WhereDidMrFujiGo
	release
	end

LavenderTown_VolunteerPokemonHouse_EventScript_YoungsterFujiBack::
	msgbox LavenderTown_VolunteerPokemonHouse_Text_MrFujiWasPrayingForCubonesMother
	release
	end

LavenderTown_VolunteerPokemonHouse_EventScript_Nidorino::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_NIDORINO, CRY_MODE_NORMAL
	msgbox LavenderTown_VolunteerPokemonHouse_Text_Nidorino
	waitmoncry
	release
	end

LavenderTown_VolunteerPokemonHouse_EventScript_Psyduck::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_PSYDUCK, CRY_MODE_NORMAL
	msgbox LavenderTown_VolunteerPokemonHouse_Text_Psyduck
	waitmoncry
	release
	end

LavenderTown_VolunteerPokemonHouse_EventScript_PokemonFanMagazine::
	lockall
	famechecker FAMECHECKER_MRFUJI, 2
	msgbox LavenderTown_VolunteerPokemonHouse_Text_GrandPrizeDrawingClipped
	releaseall
	end

LavenderTown_VolunteerPokemonHouse_EventScript_Bookshelf::
	msgbox LavenderTown_VolunteerPokemonHouse_Text_PokemonMagazinesLineShelf, MSGBOX_SIGN
	end

LavenderTown_PokemonCenter_1F_Text_HearMrFujiNotFromAroundHere::
	.string "I recently moved to this town.\p"
	.string "I hear that MR. FUJI's not from\n"
	.string "these parts originally, either.$"

LavenderTown_VolunteerPokemonHouse_Text_WhereDidMrFujiGo::
	.string "That's odd, MR. FUJI isn't here.\n"
	.string "Where'd he go?$"

LavenderTown_VolunteerPokemonHouse_Text_MrFujiWasPrayingForCubonesMother::
	.string "MR. FUJI had been praying alone\n"
	.string "for CUBONE's mother.$"

LavenderTown_VolunteerPokemonHouse_Text_MrFujiLooksAfterOrphanedMons::
	.string "This is really MR. FUJI's house.\p"
	.string "He's really kind.\p"
	.string "He looks after abandoned and\n"
	.string "orphaned POKéMON.$"

LavenderTown_VolunteerPokemonHouse_Text_MonsNiceToHug::
	.string "It's so warm!\n"
	.string "POKéMON are so nice to hug.$"

LavenderTown_VolunteerPokemonHouse_Text_Nidorino::
	.string "NIDORINO: Gaoo!$"

LavenderTown_VolunteerPokemonHouse_Text_Psyduck::
	.string "PSYDUCK: Gwappa!$"

LavenderTown_VolunteerPokemonHouse_Text_IdLikeYouToHaveThis::
	.string "MR. FUJI: {PLAYER}…\p"
	.string "Your POKéDEX quest is one that\n"
	.string "requires strong dedication.\p"
	.string "Without deep love for POKéMON,\n"
	.string "your quest may fail.\p"
	.string "I'm not sure if this will help you,\n"
	.string "but I'd like you to have it.$"

LavenderTown_VolunteerPokemonHouse_Text_ReceivedPokeFluteFromMrFuji::
	.string "{PLAYER} received a POKé FLUTE\n"
	.string "from MR. FUJI.$"

LavenderTown_VolunteerPokemonHouse_Text_ExplainPokeFlute::
	.string "Upon hearing the POKé FLUTE,\n"
	.string "sleeping POKéMON will spring awake.\p"
	.string "Try using it on POKéMON that are\n"
	.string "sleeping obstacles.$"

LavenderTown_VolunteerPokemonHouse_Text_MustMakeRoomForThis::
	.string "You must make room for this!$"

LavenderTown_VolunteerPokemonHouse_Text_HasPokeFluteHelpedYou::
	.string "MR. FUJI: Has my POKé FLUTE\n"
	.string "helped you?$"

LavenderTown_VolunteerPokemonHouse_Text_GrandPrizeDrawingClipped::
	.string "POKéMON FAN MAGAZINE\n"
	.string "Monthly Grand Prize Drawing!\p"
	.string "The application form is…\p"
	.string "Gone! It's been clipped out.\n"
	.string "Someone must have applied already.$"

LavenderTown_VolunteerPokemonHouse_Text_PokemonMagazinesLineShelf::
	.string "POKéMON magazines line the shelf.\p"
	.string "POKéMON INSIDER…\p"
	.string "POKéMON FAN…$"


Chaos_EventScript_ArcanineSpecialist::
	lock
	faceplayer
	goto_if_eq VAR_CHAOS_ARCANITE_REWARD, 2, Chaos_EventScript_ArcanineAlreadyRewarded
	goto_if_eq VAR_CHAOS_ARCANITE_REWARD, 1, Chaos_EventScript_ArcanineClaim
	checktrainerflag TRAINER_CHAOS_ARCANINE_SPECIALIST
	goto_if TRUE, Chaos_EventScript_ArcanineWon
	msgbox Chaos_Text_ArcanineInvitation, MSGBOX_YESNO
	goto_if_eq VAR_RESULT, NO, Chaos_EventScript_ArcanineDeclined
	special ChoosePartyMon
	goto_if_ge VAR_0x8004, PARTY_SIZE, Chaos_EventScript_ArcanineDeclined
	special ChaosBeginArcanineChallenge
	goto_if_eq VAR_RESULT, FALSE, Chaos_EventScript_ArcanineInvalid
	trainerbattle_single TRAINER_CHAOS_ARCANINE_SPECIALIST, Chaos_Text_ArcanineBattleIntro, Chaos_Text_ArcanineDefeat, Chaos_EventScript_ArcanineWon, NO_MUSIC
	special ChaosRestoreArcanineChallengeParty
	goto Chaos_EventScript_ArcanineDeclined

Chaos_EventScript_ArcanineWon::
	special ChaosRestoreArcanineChallengeParty
	setvar VAR_CHAOS_ARCANITE_REWARD, 1
Chaos_EventScript_ArcanineClaim::
	msgbox Chaos_Text_ArcanineReward
	giveitem ITEM_ARCANITE
	goto_if_eq VAR_RESULT, FALSE, Chaos_EventScript_ArcanineBagFull
	setvar VAR_CHAOS_ARCANITE_REWARD, 2
	msgbox Chaos_Text_ArcanineRing
	release
	end

Chaos_EventScript_ArcanineBagFull::
	msgbox Chaos_Text_ArcanineMakeRoom
	release
	end

Chaos_EventScript_ArcanineAlreadyRewarded::
	msgbox Chaos_Text_ArcanineRing
	release
	end

Chaos_EventScript_ArcanineInvalid::
	msgbox Chaos_Text_ArcanineChooseLiving
	release
	end

Chaos_EventScript_ArcanineDeclined::
	msgbox Chaos_Text_ArcanineComeBack
	release
	end

Chaos_EventScript_ArcaninePartner::
	lock
	faceplayer
	playmoncry SPECIES_ARCANINE, CRY_MODE_NORMAL
	msgbox Chaos_Text_ArcanineCry
	waitmoncry
	release
	end

Chaos_Text_ArcanineInvitation::
	.string "These unusual stones have been\n"
	.string "turning up all over KANTO.\p"
	.string "My ARCANINE can Mega Evolve!\p"
	.string "Care for a one-on-one challenge?\n"
	.string "Choose one healthy POKéMON.\p"
	.string "Win, and I'll give you ARCANITE!$"
Chaos_Text_ArcanineBattleIntro::
	.string "One partner each. Let's see\n"
	.string "what your bond can do!$"
Chaos_Text_ArcanineDefeat::
	.string "Splendid! A well-earned victory!$"
Chaos_Text_ArcanineReward::
	.string "As promised, this is ARCANITE.\n"
	.string "You've earned it!$"
Chaos_Text_ArcanineRing::
	.string "Let ARCANINE hold ARCANITE.\p"
	.string "With a MEGA RING, it can Mega\n"
	.string "Evolve in battle!\p"
	.string "PROF. OAK may know more about\n"
	.string "those rings after SABRINA's test.$"
Chaos_Text_ArcanineMakeRoom::
	.string "Your prize is safe with me!\n"
	.string "Make room in your BAG and return.$"
Chaos_Text_ArcanineChooseLiving::
	.string "Choose a healthy POKéMON that\n"
	.string "has hatched from its EGG.$"
Chaos_Text_ArcanineComeBack::
	.string "Come back whenever you're ready!$"
Chaos_Text_ArcanineCry::
	.string "ARCANINE: Arooo!$"
