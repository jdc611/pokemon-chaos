GleLeagueHall_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, GleHall_OnTransition
	map_script MAP_SCRIPT_ON_FRAME_TABLE, GleHall_OnFrame
	.byte 0
GleHall_OnTransition::
	special GleOpeningStage
	setvar VAR_TEMP_0, 0
	compare VAR_RESULT, 4
	call_if_eq GleHall_SetIntro
	end
GleHall_SetIntro::
	setvar VAR_TEMP_0, 1
	return
GleHall_OnFrame::
	map_script_2 VAR_TEMP_0, 1, GleHall_Opening
	.2byte 0
