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
	closemessage
	releaseall
	callnative StartFishingOldRodFromWaterMenu
	end

EventScript_FishGoodRod::
	closemessage
	releaseall
	callnative StartFishingGoodRodFromWaterMenu
	end

EventScript_FishSuperRod::
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
