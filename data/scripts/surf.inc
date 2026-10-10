EventScript_UseSurf::
	lockall
	@ Chaos QoL: A on fishable water gives one compact action menu.
	multichoice 0, 0, MULTI_SURF_FISH_BACK, FALSE
	switch VAR_RESULT
	case 0, EventScript_UseSurfSelected
	case 1, EventScript_UseFishSelected
	goto EventScript_ReleaseUseSurf

EventScript_UseSurfSelected::
	checkitem ITEM_HM_SURF
	goto_if_eq VAR_RESULT, FALSE, EventScript_SurfToolMissing
	checkfieldmove FIELD_MOVE_SURF, TRUE
	goto_if_eq VAR_RESULT, PARTY_SIZE, EventScript_SurfBadgeMissing
	setfieldeffectargument 0, PARTY_SIZE
	setfieldeffectargument 1, ITEM_HM_SURF
	msgbox gText_PlayerUsedSurf, MSGBOX_DEFAULT
	dofieldeffect FLDEFF_USE_SURF
	goto EventScript_ReleaseUseSurf

EventScript_SurfToolMissing::
	msgbox Text_SurfToolMissing, MSGBOX_DEFAULT
	goto EventScript_ReleaseUseSurf

EventScript_SurfBadgeMissing::
	msgbox Text_SurfBadgeMissing, MSGBOX_DEFAULT
	goto EventScript_ReleaseUseSurf

EventScript_UseFishSelected::
	@ Let the player explicitly choose an owned rod.
	multichoice 0, 0, MULTI_FISHING_RODS, FALSE
	switch VAR_RESULT
	case 0, EventScript_FishOldRod
	case 1, EventScript_FishGoodRod
	case 2, EventScript_FishSuperRod
	goto EventScript_ReleaseUseSurf

EventScript_FishOldRod::
	checkitem ITEM_OLD_ROD
	goto_if_eq VAR_RESULT, FALSE, EventScript_ChaosMissingRod
	closemessage
	releaseall
	callnative StartFishingOldRodFromWaterMenu
	end

EventScript_FishGoodRod::
	checkitem ITEM_GOOD_ROD
	goto_if_eq VAR_RESULT, FALSE, EventScript_ChaosMissingRod
	closemessage
	releaseall
	callnative StartFishingGoodRodFromWaterMenu
	end

EventScript_FishSuperRod::
	checkitem ITEM_SUPER_ROD
	goto_if_eq VAR_RESULT, FALSE, EventScript_ChaosMissingRod
	closemessage
	releaseall
	callnative StartFishingSuperRodFromWaterMenu
	end

EventScript_ReleaseUseSurf::
	releaseall
EventScript_EndUseSurf::
	end

EventScript_CurrentTooFast::
	lockall
	checkfieldmove FIELD_MOVE_SURF
	goto_if_eq VAR_RESULT, FALSE, EventScript_CurrentTooFastEndMsg
	playse SE_SELECT
	msgbox Text_CurrentTooFast
EventScript_CurrentTooFastEndMsg::
	releaseall
	end

EventScript_ChaosMissingRod::
 checkitem ITEM_OLD_ROD
 goto_if_eq VAR_RESULT, TRUE, EventScript_ChaosBetterRod
 checkitem ITEM_GOOD_ROD
 goto_if_eq VAR_RESULT, TRUE, EventScript_ChaosBetterRod
 checkitem ITEM_SUPER_ROD
 goto_if_eq VAR_RESULT, TRUE, EventScript_ChaosBetterRod
 msgbox Text_ChaosNoRod
 goto EventScript_ReleaseUseSurf
EventScript_ChaosBetterRod::
 msgbox Text_ChaosBetterRod
 goto EventScript_ReleaseUseSurf
Text_ChaosNoRod::
 .string "You don't have a fishing rod yet!$"
Text_ChaosBetterRod::
 .string "You need a better fishing rod\n"
 .string "to fish here.$"

EventScript_ChaosTrainToCap::
 lockall
 callnative ChaosBufferLevelCap
 msgbox Text_ChaosCurrentCap
 closemessage
 multichoice 0, 0, MULTI_CHAOS_TRAIN_TO_CAP, FALSE
 goto_if_eq VAR_RESULT, 0, EventScript_ChaosTrainWhole
 goto_if_eq VAR_RESULT, 1, EventScript_ChaosTrainOne
 releaseall
 end
EventScript_ChaosTrainWhole::
 callnative ChaosTrainWholeParty
 waitstate
 releaseall
 end
EventScript_ChaosTrainOne::
 callnative ChooseMonForTrainToCap
 waitstate
 releaseall
 end
Text_ChaosCurrentCap::
 .string "Current level cap: {STR_VAR_1}\p"
 .string "Train freely, then use MOVE\n"
 .string "RELEARNER to choose your moves.$"

EventScript_ChaosRecordsNurse::
 lock
 faceplayer
 msgbox Text_ChaosRecordsWelcome, MSGBOX_YESNO
 goto_if_eq VAR_RESULT, NO, EventScript_ChaosRecordsCare
 setvar VAR_0x8005, 0
 checkflag FLAG_SYS_GAME_CLEAR
 goto_if FALSE, EventScript_ChaosRecordsCurrent
 msgbox Text_ChaosRecordsLeague, MSGBOX_YESNO
 goto_if_eq VAR_RESULT, NO, EventScript_ChaosRecordsCurrent
 setvar VAR_0x8005, 1
EventScript_ChaosRecordsCurrent::
 callnative ChaosOpenRunRecords
 waitstate
EventScript_ChaosRecordsCare::
 callnative ChaosClaimCarePackage
 goto_if_eq VAR_RESULT, 1, EventScript_ChaosCareReceived
 goto_if_eq VAR_RESULT, 2, EventScript_ChaosCareBagFull
 release
 end
EventScript_ChaosCareReceived::
 playfanfare MUS_OBTAIN_ITEM
 msgbox Text_ChaosCareReceived
 waitfanfare
 release
 end
EventScript_ChaosCareBagFull::
 msgbox Text_ChaosCareBagFull
 release
 end
Text_ChaosRecordsWelcome::
 .string "I keep your RUN RECORDS!\nWould you like to see them?$"
Text_ChaosRecordsLeague::
 .string "Open your completed LEAGUE record?\nNO shows your current run.$"
Text_ChaosCareReceived::
 .string "A CHAOS CARE PACKAGE for you!\nBalls, berries, and useful supplies.\p"
 .string "This bundle is yours to keep.\nCome back as your journey grows!$"
Text_ChaosCareBagFull::
 .string "Please make room in your BAG.\nI'll hold your CARE PACKAGE here.$"

EventScript_ChaosClaimGymStone::
	lock
	faceplayer
	callnative ChaosClaimGymStone
	goto_if_eq VAR_RESULT, 2, EventScript_ChaosGymStoneBagFull
	goto_if_ne VAR_RESULT, 1, EventScript_ChaosGymStoneDone
	playfanfare MUS_OBTAIN_ITEM
	msgbox Text_ChaosGymStoneReceived
	waitfanfare
EventScript_ChaosGymStoneDone::
	return
EventScript_ChaosGymStoneBagFull::
	msgbox Text_ChaosGymStoneBagFull
	return
Text_ChaosGymStoneReceived::
	.string "{PLAYER} received {STR_VAR_1}!\pA Mega Stone to honor your victory!$"
Text_ChaosGymStoneBagFull::
	.string "Your Bag has no room for my stone.\nCome back after making room!$"

EventScript_ChaosV2CareReceived::
 lockall
 playfanfare MUS_OBTAIN_ITEM
 msgbox Text_ChaosV2CareReceived
 waitfanfare
EventScript_ChaosV2CarePages::
 callnative ChaosCareNextPage
 goto_if_eq VAR_RESULT, FALSE, EventScript_ChaosV2CareDone
 msgbox gStringVar4
 goto EventScript_ChaosV2CarePages
EventScript_ChaosV2CareDone::
 releaseall
 end
EventScript_ChaosV2CareFull::
 lockall
 msgbox Text_ChaosV2CareFull
 releaseall
 end
Text_ChaosV2CareReceived::
 .string "CARE PACKAGE RECEIVED!$"
Text_ChaosV2CareFull::
 .string "Your CARE PACKAGE is waiting!\nPlease make room in your BAG.\p"
 .string "Return to this area to receive it.$"
ChaosMysteryEggVendor0::
 setvar VAR_0x8004, 0
 goto ChaosMysteryEggVendor
ChaosMysteryEggVendor1::
 setvar VAR_0x8004, 1
 goto ChaosMysteryEggVendor
ChaosMysteryEggVendor2::
 setvar VAR_0x8004, 2
 goto ChaosMysteryEggVendor
ChaosMysteryEggVendor::
	specialvar VAR_RESULT, IsIronmonRun
	goto_if_eq VAR_RESULT, TRUE, Ironmon_EventScript_ServiceBlocked
 lock
 faceplayer
 goto_if_eq VAR_0x8004, 0, ChaosMysteryEggVendorBaby
 goto_if_eq VAR_0x8004, 1, ChaosMysteryEggVendorStarter
 msgbox Text_ChaosMysteryEggRegional
 goto ChaosMysteryEggVendorOffer
ChaosMysteryEggVendorBaby::
 msgbox Text_ChaosMysteryEggBaby
 goto ChaosMysteryEggVendorOffer
ChaosMysteryEggVendorStarter::
 msgbox Text_ChaosMysteryEggStarter
ChaosMysteryEggVendorOffer::
 msgbox Text_ChaosMysteryEggOffer, MSGBOX_YESNO
 goto_if_eq VAR_RESULT, NO, ChaosMysteryEggVendorDone
 callnative ChaosBuyMysteryEgg
 goto_if_eq VAR_RESULT, 1, ChaosMysteryEggVendorMoney
 goto_if_eq VAR_RESULT, 2, ChaosMysteryEggVendorSpace
 goto_if_eq VAR_RESULT, 3, ChaosMysteryEggVendorSold
 playfanfare MUS_OBTAIN_ITEM
 msgbox Text_ChaosMysteryEggReceived
 waitfanfare
ChaosMysteryEggVendorDone::
 release
 end
ChaosMysteryEggVendorMoney::
 msgbox Text_ChaosMysteryEggMoney
 goto ChaosMysteryEggVendorDone
ChaosMysteryEggVendorSpace::
 msgbox Text_ChaosMysteryEggSpace
 goto ChaosMysteryEggVendorDone
ChaosMysteryEggVendorSold::
 msgbox Text_ChaosMysteryEggSold
 goto ChaosMysteryEggVendorDone
Text_ChaosMysteryEggBaby::
 .string "I raise BABY POKéMON EGGS!\nThey're chosen from these seven:\p"
 .string "TOGEPI, PICHU, AZURILL, RIOLU,\nBUDEW, CLEFFA, and IGGLYBUFF.\p"
 .string "Which one hatches is a surprise.\nA little partner with big potential!$"
Text_ChaosMysteryEggStarter::
 .string "I raise STARTER POKéMON EGGS!\nMy pool includes all 27 starters,\lfrom KANTO through PALDEA.\p"
 .string "A BULBASAUR? A SCORBUNNY?\nYou won't know until it hatches!$"
Text_ChaosMysteryEggRegional::
 .string "My EGGS hatch REGIONAL FORMS!\nEight special forms from ALOLA,\lGALAR, HISUI, and PALDEA.\p"
 .string "A HISUIAN ZORUA? A PALDEAN\nWOOPER? Each has its own style.\lThe exact one is a surprise!$"
Text_ChaosMysteryEggOffer::
 .string "Each MYSTERY EGG costs ¥5,000.\nOne EGG per trainer from my pool.\p"
 .string "Want to take one home?$"
Text_ChaosMysteryEggReceived::
 .string "You received a MYSTERY EGG!\nIf your party is full, check your PC.$"
Text_ChaosMysteryEggMoney::
 .string "Not quite enough money yet.\nCome back whenever you're ready!$"
Text_ChaosMysteryEggSpace::
 .string "Your party and PC need some room.\nI haven't charged you a thing!$"
Text_ChaosMysteryEggSold::
 .string "That's my one EGG for you!\nTake good care of the little one.$"
