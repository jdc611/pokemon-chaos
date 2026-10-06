ChaosPokemonRanch_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, ChaosRanch_Populate
	map_script MAP_SCRIPT_ON_RESUME, ChaosRanch_Populate
	map_script MAP_SCRIPT_ON_LOAD, ChaosRanch_Scenery
	.byte 0

ChaosRanch_Scenery::
	special ChaosArcadeRanchScenery
	end

ChaosRanch_Populate::
	special ChaosRanchPopulate
	end

ChaosRanch_Door::
	lockall
	msgbox ChaosRanch_Text_Choose, MSGBOX_DEFAULT
	closemessage
	special ChaosRanchChooseBox
	compare VAR_RESULT, 14
	goto_if_ge ChaosRanch_Cancel
	special ChaosRanchEnter
	closemessage
	warpdoor MAP_CHAOS_POKEMON_RANCH, 0
	waitstate
	releaseall
	end

ChaosRanch_Sign::
	lockall
	msgbox ChaosRanch_Text_Sign, MSGBOX_DEFAULT
ChaosRanch_Options::
	closemessage
	special ChaosRanchOptions
	compare VAR_RESULT, 0
	goto_if_eq ChaosRanch_BoxSelect
	compare VAR_RESULT, 1
	goto_if_eq ChaosRanch_ThemeSelect
	compare VAR_RESULT, 2
	goto_if_eq ChaosRanch_DecorationSelect
	goto ChaosRanch_Cancel
ChaosRanch_BoxSelect::
	special ChaosRanchChooseBox
	compare VAR_RESULT, 14
	goto_if_ge ChaosRanch_Options
	special ChaosRanchChangeBox
	closemessage
	warp MAP_CHAOS_POKEMON_RANCH, 0
	waitstate
	releaseall
	end
ChaosRanch_ThemeSelect::
	special ChaosRanchThemeMenu
	special ChaosRanchApplyCosmetic
	goto ChaosRanch_Options
ChaosRanch_DecorationSelect::
	special ChaosRanchDecorationMenu
	special ChaosRanchApplyCosmetic
	goto ChaosRanch_Options

ChaosRanch_Interact::
	lock
	faceplayer
	special ChaosRanchMenu
	compare VAR_RESULT, 0
	goto_if_eq ChaosRanch_Summary
	compare VAR_RESULT, 1
	goto_if_eq ChaosRanch_Withdraw
	compare VAR_RESULT, 2
	goto_if_eq ChaosRanch_TakeItem
	goto ChaosRanch_Cancel

ChaosRanch_Summary::
	special ChaosRanchSummary
	release
	end

ChaosRanch_Withdraw::
	special ChaosRanchWithdraw
	compare VAR_RESULT, 2
	goto_if_eq ChaosRanch_Grave
	compare VAR_RESULT, 1
	goto_if_eq ChaosRanch_Full
	msgbox ChaosRanch_Text_Withdrawn, MSGBOX_DEFAULT
	release
	end

ChaosRanch_TakeItem::
	special ChaosRanchTakeItem
	compare VAR_RESULT, 1
	goto_if_eq ChaosRanch_NoItem
	compare VAR_RESULT, 2
	goto_if_eq ChaosRanch_BagFull
	compare VAR_RESULT, 3
	goto_if_eq ChaosRanch_RestrictedItem
	msgbox ChaosRanch_Text_Item, MSGBOX_DEFAULT
	release
	end

ChaosRanch_Grave::
	msgbox ChaosRanch_Text_Grave, MSGBOX_DEFAULT
	goto ChaosRanch_Cancel
ChaosRanch_Full::
	msgbox ChaosRanch_Text_Full, MSGBOX_DEFAULT
	goto ChaosRanch_Cancel
ChaosRanch_NoItem::
	msgbox ChaosRanch_Text_NoItem, MSGBOX_DEFAULT
	goto ChaosRanch_Cancel
ChaosRanch_BagFull::
	msgbox ChaosRanch_Text_BagFull, MSGBOX_DEFAULT
	goto ChaosRanch_Cancel
ChaosRanch_RestrictedItem::
	msgbox ChaosRanch_Text_RestrictedItem, MSGBOX_DEFAULT
	goto ChaosRanch_Cancel
ChaosRanch_Cancel::
	releaseall
	end

ChaosRanch_Text_Choose::
	.string "Visit the POKéMON RANCH?\nChoose a PC Box pasture.$"
ChaosRanch_Text_Sign::
	.string "POKéMON RANCH\pPOKéMON stored in your PC Boxes\ncan roam freely here.\pChoose a pasture or decorate.$"
ChaosRanch_Text_Withdrawn::
	.string "The POKéMON joined your party!$"
ChaosRanch_Text_Full::
	.string "Your party is full.\nThe POKéMON will stay here.$"
ChaosRanch_Text_Grave::
	.string "POKéMON in the GRAVE Box\ncannot be withdrawn.$"
ChaosRanch_Text_NoItem::
	.string "This POKéMON isn't holding an item.$"
ChaosRanch_Text_BagFull::
	.string "Your BAG is full.\nThe POKéMON keeps its item.$"
ChaosRanch_Text_RestrictedItem::
	.string "This item cannot be taken here.\nPlease check the PC Storage System.$"
ChaosRanch_Text_Item::
	.string "You took the {STR_VAR_1}\nand put it in your BAG.$"
