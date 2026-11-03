GleExclusive_Seafoam::
	setvar VAR_0x8004, 0
	goto GleExclusive_Approach
GleExclusive_PowerPlant::
	setvar VAR_0x8004, 1
	goto GleExclusive_Approach
GleExclusive_Ember::
	setvar VAR_0x8004, 2
	goto GleExclusive_Approach
GleExclusive_Cave::
	setvar VAR_0x8004, 3
GleExclusive_Approach::
	lockall
	faceplayer
	special GleExclusiveInfo
	compare VAR_RESULT, 2
	goto_if_eq GleExclusive_AlreadyCaught
	compare VAR_RESULT, 1
	goto_if_ne GleExclusive_Locked
	msgbox GleExclusive_TextSave, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleExclusive_Confirm
	call Common_EventScript_SaveGame
GleExclusive_Confirm::
	special GleExclusiveRefresh
	msgbox GleExclusive_TextConfirm, MSGBOX_YESNO
	compare VAR_RESULT, NO
	goto_if_eq GleExclusive_Done
	closemessage
	special GleExclusiveStart
	special GleExclusiveFinish
	compare VAR_RESULT, 1
	goto_if_eq GleExclusive_Caught
	msgbox GleExclusive_TextRetry, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleExclusive_Caught::
	removeobject VAR_LAST_TALKED
	msgbox GleExclusive_TextCaught, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleExclusive_AlreadyCaught::
	msgbox GleExclusive_TextQuiet, MSGBOX_DEFAULT
	goto GleExclusive_Done
GleExclusive_Locked::
	msgbox GleExclusive_TextLocked, MSGBOX_DEFAULT
GleExclusive_Done::
	releaseall
	end
GleExclusive_TextSave:
	.string "A rare presence is waiting.\nWould you like to save before\lyou approach?$"
GleExclusive_TextConfirm:
	.string "{STR_VAR_1} / Lv.{STR_VAR_2}\nApproach this POKéMON?$"
GleExclusive_TextLocked:
	.string "This POKéMON watches from afar.\nReturn when your GYM reaches\lrank {STR_VAR_3}.$"
GleExclusive_TextRetry:
	.string "The POKéMON remains in its habitat.\nYou can return and try again.$"
GleExclusive_TextCaught:
	.string "Your rare partner is safe.\nThis encounter is now recorded\lin your GYM's research log.$"
GleExclusive_TextQuiet:
	.string "This habitat is quiet.\nYou already befriended its\lrare POKéMON.$"

GleExpedition_Guide::
	lock
	faceplayer
	msgbox GleExpedition_TextWelcome, MSGBOX_DEFAULT
GleExpedition_Choose::
	closemessage
	special GleExpeditionMenu
	compare VAR_RESULT, 4
	goto_if_ge GleExclusive_Done
	special GleExpeditionTravel
	compare VAR_RESULT, 1
	goto_if_eq GleExpedition_Travelled
	msgbox GleExpedition_TextLocked, MSGBOX_DEFAULT
	goto GleExpedition_Choose
GleExpedition_Travelled::
	waitstate
	release
	end
GleExpedition_TextWelcome:
	.string "The GLA sponsors field expeditions.\nTransport is free as your rank\lopens each destination.\pWe take you to the entrance.\nBring supplies and explore the\lhabitat to find its rare resident.$"
GleExpedition_TextLocked:
	.string "Transport to that habitat opens\nat GYM rank {STR_VAR_3}.\lKeep developing your GYM!$"
