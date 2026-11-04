GleTown_MapScripts::
	map_script MAP_SCRIPT_ON_LOAD, GleTown_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, GleTown_OnFrame
	.byte 0
GleTown_OnLoad::
	special GleTownDraw
	special GleTownArrival
	end
GleTown_OnFrame::
	map_script_2 VAR_TEMP_F, 1, GleChampion_Celebration
	.2byte 0
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
	special GleRecognitionInfo
	compare VAR_RESULT, 0
	goto_if_eq GleTownResident_Request
	msgbox gStringVar4, MSGBOX_DEFAULT
GleTownResident_Request::
	special GleResidentRequestInfo
	compare VAR_RESULT, 3
	goto_if_eq GleResident_Completed
	compare VAR_RESULT, 4
	goto_if_eq GleResident_Decision
	compare VAR_RESULT, 2
	goto_if_eq GleResident_BuildingReady
	compare VAR_RESULT, 1
	goto_if_eq GleResident_Pending
GleResident_Status::
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
	special GleRivalInfo
	compare VAR_RESULT, 0
	goto_if_ne GleRival_Completed
	msgbox GleTown_TextRival, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleRival_Done
	closemessage
	special GymLeaderStartRivalFriendly
	compare VAR_RESULT, 1
	goto_if_eq GleRival_Won
	compare VAR_RESULT, 2
	goto_if_eq GleRival_Lost
	msgbox GleText_BadTeam, MSGBOX_DEFAULT
	goto GleRival_Done
GleRival_Completed::
	msgbox GleTown_TextRivalCompleted, MSGBOX_DEFAULT
	goto GleRival_Done
GleRival_Won::
	msgbox GleTown_TextRivalWon, MSGBOX_DEFAULT
	goto GleRival_Done
GleRival_Lost::
	msgbox GleTown_TextRivalLost, MSGBOX_DEFAULT
GleRival_Done::
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
	.string "{RIVAL}: My GYM specializes in\n{STR_VAR_1} POKéMON.\pWe'll use your current rank rules:\n{STR_VAR_3} POKéMON, level cap {STR_VAR_2}.\pIf I'm ahead, I'll scale down.\nOur ranks won't change.\pBring your registered GYM team.\nReady for a friendly?$"
GleTown_TextRivalCompleted:
	.string "{RIVAL}: You won this rank's\nfriendly. Come back at a new rank!\pThis town is still my project.\nKeep an eye on what I build!$"
GleTown_TextRivalWon:
	.string "{RIVAL}: Good match, LEADER!\nYour GYM has come a long way.\pYour one-time prize was deposited.\nOur ranks stay as they were.$"
GleTown_TextRivalLost:
	.string "{RIVAL}: Keep working, LEADER!\nWe can try this friendly again.\pYour team is healed.\nYour rank hasn't changed.$"
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

GleResident_Pending::
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto GleResident_Standing
GleResident_BuildingReady::
	msgbox gStringVar4, MSGBOX_DEFAULT
	msgbox GleResident_TextCollect, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleResident_Standing
	setvar VAR_RESULT, 0
	special GleResidentRequestResolve
	goto GleResident_Result
GleResident_Decision::
	msgbox gStringVar4, MSGBOX_DEFAULT
	closemessage
	special GleResidentRequestChoices
	compare VAR_RESULT, 2
	goto_if_ge GleResident_Standing
	special GleResidentRequestResolve
GleResident_Result::
	compare VAR_RESULT, 0
	goto_if_ne GleResident_Completed
	playfanfare MUS_OBTAIN_ITEM
	msgbox GleResident_TextResolved, MSGBOX_DEFAULT
	waitfanfare
	goto GleResident_Standing
GleResident_Completed::
	msgbox GleResident_TextThanks, MSGBOX_DEFAULT
GleResident_Standing::
	compare VAR_LAST_TALKED, 1
	goto_if_ne GleResident_End
	special GleTownStandingInfo
	msgbox gStringVar4, MSGBOX_DEFAULT
	special GleRocketJournal
	msgbox gStringVar4, MSGBOX_DEFAULT
GleResident_End::
	release
	end
GleResident_TextCollect:
	.string "The requested building is open!\nAccept the one-time contribution?$"
GleResident_TextResolved:
	.string "Your decision is recorded.\nThe funds and town records\lhave been updated.$"
GleResident_TextThanks:
	.string "Thank you for hearing us out.\nOur earlier decision is recorded.\lKeep making this town your own!$"

GleRocket_Contact::
	lockall
	faceplayer
GleRocket_Status::
	special GleRocketInfo
	compare VAR_RESULT, 2
	goto_if_eq GleRocket_Decision
	compare VAR_RESULT, 1
	goto_if_ne GleRocket_Done
	msgbox GleRocket_TextIncident, MSGBOX_DEFAULT
	special GleRocketInfo
	msgbox gStringVar4, MSGBOX_DEFAULT
	msgbox GleRocket_TextChallenge, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleRocket_Done
	closemessage
	special GymLeaderStartTownDefense
	compare VAR_RESULT, 1
	goto_if_eq GleRocket_Won
	compare VAR_RESULT, 2
	goto_if_eq GleRocket_Lost
	msgbox GleRocket_TextInvalid, MSGBOX_DEFAULT
	goto GleRocket_Done
GleRocket_Won::
	msgbox GleRocket_TextWon, MSGBOX_DEFAULT
	goto GleRocket_Status
GleRocket_Lost::
	msgbox GleRocket_TextLost, MSGBOX_DEFAULT
	goto GleRocket_Done
GleRocket_Decision::
	msgbox GleRocket_TextDecision, MSGBOX_DEFAULT
	closemessage
	special GleRocketDecisionMenu
	compare VAR_RESULT, 2
	goto_if_ge GleRocket_Done
	special GleRocketResolve
	compare VAR_RESULT, 0
	goto_if_ne GleRocket_Done
	removeobject 235
	special GleRocketRefresh
	msgbox GleRocket_TextResolved, MSGBOX_DEFAULT
GleRocket_Done::
	special GleTownArrival
	releaseall
	end
GleRocket_TextIncident:
	.string "A {STR_VAR_3}!\nYour staff held off {STR_VAR_2} groups.\l{STR_VAR_1} battles remain for you.$"
GleRocket_TextChallenge:
	.string "Use your traveling party to fight?\nAny type is allowed. These fights\lwon't change your Gym Rank.\pYour team is restored and healed\nafter each fight. Ready?$"
GleRocket_TextWon:
	.string "That group has been driven back!\nYour town's defense continues.$"
GleRocket_TextLost:
	.string "Rocket still holds its position.\nYour team has been restored.\lRegroup and try again.$"
GleRocket_TextInvalid:
	.string "Bring at least one healthy partner.\nNo registered-team rules apply\lto these town-defense battles.$"
GleRocket_TextDecision:
	.string "The raid has been defeated.\nRocket offers hush money to leave\lwithout being reported.\pThe GLA also pays for turning\nraiders over. How will you handle it?\lYou may decide later.$"
GleRocket_TextResolved:
	.string "The incident is resolved.\nYour funds, standing and town\lrecords have been updated.\pOrdinary town income resumes.\nBuildings and their upgrades\lremain in place.$"

GleChampion_Celebration::
	lockall
	setvar VAR_TEMP_F, 0
	special GleCelebrationInfo
	compare VAR_RESULT, 0
	goto_if_eq GleExclusive_Done
	addobject 245
	addobject 246
	compare VAR_RESULT, 2
	goto_if_eq GleChampion_ShortCelebration
	playfanfare MUS_OBTAIN_BADGE
	msgbox gStringVar4, MSGBOX_DEFAULT
	special GleCelebrationStaffText
	msgbox gStringVar4, MSGBOX_DEFAULT
	msgbox GleChampion_TextHome, MSGBOX_DEFAULT
	waitfanfare
	goto GleChampion_EndCelebration
GleChampion_ShortCelebration::
	playfanfare MUS_OBTAIN_ITEM
	msgbox gStringVar4, MSGBOX_DEFAULT
	waitfanfare
GleChampion_EndCelebration::
	special GleCelebrationFinish
	releaseall
	end
GleChampion_Celebrant::
	lock
	faceplayer
	special GleRecognitionInfo
	msgbox gStringVar4, MSGBOX_DEFAULT
	release
	end
GleChampion_Display::
	msgbox GleChampion_TextDisplay, MSGBOX_SIGN
	end
GleChampion_TextHome:
	.string "Our town's championship memorial\nwill honour this achievement.\pThe celebrations may be brief,\nbut our pride in this Gym\lwill stay with us.$"
GleChampion_TextDisplay:
	.string "The town's championship memorial.\nA place in Kanto's history,\lshared by this Gym and its people.$"
GleChampion_NewsNpc::
	lock
	faceplayer
	special GleRecognitionInfo
	msgbox gStringVar4, MSGBOX_DEFAULT
	msgbox GleChampion_TextReadNews, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleHall_Stay
GleChampion_NewsMenu::
	closemessage
	special GleNewsMenu
	compare VAR_RESULT, 5
	goto_if_ge GleHall_Stay
	special GleNewsPage
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto GleChampion_NewsMenu
GleChampion_TextReadNews:
	.string "Read the latest Kanto Gazette?$"
