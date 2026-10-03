SaffronCity_Frlg_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, SaffronCity_OnTransition
	.byte 0

SaffronCity_OnTransition::
	setworldmapflag FLAG_WORLD_MAP_SAFFRON_CITY
	call_if_set FLAG_BADGE06_GET, EventScript_ChaosCheckOakRewardReady
	setflag FLAG_TEMP_3
	call_if_eq VAR_CHAOS_OAK_MEGA_REWARD, 1, EventScript_ChaosShowSaffronOak
	call_if_unset FLAG_RESCUED_MR_FUJI, SaffronCity_EventScript_MoveDoorGuardGrunt
	end

SaffronCity_EventScript_MoveDoorGuardGrunt::
	setobjectxyperm LOCALID_SAFFRON_DOOR_GUARD_GRUNT, 33, 31
	return

SaffronCity_EventScript_RocketGrunt1::
	msgbox SaffronCity_Text_WhatDoYouWantGetLost, MSGBOX_NPC
	end

SaffronCity_EventScript_RocketGrunt2::
	msgbox SaffronCity_Text_BossTakeTownForTeamRocket, MSGBOX_NPC
	end

SaffronCity_EventScript_RocketGrunt3::
	msgbox SaffronCity_Text_DontGetDefiantOrIllHurtYou, MSGBOX_NPC
	end

SaffronCity_EventScript_RocketGrunt4::
	msgbox SaffronCity_Text_SaffronBelongsToTeamRocket, MSGBOX_NPC
	end

SaffronCity_EventScript_RocketGrunt5::
	msgbox SaffronCity_Text_CriminalLifeMakesMeFeelAlive, MSGBOX_NPC
	end

SaffronCity_EventScript_RocketGrunt6::
	msgbox SaffronCity_Text_WatchWhereYoureWalking, MSGBOX_NPC
	end

SaffronCity_EventScript_RocketGrunt7::
	msgbox SaffronCity_Text_WeCanExploitMonsAroundWorld, MSGBOX_NPC
	end

SaffronCity_EventScript_DoorGuardGrunt::
	lock
	goto_if_set FLAG_RESCUED_MR_FUJI, SaffronCity_EventScript_DoorGuardAsleep
	applymovement LOCALID_SAFFRON_DOOR_GUARD_GRUNT, Common_Movement_FacePlayer
	waitmovement 0
	msgbox SaffronCity_Text_ImASecurityGuard
	release
	end

SaffronCity_EventScript_DoorGuardAsleep::
	textcolor NPC_TEXT_COLOR_NEUTRAL
	msgbox SaffronCity_Text_HesTakingASnooze
	release
	end

SaffronCity_EventScript_WorkerM::
	msgbox SaffronCity_Text_YouBeatTeamRocket, MSGBOX_NPC
	end

SaffronCity_EventScript_Youngster::
	msgbox SaffronCity_Text_SafeToGoOutAgain, MSGBOX_NPC
	end

SaffronCity_EventScript_Lass::
	msgbox SaffronCity_Text_PeopleComingBackToSaffron, MSGBOX_NPC
	end

SaffronCity_EventScript_Boy::
	msgbox SaffronCity_Text_FlewHereOnPidgeot, MSGBOX_NPC
	end

SaffronCity_EventScript_Pidgeot::
	lock
	faceplayer
	waitse
	playmoncry SPECIES_PIDGEOT, CRY_MODE_NORMAL
	msgbox SaffronCity_Text_Pidgeot
	waitmoncry
	release
	end

SaffronCity_EventScript_Man::
	msgbox SaffronCity_Text_SawRocketBossEscaping, MSGBOX_NPC
	end

SaffronCity_EventScript_CitySign::
	msgbox SaffronCity_Text_CitySign, MSGBOX_SIGN
	end

SaffronCity_EventScript_DojoSign::
	msgbox SaffronCity_Text_FightingDojo, MSGBOX_SIGN
	end

SaffronCity_EventScript_GymSign::
	lockall
	famechecker FAMECHECKER_SABRINA, 0
	msgbox SaffronCity_Text_GymSign
	releaseall
	end

SaffronCity_EventScript_TrainerTips1::
	msgbox SaffronCity_Text_FullHealCuresStatus, MSGBOX_SIGN
	end

SaffronCity_EventScript_TrainerTips2::
	msgbox SaffronCity_Text_GreatBallImprovedCatchRate, MSGBOX_SIGN
	end

SaffronCity_EventScript_SilphCoSign::
	msgbox SaffronCity_Text_SilphCoSign, MSGBOX_SIGN
	end

SaffronCity_EventScript_MrPsychicsHouseSign::
	msgbox SaffronCity_Text_MrPsychicsHouse, MSGBOX_SIGN
	end

SaffronCity_EventScript_SilphProductSign::
	msgbox SaffronCity_Text_SilphsLatestProduct, MSGBOX_SIGN
	end

SaffronCity_EventScript_TrainerFanClubSign::
	msgbox SaffronCity_Text_TrainerFanClubSign, MSGBOX_SIGN
	end

SaffronCity_Text_WhatDoYouWantGetLost::
	.string "What do you want?\n"
	.string "Get lost!$"

SaffronCity_Text_BossTakeTownForTeamRocket::
	.string "BOSS said he'll take this town\n"
	.string "in the name of TEAM ROCKET!$"

SaffronCity_Text_DontGetDefiantOrIllHurtYou::
	.string "Don't get defiant!\n"
	.string "Or I'll have to hurt you!$"

SaffronCity_Text_SaffronBelongsToTeamRocket::
	.string "SAFFRON belongs to TEAM ROCKET!$"

SaffronCity_Text_CriminalLifeMakesMeFeelAlive::
	.string "My life as a criminal makes me feel\n"
	.string "so alive!$"

SaffronCity_Text_WatchWhereYoureWalking::
	.string "Ow!\n"
	.string "Watch where you're walking!$"

SaffronCity_Text_WeCanExploitMonsAroundWorld::
	.string "With SILPH under control, we can\n"
	.string "exploit POKéMON around the world!\p"
	.string "We'll get stinking rich, yeahah!$"

SaffronCity_Text_YouBeatTeamRocket::
	.string "You beat TEAM ROCKET all alone?\n"
	.string "That's amazing!$"

SaffronCity_Text_SafeToGoOutAgain::
	.string "Yeah! TEAM ROCKET is gone!\n"
	.string "It's safe to go out again!$"

SaffronCity_Text_PeopleComingBackToSaffron::
	.string "People fled from here in droves\n"
	.string "when those ROCKETS came.\p"
	.string "They should be flocking back to\n"
	.string "SAFFRON now.$"

SaffronCity_Text_FlewHereOnPidgeot::
	.string "I flew here on my PIDGEOT when\n"
	.string "I read about SILPH.\p"
	.string "It's already over?\n"
	.string "I missed the media action…$"

SaffronCity_Text_Pidgeot::
	.string "PIDGEOT: Bi bibii!$"

SaffronCity_Text_SawRocketBossEscaping::
	.string "I saw the ROCKET BOSS escaping\n"
	.string "SILPH's building.$"

SaffronCity_Text_ImASecurityGuard::
	.string "I'm a security guard.\p"
	.string "Suspicious kids I don't allow in!$"

SaffronCity_Text_HesTakingASnooze::
	.string "…\n"
	.string "Snore…\p"
	.string "Hah! He's taking a snooze!$"

SaffronCity_Text_CitySign::
	.string "SAFFRON CITY\n"
	.string "Shining, Golden Land of Commerce$"

SaffronCity_Text_FightingDojo::
	.string "FIGHTING DOJO$"

SaffronCity_Text_GymSign::
	.string "SAFFRON CITY POKéMON GYM\n"
	.string "LEADER: SABRINA\l"
	.string "The Master of PSYCHIC POKéMON!$"

SaffronCity_Text_FullHealCuresStatus::
	.string "TRAINER TIPS\p"
	.string "FULL HEAL cures all ailments like\n"
	.string "burns, paralysis, poisoning,\l"
	.string "freezing, and sleep.\p"
	.string "It costs a bit more, but it's more\n"
	.string "convenient than buying other items.$"

SaffronCity_Text_GreatBallImprovedCatchRate::
	.string "TRAINER TIPS\p"
	.string "The new GREAT BALL offers improved\n"
	.string "catching rates.\p"
	.string "Try it on those hard-to-catch\n"
	.string "POKéMON.$"

SaffronCity_Text_SilphCoSign::
	.string "SILPH CO. OFFICE BUILDING$"

SaffronCity_Text_MrPsychicsHouse::
	.string "MR. PSYCHIC'S HOUSE$"

SaffronCity_Text_SilphsLatestProduct::
	.string "SILPH's latest product!\n"
	.string "Release to be determined…$"

SaffronCity_Text_TrainerFanClubSign::
	.string "POKéMON TRAINER FAN CLUB\p"
	.string "Many TRAINERS have scribbled their\n"
	.string "names on this sign.$"

SaffronCity_Text_HowCanClubNotRecognizeLance::
	.string "This FAN CLUB…\n"
	.string "No one here has a clue!\p"
	.string "How could they not recognize\n"
	.string "the brilliance that is LANCE?\p"
	.string "He stands for justice!\n"
	.string "He's cool, and yet passionate!\l"
	.string "He's the greatest, LANCE!$"


EventScript_ChaosShowSaffronOak::
	clearflag FLAG_TEMP_3
	return

EventScript_ChaosOakMegaReward::
	lockall
	goto_if_ne VAR_CHAOS_OAK_MEGA_REWARD, 1, EventScript_ChaosOakRewardEnd
	applymovement LOCALID_PLAYER, Common_Movement_FaceLeft
	waitmovement 0
	msgbox Text_ChaosOakWatchingGrowth
	checkitem ITEM_MEGA_RING
	call_if_eq VAR_RESULT, FALSE, EventScript_ChaosOakGiveMegaRing
	checkitem ITEM_MEGA_RING
	goto_if_eq VAR_RESULT, FALSE, EventScript_ChaosOakBagFull
	special ChaosChooseOakMegaStone
	call_if_eq VAR_RESULT, FALSE, EventScript_ChaosOakFallbackStone
	giveitem VAR_0x8004
	goto_if_eq VAR_RESULT, FALSE, EventScript_ChaosOakBagFull
	setvar VAR_CHAOS_OAK_MEGA_REWARD, 2
	msgbox Text_ChaosOakRematchChallenge
	removeobject LOCALID_CHAOS_SAFFRON_OAK
	setflag FLAG_TEMP_3
EventScript_ChaosOakRewardEnd::
	releaseall
	end

EventScript_ChaosOakGiveMegaRing::
	giveitem ITEM_MEGA_RING
	return

EventScript_ChaosOakFallbackStone::
	msgbox Text_ChaosOakGyaradosStone
	return

EventScript_ChaosOakBagFull::
	msgbox Text_ChaosOakMakeRoom
	releaseall
	end

Text_ChaosOakWatchingGrowth::
	.string "OAK: {PLAYER}! I've been hearing\n"
	.string "wonderful things about you!\p"
	.string "Your POKéMON grow with you.\n"
	.string "That bond is something special.\p"
	.string "This MEGA RING and MEGA STONE\n"
	.string "can help that bond shine!\p"
	.string "Give the stone to its POKéMON,\n"
	.string "then use Mega Evolution in battle.$"

Text_ChaosOakGyaradosStone::
	.string "GYARADOS can use this stone!\p"
	.string "The man on ROUTE 4 sells MAGIKARP.\n"
	.string "Raise one into a fine GYARADOS!$"

Text_ChaosOakRematchChallenge::
	.string "But I have one more test for you!\p"
	.string "BROCK, MISTY, and LT. SURGE are\n"
	.string "ready to challenge you again.\p"
	.string "Visit them in any order. Win all\n"
	.string "three, then take on KOGA!\p"
	.string "I know you and your POKéMON\n"
	.string "can rise to the challenge!$"

Text_ChaosOakMakeRoom::
	.string "Make room in your BAG, then\n"
	.string "come speak to me again.$"

Text_ChaosRematchIntro::
	.string "OAK told me about your progress!\p"
	.string "Let's see how far you've come!\n"
	.string "My new team is ready!$"
Text_ChaosRematchDefeat::
	.string "What a remarkable bond!\n"
	.string "You have grown into a fine Trainer!$"
Text_ChaosRematchWon::
	.string "You've passed my rematch test!\p"
	.string "When all three tests are complete,\n"
	.string "KOGA will be ready for you.$"
Text_ChaosRematchAlreadyWon::
	.string "You've already passed my test.\n"
	.string "Keep growing with your POKéMON!$"
Text_ChaosNeedSabrina::
	.string "The route to FUCHSIA is reserved\n"
	.string "for Trainers with the MARSHBADGE.\p"
	.string "Challenge SABRINA in SAFFRON\n"
	.string "CITY before continuing.$"

EventScript_ChaosFuchsiaGateLocked::
	msgbox Text_ChaosNeedSabrina
	release
	end

EventScript_ChaosCheckOakRewardReady::
	call_if_eq VAR_CHAOS_OAK_MEGA_REWARD, 0, EventScript_ChaosReadyOakMegaReward
	return
