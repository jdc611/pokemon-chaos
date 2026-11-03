GleTown_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, GleTown_OnLoad
	.byte 0
GleTown_OnLoad::
	special GleTownDraw
	end
GleTownPlot::
	lockall
	setvar VAR_0x8009, 255
	special GlePlotInfo
	compare VAR_RESULT, 2
	goto_if_eq GleTown_RivalPlot
	compare VAR_RESULT, 1
	goto_if_ne GleTown_PlotDone
	special GlePlotHabitatName
	msgbox GleTown_TextPlot, MSGBOX_DEFAULT
	closemessage
	special GlePlotMenu
	special GlePlotBuildInfo
	compare VAR_RESULT, 1
	goto_if_ne GleTown_PlotDone
	msgbox GleTown_TextBuildConfirm, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleTown_PlotDone
	special GlePlotNeedsHabitatType
	compare VAR_RESULT, 1
	goto_if_ne GleTown_PlotConstruct
	msgbox GleTown_TextHabitatType, MSGBOX_DEFAULT
	closemessage
	special GleHabitatTypeMenu
	compare VAR_RESULT, 18
	goto_if_ge GleTown_PlotDone
	copyvar VAR_0x8009, VAR_RESULT
GleTown_PlotConstruct::
	special GlePlotConstruct
	compare VAR_RESULT, 0
	goto_if_eq GleTown_PlotBuilt
	msgbox GleTown_TextBuildFailed, MSGBOX_DEFAULT
	goto GleTown_PlotDone
GleTown_PlotBuilt::
	special GlePlotApplyHabitatType
	playfanfare MUS_EVOLVED
	msgbox GleTown_TextBuilt, MSGBOX_DEFAULT
	waitfanfare
GleTown_PlotDone::
	releaseall
	end
GleTown_RivalPlot::
	msgbox GleTown_TextRivalPlot, MSGBOX_DEFAULT
	goto GleTown_PlotDone
GleTownResident::
	lock
	faceplayer
	special GleTownStatus
	msgbox GleTown_TextResidents, MSGBOX_DEFAULT
	release
	end
GleBuilding_Clerk::
	lock
	faceplayer
	special GleBuildingInfo
	compare VAR_RESULT, 6
	goto_if_eq GleBuilding_Dojo
	compare VAR_RESULT, 5
	goto_if_eq GleBuilding_Restaurant
	msgbox GleTown_TextBusiness, MSGBOX_DEFAULT
	special GleTownStatus
	msgbox GleTown_TextResidents, MSGBOX_DEFAULT
	release
	end
GleBuilding_Dojo::
	call GleTraining_Challenge
	goto GleBuilding_Done
GleBuilding_Restaurant::
	msgbox GleTown_TextRestaurant, MSGBOX_DEFAULT
	special HealPlayerParty
	playfanfare MUS_RG_POKE_CENTER
	waitfanfare
GleBuilding_Done::
	release
	end
GleCenter_Nurse::
	lock
	faceplayer
	msgbox Text_WelcomeWantToHealPkmn_Frlg, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleBuilding_Done
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterLeft
	waitmovement 0
	dofieldeffect FLDEFF_POKECENTER_HEAL
	waitfieldeffect FLDEFF_POKECENTER_HEAL
	special HealPlayerParty
	applymovement VAR_LAST_TALKED, Common_Movement_WalkInPlaceFasterDown
	waitmovement 0
	msgbox Text_RestoredPkmnToFullHealth_Frlg, MSGBOX_DEFAULT
	release
	end
GleMart_Clerk::
	lock
	faceplayer
	message gText_HowMayIServeYou
	waitmessage
	pokemart GleMart_Items
	msgbox gText_PleaseComeAgain, MSGBOX_DEFAULT
	release
	end
	.align 2
GleMart_Items:
	.2byte ITEM_POKE_BALL
	.2byte ITEM_GREAT_BALL
	.2byte ITEM_POTION
	.2byte ITEM_SUPER_POTION
	.2byte ITEM_ANTIDOTE
	.2byte ITEM_PARALYZE_HEAL
	.2byte ITEM_ESCAPE_ROPE
	.2byte ITEM_REPEL
	.2byte ITEM_NONE
GleRival_Leader::
	lock
	faceplayer
	msgbox GleTown_TextRival, MSGBOX_DEFAULT
	release
	end
GleTown_TextPlot:
	.string "PLOT {STR_VAR_1} / {STR_VAR_2}\nCurrent: {STR_VAR_3}\pChoose a building or upgrade.\nPlace small buildings anywhere;\llarger projects need larger plots.$"
GleTown_TextBuildConfirm:
	.string "{STR_VAR_1}: TIER {STR_VAR_3}\nBuild / upgrade for ¥{STR_VAR_2}?$"
GleTown_TextBuildFailed:
	.string "This project can't proceed yet.\nCheck money, plot size, rank,\lpopulation and unique buildings.\pStand south of the plot sign\nso construction has a clear site.\lThe BLACK MARKET needs notoriety.$"
GleTown_TextBuilt:
	.string "Construction complete!\nYour town has been updated.\pHouses and hotels add capacity.\nResidents arrive gradually, and\lincome is deposited automatically.$"
GleTown_TextRivalPlot:
	.string "Your rival manages this town.\nIts construction progresses as\lthe world around you develops.$"
GleTown_TextResidents:
	.string "Residents: {STR_VAR_1} / {STR_VAR_2}\nTown reputation: {STR_VAR_3}\pA welcoming town attracts people.\nIncome is deposited as you play.$"
GleTown_TextBusiness:
	.string "Welcome to the {STR_VAR_1}!\nWe're part of this town's future.$"
GleTown_TextDojo:
	.string "Train a POKéMON to your rank cap?\nChoose your trainee on the\lnext screen.$"
GleTown_TextRestaurant:
	.string "Take a seat, LEADER.\nA good meal restores your team.$"
GleTown_TextRival:
	.string "This GYM and town are my project.\nKeep an eye on what I build!$"
GleTownPlot0::
	setvar VAR_0x8007, 0
	goto GleTownPlot

GleTownPlot1::
	setvar VAR_0x8007, 1
	goto GleTownPlot

GleTownPlot2::
	setvar VAR_0x8007, 2
	goto GleTownPlot

GleTownPlot3::
	setvar VAR_0x8007, 3
	goto GleTownPlot

GleTownPlot4::
	setvar VAR_0x8007, 4
	goto GleTownPlot

GleTownPlot5::
	setvar VAR_0x8007, 5
	goto GleTownPlot

GleTownPlot6::
	setvar VAR_0x8007, 6
	goto GleTownPlot

GleTownPlot7::
	setvar VAR_0x8007, 7
	goto GleTownPlot

GleTownPlot8::
	setvar VAR_0x8007, 8
	goto GleTownPlot

GleTownPlot9::
	setvar VAR_0x8007, 9
	goto GleTownPlot

GleTownPlot10::
	setvar VAR_0x8007, 10
	goto GleTownPlot

GleTownPlot11::
	setvar VAR_0x8007, 11
	goto GleTownPlot

GleTownPlot12::
	setvar VAR_0x8007, 12
	goto GleTownPlot

GleTownPlot13::
	setvar VAR_0x8007, 13
	goto GleTownPlot

GleTownPlot14::
	setvar VAR_0x8007, 14
	goto GleTownPlot

GleTownPlot15::
	setvar VAR_0x8007, 15
	goto GleTownPlot

GleTownPlot16::
	setvar VAR_0x8007, 16
	goto GleTownPlot

GleTownPlot17::
	setvar VAR_0x8007, 17
	goto GleTownPlot

GleTownPlot18::
	setvar VAR_0x8007, 18
	goto GleTownPlot

GleTownPlot19::
	setvar VAR_0x8007, 19
	goto GleTownPlot

GleTownPlot20::
	setvar VAR_0x8007, 20
	goto GleTownPlot

GleTownPlot21::
	setvar VAR_0x8007, 21
	goto GleTownPlot

GleTownPlot22::
	setvar VAR_0x8007, 22
	goto GleTownPlot

GleTownPlot23::
	setvar VAR_0x8007, 23
	goto GleTownPlot

GleLandmarkSign::
	lockall
	special GleLandmarkInfo
	compare VAR_RESULT, 2
	goto_if_eq GleTown_RivalPlot
	compare VAR_RESULT, 3
	goto_if_eq GleLandmark_Finished
	msgbox GleLandmark_TextConfirm, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleTown_PlotDone
	special GleLandmarkConstruct
	compare VAR_RESULT, 1
	goto_if_eq GleTown_PlotBuilt
	msgbox GleLandmark_TextLocked, MSGBOX_DEFAULT
	goto GleTown_PlotDone
GleLandmark_Finished::
	msgbox GleLandmark_TextFinished, MSGBOX_DEFAULT
	goto GleTown_PlotDone
GleLandmark_TextConfirm:
	.string "{STR_VAR_1}: TIER {STR_VAR_3}\nDevelop for ¥{STR_VAR_2}?$"
GleLandmark_TextLocked:
	.string "Landmark tiers need ranks 3, 5, 7.\nHigher tiers also need 12 or 24\lresidents, and enough money.\pStand south of the sign\nto keep the work site clear.$"
GleLandmark_TextFinished:
	.string "Your town's landmark is fully\nrestored. Thank you, LEADER!$"

GleTown_TextHabitatType:
	.string "Choose this habitat's type.\nAny type is welcome in your town.\lUpgrades preserve your choice.$"
