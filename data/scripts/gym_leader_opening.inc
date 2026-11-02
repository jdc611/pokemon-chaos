GleLab_Opening::
	lockall
	setvar VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 9
	msgbox GleText_Welcome, MSGBOX_DEFAULT
GleLab_Town::
	closemessage
	special GleTownMenu
	compare VAR_RESULT, 0
	goto_if_eq GleLab_Saffron
	compare VAR_RESULT, 1
	goto_if_eq GleLab_Celadon
	compare VAR_RESULT, 2
	goto_if_eq GleLab_Fuchsia
	goto GleLab_Town
GleLab_Saffron::
	msgbox GleText_Saffron, MSGBOX_DEFAULT
	setvar VAR_0x8007, 0
	goto GleLab_TownConfirm
GleLab_Celadon::
	msgbox GleText_Celadon, MSGBOX_DEFAULT
	setvar VAR_0x8007, 1
	goto GleLab_TownConfirm
GleLab_Fuchsia::
	msgbox GleText_Fuchsia, MSGBOX_DEFAULT
	setvar VAR_0x8007, 2
GleLab_TownConfirm::
	msgbox GleText_TownConfirm, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleLab_Town
	copyvar VAR_RESULT, VAR_0x8007
	special GleTownChoose
	msgbox GleText_Rename, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleLab_ToGym
	closemessage
	special GleTownRename
GleLab_ToGym::
	msgbox GleText_ToGym, MSGBOX_DEFAULT
	closemessage
	special GleWarpHome
	waitstate
	releaseall
	end

GleGym_Opening::
	lockall
	setvar VAR_TEMP_0, 0
	special GleOpeningStage
	compare VAR_RESULT, 3
	goto_if_eq GleGym_Draft
	compare VAR_RESULT, 2
	goto_if_eq GleGym_Signature
	msgbox GleText_YourGym, MSGBOX_DEFAULT
GleGym_Type::
	closemessage
	special GleTypeMenu
	special GleTypeChoose
	compare VAR_RESULT, 1
	goto_if_eq GleGym_Signature
	msgbox GleText_TryType, MSGBOX_DEFAULT
	goto GleGym_Type
GleGym_Signature::
	msgbox GleText_Signature, MSGBOX_DEFAULT
GleGym_SignatureMenu::
	closemessage
	special GleSignatureMenu
	copyvar VAR_0x8007, VAR_RESULT
	special GleSignatureInfo
	msgbox GleText_SignatureConfirm, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleGym_SignatureMenu
	special GleSignatureChoose
	msgbox GleText_Draft, MSGBOX_DEFAULT
GleGym_Draft::
	closemessage
	special GleOpeningStage
	compare VAR_RESULT, 4
	goto_if_eq GleGym_ToLeague
	special GleDraftMenu
	copyvar VAR_0x8007, VAR_RESULT
	compare VAR_RESULT, 3
	goto_if_ge GleGym_Draft
GleGym_DraftDetail::
	special GleDraftInfo
	msgbox GleText_DraftInfo, MSGBOX_DEFAULT
	closemessage
	special GleDraftActions
	compare VAR_RESULT, 0
	goto_if_eq GleGym_DraftChoose
	compare VAR_RESULT, 1
	goto_if_eq GleGym_DraftSummary
	goto GleGym_Draft
GleGym_DraftSummary::
	closemessage
	special GleDraftSummary
	goto GleGym_DraftDetail
GleGym_DraftChoose::
	special GleDraftChoose
	compare VAR_RESULT, 1
	goto_if_eq GleGym_DraftReceived
	msgbox GleText_ChoiceFailed, MSGBOX_DEFAULT
	goto GleGym_Draft
GleGym_DraftReceived::
	playfanfare MUS_EVOLVED
	msgbox GleText_Joined, MSGBOX_DEFAULT
	waitfanfare
	goto GleGym_Draft
GleGym_ToLeague::
	msgbox GleText_LeagueVisit, MSGBOX_DEFAULT
	closemessage
	special GleWarpLeague
	waitstate
	releaseall
	end

GleHall_Opening::
	lockall
	setvar VAR_TEMP_0, 0
	msgbox GleText_LeagueWelcome, MSGBOX_DEFAULT
	msgbox GleText_LeagueRules, MSGBOX_DEFAULT
	special GleFinishOpening
	releaseall
	end

GleGym_Guy::
	lock
	faceplayer
	msgbox GleText_Heal, MSGBOX_DEFAULT
	special HealPlayerParty
	playfanfare MUS_RG_POKE_CENTER
	waitfanfare
	release
	end
GleGym_Oak::
	lock
	faceplayer
	msgbox GleText_OakAdvice, MSGBOX_DEFAULT
	release
	end
GleHall_Guy::
	lock
	faceplayer
	msgbox GleText_HallAdvice, MSGBOX_DEFAULT
	release
	end

GleText_Welcome:
	.string "OAK: Welcome, {PLAYER}!\nKANTO needs a new generation of\lGYM LEADERS.\pChoose a hometown to rebuild.\nYour rival will lead another town.\lThe remaining town stays as it is.$"
GleText_Saffron:
	.string "SAFFRON: Central routes and\na broad urban footprint.\pLandmark: SILPH CO.\nAn ambitious city to restore.$"
GleText_Celadon:
	.string "CELADON: A spacious commercial\ncity with shopping and visitors.\pLandmark: the GAME CORNER.\nBalance profit with town appeal.$"
GleText_Fuchsia:
	.string "FUCHSIA: A quieter southern town\nwith a focus on nature.\pLandmark: the SAFARI ZONE.\nChoose which species live there.$"
GleText_TownConfirm:
	.string "Make this your hometown?$"
GleText_Rename:
	.string "Would you like to rename\nyour town?$"
GleText_ToGym:
	.string "OAK: Your GYM is ready.\nLet's go and make it your own!$"
GleText_YourGym:
	.string "OAK: This is your GYM.\nThe GYM GUIDE can heal your team.\pFirst, choose your specialty.\nYour official team must share\lyour GYM's type.$"
GleText_TryType:
	.string "We couldn't prepare that draft.\nPlease choose your type again.$"
GleText_Signature:
	.string "Choose your signature TM.\nYou'll keep a reusable copy.\pOne choice is a reliable attack,\none is tactical, and one helps\lbuild momentum. Choose carefully.$"
GleText_SignatureConfirm:
	.string "Your {STR_VAR_2}:\n{STR_VAR_1}.\pMake this your permanent\nsignature TM?$"
GleText_Draft:
	.string "Now draft your starting team.\nChoose one POKéMON in each\lof three rounds.\pEach begins at Lv.5.\nYou can inspect every candidate's\lability and SUMMARY before choosing.$"
GleText_DraftInfo:
	.string "{STR_VAR_1} / Lv.5\nGYM type: {STR_VAR_3}\lAbility: {STR_VAR_2}$"
GleText_ChoiceFailed:
	.string "That choice wasn't completed.\nYour draft has been preserved.$"
GleText_Joined:
	.string "{STR_VAR_1} joined your team!$"
GleText_LeagueVisit:
	.string "OAK: Your team is ready!\nNext, meet the GYM LEADER\lASSOCIATION in VIRIDIAN.$"
GleText_LeagueWelcome:
	.string "Welcome to the GLA!\nYour GYM's success and your town's\lfuture are in your hands.$"
GleText_LeagueRules:
	.string "GYM RANK sets your level cap\nand official team size.\pAn off-type POKéMON may join your\nofficial team if it knows your\lsignature move. Only one at a time.\pOutside official battles,\nyou may train any POKéMON.\lExplore, catch, and build your team!$"
GleText_Heal:
	.string "Welcome back, LEADER!\nLet's get your team ready.$"
GleText_OakAdvice:
	.string "OAK: A great LEADER learns from\nevery challenger. Take care of\lyour POKéMON and your town.$"
GleText_HallAdvice:
	.string "Explore KANTO and strengthen\nyour team. Your GYM GUIDE can\lheal your POKéMON whenever you return.$"
