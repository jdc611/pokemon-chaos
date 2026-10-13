OaksDraftRoom_MapScripts::
    .byte 0

OaksDraftRoom_Entrance::
    specialvar VAR_RESULT, IsOaksDraft
    goto_if_eq VAR_RESULT, FALSE, PalletTown_ProfessorOaksLab_EventScript_LeftSign
    lockall
    warp MAP_OAKS_DRAFT_ROOM, 6, 11
    waitstate
    releaseall
    end

OaksDraftRoom_Oak::
    lockall
    special OaksDraftGetRound
    goto_if_eq VAR_RESULT, 6, OaksDraftRoom_Report
    msgbox OaksDraftRoom_Text_Next
    releaseall
    end
OaksDraftRoom_Report::
    special OaksDraftOpenReport
    releaseall
    end
OaksDraftRoom_Rival::
    lockall
    msgbox OaksDraftRoom_Text_Rival
    releaseall
    end

OaksDraftRoom_Crate0::
    lockall
    special OaksDraftGetRound
    goto_if_ne VAR_RESULT, 0, OaksDraftRoom_Locked
    setvar VAR_0x8004, 0
    special OaksDraftOpenCrate
    goto_if_eq VAR_RESULT, 2, OaksDraftRoom_Cancel
    msgbox OaksDraftRoom_Text_RivalPick
    special OaksDraftGetRound
    goto_if_eq VAR_RESULT, 6, OaksDraftRoom_Finished
    releaseall
    end

OaksDraftRoom_Crate1::
    lockall
    special OaksDraftGetRound
    goto_if_ne VAR_RESULT, 1, OaksDraftRoom_Locked
    setvar VAR_0x8004, 1
    special OaksDraftOpenCrate
    goto_if_eq VAR_RESULT, 2, OaksDraftRoom_Cancel
    msgbox OaksDraftRoom_Text_RivalPick
    special OaksDraftGetRound
    goto_if_eq VAR_RESULT, 6, OaksDraftRoom_Finished
    releaseall
    end

OaksDraftRoom_Crate2::
    lockall
    special OaksDraftGetRound
    goto_if_ne VAR_RESULT, 2, OaksDraftRoom_Locked
    setvar VAR_0x8004, 2
    special OaksDraftOpenCrate
    goto_if_eq VAR_RESULT, 2, OaksDraftRoom_Cancel
    msgbox OaksDraftRoom_Text_RivalPick
    special OaksDraftGetRound
    goto_if_eq VAR_RESULT, 6, OaksDraftRoom_Finished
    releaseall
    end

OaksDraftRoom_Crate3::
    lockall
    special OaksDraftGetRound
    goto_if_ne VAR_RESULT, 3, OaksDraftRoom_Locked
    setvar VAR_0x8004, 3
    special OaksDraftOpenCrate
    goto_if_eq VAR_RESULT, 2, OaksDraftRoom_Cancel
    msgbox OaksDraftRoom_Text_RivalPick
    special OaksDraftGetRound
    goto_if_eq VAR_RESULT, 6, OaksDraftRoom_Finished
    releaseall
    end

OaksDraftRoom_Crate4::
    lockall
    special OaksDraftGetRound
    goto_if_ne VAR_RESULT, 4, OaksDraftRoom_Locked
    setvar VAR_0x8004, 4
    special OaksDraftOpenCrate
    goto_if_eq VAR_RESULT, 2, OaksDraftRoom_Cancel
    msgbox OaksDraftRoom_Text_RivalPick
    special OaksDraftGetRound
    goto_if_eq VAR_RESULT, 6, OaksDraftRoom_Finished
    releaseall
    end

OaksDraftRoom_Crate5::
    lockall
    special OaksDraftGetRound
    goto_if_ne VAR_RESULT, 5, OaksDraftRoom_Locked
    setvar VAR_0x8004, 5
    special OaksDraftOpenCrate
    goto_if_eq VAR_RESULT, 2, OaksDraftRoom_Cancel
    msgbox OaksDraftRoom_Text_RivalPick
    special OaksDraftGetRound
    goto_if_eq VAR_RESULT, 6, OaksDraftRoom_Finished
    releaseall
    end

OaksDraftRoom_Locked::
    msgbox OaksDraftRoom_Text_Locked
OaksDraftRoom_Cancel::
    releaseall
    end
OaksDraftRoom_Finished::
    setflag FLAG_SYS_POKEMON_GET
    setflag FLAG_PALLET_LADY_NOT_BLOCKING_SIGN
    setvar VAR_STARTER_MON, 0
    setvar VAR_MAP_SCENE_PALLET_TOWN_PROFESSOR_OAKS_LAB, 3
    msgbox OaksDraftRoom_Text_Finished
    special OaksDraftOpenReport
    warp MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB, 6, 7
    waitstate
    releaseall
    end
OaksDraftRoom_Text_Next:
    .string "Choose one friend from each crate.\nStart with the top-left crate.\pInspect their summaries first.\nYour confirmed choices are permanent.$"
OaksDraftRoom_Text_Rival:
    .string "I'll choose from the two you leave.\nLet's see what our choices become!$"
OaksDraftRoom_Text_RivalPick:
    .string "{RIVAL}: I'll take {STR_VAR_1},\nthe {STR_VAR_2}!\pOAK takes the remaining companion\ninto his care.$"
OaksDraftRoom_Text_Locked:
    .string "This crate is sealed or already chosen.\nFinish the current crate first.$"
OaksDraftRoom_Text_Finished:
    .string "OAK: Six companions, six new beginnings.\nHere is your original Draft Report.\pYou can revisit this room to compare\nyour choices with your growing team.$"

OaksDraftRoom_Exit::
    lockall
    warp MAP_PALLET_TOWN_PROFESSOR_OAKS_LAB, 6, 6
    waitstate
    releaseall
    end
