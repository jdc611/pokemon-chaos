GleRecruit_AfterBattle::
	special GleRecruitInfo
	compare VAR_RESULT, 0
	goto_if_eq GleRecruit_Refusal
	compare VAR_RESULT, 2
	goto_if_eq GleRecruit_ReturnOffer
	msgbox GleRecruit_TextOffer, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleRecruit_Return
	goto GleRecruit_Hire
GleRecruit_ReturnOffer::
	msgbox GleRecruit_TextReturn, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleRecruit_Return
GleRecruit_Hire::
	special GleRecruitHire
	compare VAR_RESULT, 0
	goto_if_eq GleRecruit_Hired
	compare VAR_RESULT, 2
	goto_if_eq GleRecruit_Full
	compare VAR_RESULT, 4
	goto_if_eq GleRecruit_NoMoney
GleRecruit_Refusal::
	msgbox GleRecruit_TextRefuse, MSGBOX_DEFAULT
	return
GleRecruit_Full::
	msgbox GleRecruit_TextFull, MSGBOX_DEFAULT
	return
GleRecruit_NoMoney::
	msgbox GleRecruit_TextNoMoney, MSGBOX_DEFAULT
	return
GleRecruit_Hired::
	playfanfare MUS_EVOLVED
	msgbox GleRecruit_TextHired, MSGBOX_DEFAULT
	waitfanfare
GleRecruit_Return::
	return
GleStaff_Office::
	lockall
	faceplayer
	special GleStaffOfficeInfo
	msgbox GleStaff_TextWelcome, MSGBOX_DEFAULT
GleStaff_Menu::
	closemessage
	special GleStaffMenu
	special GleStaffSelect
	compare VAR_RESULT, 0
	goto_if_ne GleExclusive_Done
	goto GleStaff_Selected
GleStaff_Member::
	lockall
	faceplayer
	special GleStaffMemberSelect
	compare VAR_RESULT, 0
	goto_if_ne GleExclusive_Done
GleStaff_Selected::
	special GleStaffInfo
	msgbox GleStaff_TextRecord, MSGBOX_DEFAULT
	special GleStaffDevelopment
	msgbox GleStaff_TextDevelopment, MSGBOX_DEFAULT
GleStaff_ActionMenu::
	closemessage
	special GleStaffActions
	special GleStaffAction
	compare VAR_RESULT, 20
	goto_if_eq GleStaff_Dismiss
	compare VAR_RESULT, 21
	goto_if_eq GleStaff_Practice
	compare VAR_RESULT, 22
	goto_if_eq GleStaff_Summary
	compare VAR_RESULT, 0
	goto_if_eq GleStaff_Changed
	compare VAR_RESULT, 1
	goto_if_eq GleStaff_Menu
	msgbox GleStaff_TextUnavailable, MSGBOX_DEFAULT
	goto GleStaff_ActionMenu
GleStaff_Changed::
	msgbox GleStaff_TextChanged, MSGBOX_DEFAULT
	goto GleStaff_Selected
GleStaff_Summary::
	closemessage
	special GleStaffSummary
	goto GleStaff_Selected
GleStaff_Practice::
	msgbox GleStaff_TextPractice, MSGBOX_DEFAULT
	closemessage
	special GymLeaderStartStaffPractice
	compare VAR_RESULT, 10
	goto_if_ge GleStaff_PracticeFailed
	msgbox GleServices_TextTrainingDone, MSGBOX_DEFAULT
	goto GleStaff_Selected
GleStaff_PracticeFailed::
	msgbox GleServices_TextTrainingUnavailable, MSGBOX_DEFAULT
	goto GleStaff_ActionMenu
GleStaff_Dismiss::
	msgbox GleStaff_TextDismiss, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleStaff_ActionMenu
	special GleStaffDismiss
	msgbox GleStaff_TextDismissed, MSGBOX_DEFAULT
	goto GleStaff_Menu
GleRecruit_TextOffer:
	.string "{STR_VAR_1}: {STR_VAR_2} stars / ¥{STR_VAR_3}\nInvite this trainer to your staff?$"
GleRecruit_TextReturn:
	.string "This is a former teammate.\nAsk them to return to your GYM?$"
GleRecruit_TextRefuse:
	.string "I'm not ready to leave my place.\nA stronger GYM or a change in\lstanding may change my mind.$"
GleRecruit_TextFull:
	.string "Your current staff roster is full.\nRaise your rank or dismiss a record\lat your GYM's staff office.$"
GleRecruit_TextNoMoney:
	.string "Come back when you have enough\nsigning funds.$"
GleRecruit_TextHired:
	.string "Welcome to the team!\nVisit your GYM's staff office to\lassign trainers or keep reserves.\pI'll report in when you next visit.\nYour traveling POKéMON are yours;\lI develop my own team.$"
GleStaff_TextWelcome:
	.string "Staff office: {STR_VAR_1}/{STR_VAR_2} records.\nActive positions: {STR_VAR_3}\pActive staff face ordinary GYM\nchallengers. Their results never\lchange your official GYM rank.\pImportant challengers reach you.\nReserves keep their development\land records. Review your staff.$"
GleStaff_TextRecord:
	.string "{STR_VAR_1}\n{STR_VAR_2}\l{STR_VAR_3}$"
GleStaff_TextDevelopment:
	.string "Experience: {STR_VAR_1} staff battles.\nWarnings: {STR_VAR_2}/3\l{STR_VAR_3}\pStaff develop their own teams as\nyour GYM grows. Repeated poor\lresults may lead to departures.$"
GleStaff_TextChanged:
	.string "The staff record is updated.\nTrainers report to their positions\lon your next visit.$"
GleStaff_TextUnavailable:
	.string "That request cannot go ahead.\nCheck active capacity, standing\land your GYM's recovery.$"
GleStaff_TextPractice:
	.string "Use your registered GYM team for\npractice. Teams follow your current\lrank cap and size allowance.\pPOKéMON gain experience.\nPractice awards no money, GYM XP\lor official defense record.$"
GleStaff_TextDismiss:
	.string "Dismiss this trainer's record?\nTheir staff history will be erased.\lYou may need to persuade them again.$"
GleStaff_TextDismissed:
	.string "The staff record was released.$"
