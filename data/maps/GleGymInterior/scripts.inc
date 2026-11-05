GleGymInterior_MapScripts::
	map_script MAP_SCRIPT_ON_TRANSITION, GleGym_OnTransition
	map_script MAP_SCRIPT_ON_LOAD, GleGym_OnLoad
	map_script MAP_SCRIPT_ON_FRAME_TABLE, GleGym_OnFrame
	.byte 0
GleGym_OnTransition::
	special GleGymPrepare
	special GleOpeningStage
	setvar VAR_TEMP_0, 0
	compare VAR_RESULT, 1
	call_if_eq GleGym_SetIntro
	compare VAR_RESULT, 2
	call_if_eq GleGym_SetIntro
	compare VAR_RESULT, 3
	call_if_eq GleGym_SetIntro
	end
GleGym_SetIntro::
	setvar VAR_TEMP_0, 1
	return
GleGym_OnFrame::
	map_script_2 VAR_TEMP_0, 1, GleGym_Opening
	.2byte 0

GleGym_OnLoad::
	special GleGymDesignOwnerAccess
	end
