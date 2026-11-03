GleHall_Services::
	closemessage
	special GleServicesMenu
	compare VAR_RESULT, 0
	goto_if_eq GleHall_Vouchers
	compare VAR_RESULT, 1
	goto_if_eq GleHall_Milestones
	compare VAR_RESULT, 2
	goto_if_eq GleHall_Training
	compare VAR_RESULT, 3
	goto_if_eq GleHall_Home
	goto GleHall_Stay
GleHall_Home::
	closemessage
	special GleWarpHome
	waitstate
	goto GleHall_Stay
GleHall_Milestones::
	special GleMilestoneRewards
	msgbox GleServices_TextMilestones, MSGBOX_DEFAULT
	goto GleHall_Services
GleHall_Vouchers::
	closemessage
	special GleVoucherMenu
	compare VAR_RESULT, 24
	goto_if_ge GleHall_VoucherEmpty
	special GleVoucherOpen
	compare VAR_RESULT, 0
	goto_if_ne GleHall_VoucherUnavailable
GleHall_VoucherChoices::
	closemessage
	special GleVoucherChoices
	compare VAR_RESULT, 3
	goto_if_ge GleHall_Services
	copyvar VAR_0x8007, VAR_RESULT
GleHall_VoucherInfo::
	special GleVoucherInfo
	msgbox GleServices_TextCandidate, MSGBOX_DEFAULT
	closemessage
	special GleDraftActions
	compare VAR_RESULT, 0
	goto_if_eq GleHall_VoucherConfirm
	compare VAR_RESULT, 1
	goto_if_eq GleHall_VoucherSummary
	goto GleHall_VoucherChoices
GleHall_VoucherSummary::
	closemessage
	special GleVoucherSummary
	goto GleHall_VoucherInfo
GleHall_VoucherConfirm::
	msgbox GleServices_TextClaim, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleHall_VoucherInfo
	special GleVoucherClaim
	compare VAR_RESULT, 0
	goto_if_ne GleHall_VoucherFull
	playfanfare MUS_EVOLVED
	msgbox GleServices_TextClaimed, MSGBOX_DEFAULT
	waitfanfare
	goto GleHall_Services
GleHall_VoucherFull::
	msgbox GleServices_TextFull, MSGBOX_DEFAULT
	goto GleHall_Services
GleHall_VoucherEmpty::
	msgbox GleServices_TextEmpty, MSGBOX_DEFAULT
	goto GleHall_Services
GleHall_VoucherUnavailable::
	msgbox GleServices_TextUnavailable, MSGBOX_DEFAULT
	goto GleHall_Services
GleHall_Training::
	call GleTraining_Challenge
	goto GleHall_Services
GleTraining_Challenge::
	msgbox GleServices_TextTraining, MSGBOX_DEFAULT
	closemessage
	special GymLeaderTrainingMenu
	compare VAR_RESULT, 3
	goto_if_gt GleTraining_Return
	special GymLeaderStartTraining
	compare VAR_RESULT, 10
	goto_if_ge GleTraining_Unavailable
	msgbox GleServices_TextTrainingDone, MSGBOX_DEFAULT
GleTraining_Return::
	return
GleTraining_Unavailable::
	msgbox GleServices_TextTrainingUnavailable, MSGBOX_DEFAULT
	return
GleServices_TextMilestones:
	.string "{STR_VAR_1} milestone voucher(s) added.\nRewards wait here if your voucher\lcase is full. No cash is required.$"
GleServices_TextCandidate:
	.string "{STR_VAR_1} / Lv.{STR_VAR_3}\nAbility: {STR_VAR_2}\pYou can inspect the SUMMARY.\nThese choices stay the same when\lyou return to this voucher.$"
GleServices_TextClaim:
	.string "Choose this partner?\nThe voucher is used only when\lyour POKéMON is delivered.$"
GleServices_TextClaimed:
	.string "{STR_VAR_1} is yours!\nYour partner arrives in a GLA BALL.\lCheck your party or PC.$"
GleServices_TextFull:
	.string "There is no room for a partner.\nMake space in your party or PC.\lYour voucher is still safe.$"
GleServices_TextEmpty:
	.string "You have no vouchers right now.\nCheck milestone rewards as your\lGYM rank grows.$"
GleServices_TextUnavailable:
	.string "No unclaimed choices are available\nfor that voucher right now.\lYour voucher has been kept.$"
GleServices_TextTraining:
	.string "Practice with your registered team.\nPOKéMON keep earned EXP. We heal\lboth sides after each match.\pPractice gives no money or GYM XP.\nAdvanced: rank 4. Expert: rank 7.$"
GleServices_TextTrainingDone:
	.string "Good practice! Your team is healed.\nYou can train again whenever\lyou like.$"
GleServices_TextTrainingUnavailable:
	.string "Check your registered team with\nyour GYM GUIDE. Official team\lrules apply to practice too.\pAdvanced needs rank 4; expert, 7.$"

GleSilphResearch::
	lock
	faceplayer
	msgbox GleResearch_TextWelcome, MSGBOX_DEFAULT
GleResearch_Notebook::
	closemessage
	special GleResearchMenu
	compare VAR_RESULT, 16
	goto_if_ge GleHall_Stay
	compare VAR_RESULT, 0
	goto_if_eq GleResearch_Null
	compare VAR_RESULT, 14
	goto_if_eq GleResearch_Slaking
	compare VAR_RESULT, 4
	goto_if_eq GleResearch_Regigigas
	compare VAR_RESULT, 3
	goto_if_eq GleResearch_Titans
	compare VAR_RESULT, 12
	goto_if_eq GleResearch_Wings
	compare VAR_RESULT, 7
	goto_if_eq GleResearch_Field
	compare VAR_RESULT, 8
	goto_if_eq GleResearch_Field
	compare VAR_RESULT, 9
	goto_if_eq GleResearch_Field
	compare VAR_RESULT, 11
	goto_if_eq GleResearch_Field
	compare VAR_RESULT, 13
	goto_if_eq GleResearch_Field
	special GleResearchInfo
	msgbox GleResearch_TextStatus, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleSilphPrivate::
	msgbox GleResearch_TextPrivate, MSGBOX_NPC
	end
GleResearch_TextWelcome:
	.string "Welcome to SILPH's public lab.\nYour field discoveries remain in\lyour research notebook.$"
GleResearch_TextStatus:
	.string "{STR_VAR_1}\nEvidence stage: {STR_VAR_2}\l{STR_VAR_3}$"
GleResearch_TextPrivate:
	.string "These floors are for laboratory\nstaff. Our public research desk\lis in the lobby.$"

GleSafari_Desk::
	msgbox GleSafari_TextWelcome, MSGBOX_DEFAULT
GleSafari_SelectSlot::
	closemessage
	special GleSafariSlots
	compare VAR_RESULT, 6
	goto_if_ge GleHall_Stay
	copyvar VAR_0x8008, VAR_RESULT
	closemessage
	special GleTypeMenu
	special GleSafariSpeciesMenu
	special GleSafariPrice
	compare VAR_RESULT, 1
	goto_if_ne GleSafari_SelectSlot
	msgbox GleSafari_TextPrice, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleSafari_SelectSlot
	special GleSafariConfigure
	compare VAR_RESULT, 0
	goto_if_ne GleSafari_Failed
	msgbox GleSafari_TextPlaced, MSGBOX_DEFAULT
	goto GleSafari_SelectSlot
GleSafari_Failed::
	msgbox GleSafari_TextFailed, MSGBOX_DEFAULT
	goto GleSafari_SelectSlot
GleSafari_TextWelcome:
	.string "Manage SAFARI grass habitats.\nEach slot supports one species.\lLandmark upgrades open more slots.\pSelect a slot, then a type.\nStronger species cost more to\lsettle here. Native wildlife stays\lwhere you leave a slot unchanged.$"
GleSafari_TextPrice:
	.string "This replaces the selected slot.\nOther slots stay as they are.\pSettle {STR_VAR_1}\nfor ¥{STR_VAR_2}?$"
GleSafari_TextPlaced:
	.string "The habitat is ready!\nYour selected species now lives\lin the SAFARI ZONE.$"
GleSafari_TextFailed:
	.string "That species is already settled,\nor the funds are insufficient.\lYour wildlife is unchanged.$"

GleSupply_Clerk::
	lock
	faceplayer
	msgbox GleSupply_TextWelcome, MSGBOX_DEFAULT
GleSupply_Category::
	closemessage
	special GleSupplyMenu
	compare VAR_RESULT, 5
	goto_if_ge GleHall_Stay
	special GleSupplyOpen
	compare VAR_RESULT, 0
	goto_if_eq GleSupply_Locked
	goto GleSupply_Category
GleSupply_Locked::
	msgbox GleSupply_TextLocked, MSGBOX_DEFAULT
	goto GleSupply_Category
GleSupply_TextWelcome:
	.string "GLA supply desk! Stock expands\nwith your rank. Honorable leaders\lreceive a 10 percent discount.$"
GleSupply_TextLocked:
	.string "No supplies in that category yet.\nRaise your rank or begin the\lTYPE: NULL research placement.$"
GleResearch_Gla::
	lock
	faceplayer
	msgbox GleResearch_TextGla, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_Null::
	special GleNullStatus
	compare VAR_RESULT, 0
	goto_if_eq GleResearch_NullLocked
	compare VAR_RESULT, 1
	goto_if_eq GleResearch_NullOffer
	compare VAR_RESULT, 3
	goto_if_eq GleResearch_NullDone
	msgbox GleResearch_TextNullCare, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_NullLocked::
	msgbox GleResearch_TextNullLocked, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_NullOffer::
	msgbox GleResearch_TextNullOffer, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleResearch_Notebook
	special GleNullClaim
	compare VAR_RESULT, 0
	goto_if_ne GleResearch_NullFull
	playfanfare MUS_EVOLVED
	msgbox GleResearch_TextNullGiven, MSGBOX_DEFAULT
	waitfanfare
	goto GleResearch_Notebook
GleResearch_NullFull::
	msgbox GleResearch_TextNullFull, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_NullDone::
	msgbox GleResearch_TextNullDone, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_TextGla:
	.string "GLA field research desk.\nLet's review your notebook and\lcare for unusual partners.$"
GleResearch_TextNullLocked:
	.string "TYPE: NULL needs a patient leader.\nReach rank 3 to qualify for our\lrehabilitation placement.$"
GleResearch_TextNullOffer:
	.string "Will you care for TYPE: NULL?\nEarn its trust through friendship\land report its evolution to us.$"
GleResearch_TextNullGiven:
	.string "TYPE: NULL has joined you!\nCheck your party or PC. MEMORIES\lare now stocked at the GLA desk.$"
GleResearch_TextNullCare:
	.string "Raise TYPE: NULL with friendship.\nA SOOTHE BELL can help. Return\lwith SILVALLY in your party or PC.$"
GleResearch_TextNullDone:
	.string "SILVALLY has accepted you!\nPlacement complete. Held MEMORIES\llet it change its type.$"

GleResearch_Slaking::
	special GleSlakingResearch
	compare VAR_RESULT, 1
	goto_if_eq GleResearch_SlakingDone
	compare VAR_RESULT, 2
	goto_if_eq GleResearch_SlakingKnown
	msgbox GleResearch_TextSlakingCare, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_SlakingDone::
	playfanfare MUS_EVOLVED
	msgbox GleResearch_TextSlakingDone, MSGBOX_DEFAULT
	waitfanfare
	goto GleResearch_Notebook
GleResearch_SlakingKnown::
	msgbox GleResearch_TextSlakingKnown, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_TextSlakingCare:
	.string "At rank 5, bring your special\nSLAKING in your party or PC.\lWe can awaken its VITAL SPIRIT.$"
GleResearch_TextSlakingDone:
	.string "Our research has awakened your\nspecial SLAKING's VITAL SPIRIT!\lOrdinary SLAKING are unchanged.$"
GleResearch_TextSlakingKnown:
	.string "Your special SLAKING's awakening\nis complete. Keep exploring and\ltraining together!$"

GleResearch_TextNullFull:
	.string "There is no room for a partner.\nMake space in your party or PC.\lYour placement will wait here.$"

GleResearch_Wings::
	special GleWingsNotebook
	compare VAR_RESULT, 0
	goto_if_eq GleResearch_WingsLocked
	msgbox GleResearch_TextWings, MSGBOX_DEFAULT
GleResearch_WingsMenu::
	closemessage
	special GleWingsMenu
	compare VAR_RESULT, 6
	goto_if_ge GleResearch_Notebook
	special GleWingsReport
	compare VAR_RESULT, 1
	goto_if_eq GleResearch_WingsRecorded
	msgbox GleResearch_TextWingsReport, MSGBOX_DEFAULT
	goto GleResearch_WingsMenu
GleResearch_WingsRecorded::
	msgbox GleResearch_TextWingsRecorded, MSGBOX_DEFAULT
	goto GleResearch_WingsMenu
GleResearch_WingsLocked::
	msgbox GleResearch_TextWingsLocked, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_TextWingsLocked:
	.string "Migration surveys open at rank 3.\nAll six birds are independent\lresearch encounters.$"
GleResearch_TextWings:
	.string "Wings of Kanto: {STR_VAR_1}/6 recorded.\nUnusual migrations bring six rare\lbirds to distant roosts.\pReview our sightings, then explore.\nEach bird can be approached on its\lown. None requires another capture.$"
GleResearch_TextWingsReport:
	.string "{STR_VAR_1}\nLast sighted: {STR_VAR_2}\lSurvey rank: {STR_VAR_3}$"
GleResearch_TextWingsRecorded:
	.string "{STR_VAR_1}\nIts capture is already recorded.\lOther birds remain independent.$"
GleWings_Approach::
	lockall
	faceplayer
	special GleWingsInfo
	compare VAR_RESULT, 2
	goto_if_eq GleExclusive_AlreadyCaught
	compare VAR_RESULT, 1
	goto_if_ne GleExclusive_Locked
	msgbox GleExclusive_TextSave, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleWings_Confirm
	call Common_EventScript_SaveGame
GleWings_Confirm::
	special GleWingsInfo
	msgbox GleExclusive_TextConfirm, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleExclusive_Done
	closemessage
	special GleWingsStart
	special GleWingsFinish
	compare VAR_RESULT, 1
	goto_if_eq GleWings_Caught
	msgbox GleExclusive_TextRetry, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleWings_Caught::
	removeobject 250
	msgbox GleResearch_TextWingsCaught, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleResearch_TextWingsCaught:
	.string "This migration is now recorded!\nYour partner is safe in your party\lor PC. Other birds still await.$"

GleTutor_Desk::
	lockall
	faceplayer
	msgbox GleTutor_TextWelcome, MSGBOX_DEFAULT
GleTutor_Menu::
	closemessage
	special GleTutorMenu
	special GleTutorInfo
	compare VAR_RESULT, 2
	goto_if_eq GleExclusive_Done
	compare VAR_RESULT, 1
	goto_if_eq GleTutor_Known
	msgbox GleTutor_TextUnlock, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleTutor_Menu
	special GleTutorPurchase
	compare VAR_RESULT, 0
	goto_if_ne GleTutor_NoMoney
	msgbox GleTutor_TextUnlocked, MSGBOX_DEFAULT
	goto GleTutor_Lesson
GleTutor_Known::
	msgbox GleTutor_TextKnown, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleTutor_Menu
GleTutor_Lesson::
	closemessage
	call MoveTutor_EventScript_OpenBox
	goto GleTutor_Menu
GleTutor_NoMoney::
	msgbox GleTutor_TextNoMoney, MSGBOX_DEFAULT
	goto GleTutor_Menu
GleTutor_TextWelcome:
	.string "GLA move tutoring. New lessons\nopen from rank 2. I offer moves\lyour current party can learn.\pUnlock each lesson once, then\nreuse it freely with compatible\lpartners from your party or PC.$"
GleTutor_TextUnlock:
	.string "{STR_VAR_1}: {STR_VAR_2} POKéDOLLARS.\nUnlock this permanent lesson?\lHonorable discounts apply.$"
GleTutor_TextUnlocked:
	.string "The lesson is permanently yours!\nCancelling teaching keeps it.\lChoose a compatible partner.$"
GleTutor_TextKnown:
	.string "{STR_VAR_1} is already unlocked.\nTeach it again for free?$"
GleTutor_TextNoMoney:
	.string "The lesson wasn't unlocked.\nCheck your funds and rank.\lYour money is unchanged.$"

GleResearch_Titans::
	special GleTitansNotebook
	msgbox GleTitans_TextNotebook, MSGBOX_DEFAULT
GleTitans_NotebookMenu::
	closemessage
	special GleTitansMenu
	compare VAR_RESULT, 6
	goto_if_ge GleResearch_Notebook
	special GleTitansReport
	msgbox GleTitans_TextReport, MSGBOX_DEFAULT
	goto GleTitans_NotebookMenu
GleTitans_TextNotebook:
	.string "Ancient Titans: {STR_VAR_1}/5 chambers.\nExplore Tanoby's first five chambers\land read each titan's seal.\pDiscover all five to open Rixy.\nNo other titan must be caught!\lGLA expeditions reach the ruins.$"
GleTitans_TextReport:
	.string "{STR_VAR_1}\n{STR_VAR_2}\lSurvey rank: {STR_VAR_3}.\pEarned access survives demotion.$"
GleTitans_Approach::
	lockall
	faceplayer
	special GleTitansInfo
	compare VAR_RESULT, 2
	goto_if_eq GleExclusive_AlreadyCaught
	compare VAR_RESULT, 4
	goto_if_eq GleTitans_Sealed
	compare VAR_RESULT, 3
	goto_if_eq GleTitans_Puzzle
	compare VAR_RESULT, 1
	goto_if_ne GleExclusive_Locked
	goto GleTitans_Save
GleTitans_Puzzle::
	special GleTitansClue
	msgbox gStringVar4, MSGBOX_DEFAULT
	closemessage
	special GleTitansSealMenu
	special GleTitansSolve
	compare VAR_RESULT, 1
	goto_if_ne GleTitans_Wrong
	msgbox GleTitans_TextSolved, MSGBOX_DEFAULT
GleTitans_Save::
	special GleTitansInfo
	msgbox GleExclusive_TextSave, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleTitans_Confirm
	call Common_EventScript_SaveGame
GleTitans_Confirm::
	special GleTitansInfo
	msgbox GleExclusive_TextConfirm, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleExclusive_Done
	closemessage
	special GleTitansStart
	special GleTitansFinish
	compare VAR_RESULT, 1
	goto_if_eq GleTitans_Caught
	msgbox GleExclusive_TextRetry, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleTitans_Caught::
	removeobject 251
	msgbox GleTitans_TextCaught, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleTitans_Sealed::
	msgbox GleTitans_TextSealed, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleTitans_Wrong::
	msgbox GleTitans_TextWrong, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleTitans_TextSolved:
	.string "The seal glows and opens!\nIts meaning is permanently recorded.$"
GleTitans_TextWrong:
	.string "The seal remains silent.\nStudy the inscription and try again.$"
GleTitans_TextSealed:
	.string "Five ancient powers bind this seal.\nDiscover Monean, Liptoo, Weepth,\lDilford and Scufib chambers.\pCaptures are not required.\nEarly discoveries are remembered.$"
GleTitans_TextCaught:
	.string "This titan's capture is recorded!\nYour partner is in your party or PC.\lThe other titans remain independent.$"

GleResearch_Regigigas::
	special GleRegigigasResearch
	compare VAR_RESULT, 1
	goto_if_eq GleResearch_RegigigasDone
	compare VAR_RESULT, 2
	goto_if_eq GleResearch_RegigigasKnown
	msgbox GleResearch_TextRegigigasCare, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_RegigigasDone::
	playfanfare MUS_EVOLVED
	msgbox GleResearch_TextRegigigasDone, MSGBOX_DEFAULT
	waitfanfare
	goto GleResearch_Notebook
GleResearch_RegigigasKnown::
	msgbox GleResearch_TextRegigigasKnown, MSGBOX_DEFAULT
	goto GleResearch_Notebook
GleResearch_TextRegigigasCare:
	.string "Survey the five outer chambers\nand catch Rixy's REGIGIGAS.\pBring it in your party or PC.\nWith the complete inscriptions,\lwe can overcome its SLOW START.$"
GleResearch_TextRegigigasDone:
	.string "The five inscriptions reveal how\nto awaken your REGIGIGAS!\pIts SLOW START is permanently\nreplaced by CLEAR BODY.\lOther REGIGIGAS remain unchanged.$"
GleResearch_TextRegigigasKnown:
	.string "Your REGIGIGAS's awakening is\ncomplete. Its CLEAR BODY endures\lwherever your journey leads.$"

GleResearch_Field::
	special GleFieldNotebook
	msgbox gStringVar4, MSGBOX_DEFAULT
GleField_NotebookMenu::
	closemessage
	special GleFieldMenu
	special GleFieldReport
	compare VAR_RESULT, 0
	goto_if_eq GleResearch_Notebook
	msgbox gStringVar4, MSGBOX_DEFAULT
	goto GleField_NotebookMenu
GleField_Approach::
	lockall
	faceplayer
	special GleFieldInfo
	compare VAR_RESULT, 2
	goto_if_eq GleExclusive_AlreadyCaught
	compare VAR_RESULT, 4
	goto_if_eq GleField_MoreEvidence
	compare VAR_RESULT, 3
	goto_if_eq GleField_Puzzle
	compare VAR_RESULT, 1
	goto_if_ne GleExclusive_Locked
	goto GleField_Save
GleField_Puzzle::
	special GleFieldClue
	msgbox gStringVar4, MSGBOX_DEFAULT
	closemessage
	special GleFieldSealMenu
	special GleFieldSolve
	compare VAR_RESULT, 1
	goto_if_ne GleTitans_Wrong
	msgbox GleTitans_TextSolved, MSGBOX_DEFAULT
GleField_Save::
	special GleFieldInfo
	msgbox GleExclusive_TextSave, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleField_Confirm
	call Common_EventScript_SaveGame
GleField_Confirm::
	special GleFieldInfo
	msgbox GleExclusive_TextConfirm, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleExclusive_Done
	closemessage
	special GleFieldStart
	special GleFieldFinish
	compare VAR_RESULT, 1
	goto_if_eq GleField_Caught
	msgbox GleExclusive_TextRetry, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleField_Caught::
	removeobject VAR_LAST_TALKED
	msgbox GleField_TextCaught, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleField_MoreEvidence::
	msgbox GleField_TextEvidence, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleField_TextCaught:
	.string "This encounter is recorded!\nYour partner is in your party or PC.\lOther encounters stay independent.$"
GleField_TextEvidence:
	.string "A poisonous chain holds firm.\nTrace the pact's other sites on\lROUTE 15, ROUTE 8 and ROUTE 18.\pDiscoveries are enough.\nNo other capture is required.$"
