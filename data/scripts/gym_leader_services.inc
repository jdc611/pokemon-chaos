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
	.string "Settle {STR_VAR_1}\nfor ${STR_VAR_2}?\pThis replaces the selected slot.\nOther slots stay as they are.$"
GleSafari_TextPlaced:
	.string "The habitat is ready!\nYour selected species now lives\lin the SAFARI ZONE.$"
GleSafari_TextFailed:
	.string "That species is already settled,\nor the funds are insufficient.\lYour wildlife is unchanged.$"
