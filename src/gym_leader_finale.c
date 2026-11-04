#include "global.h"
#include "gym_leader.h"
#include "gym_leader_finale.h"
#include "gym_leader_script.h"
#include "battle.h"
#include "credits.h"
#include "event_data.h"
#include "fieldmap.h"
#include "main.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "save.h"
#include "string_util.h"
#include "constants/battle_ai.h"
#include "constants/flags.h"
#include "constants/maps.h"

// Bit 223 follows the eight championship licenses and three display choices.
bool32 GymLeaderFinalOakComplete(void){return IsGymLeaderRun()&&!!(GymLeaderState()->uniqueObtained[27]&128);}
bool32 GymLeaderFinalOakEligible(void){return IsGymLeaderRun()&&GymLeaderState()->openingStage==5&&GymLeaderState()->rank==8&&!GymLeaderState()->pendingInspection&&GymLeaderState()->championshipWins&&!GymLeaderFinalOakComplete();}
bool32 GymLeaderPrepareFinalOakRoster(struct Trainer *trainer,struct TrainerMon *entries)
{
    static const u16 species[]={SPECIES_TAUROS,SPECIES_EXEGGUTOR,SPECIES_ARCANINE,SPECIES_BLASTOISE,SPECIES_VENUSAUR,SPECIES_CHARIZARD};
    static const u16 moves[][4]={{MOVE_BODY_SLAM,MOVE_ZEN_HEADBUTT,MOVE_EARTHQUAKE,MOVE_IRON_HEAD},{MOVE_PSYCHIC,MOVE_ENERGY_BALL,MOVE_SLEEP_POWDER,MOVE_SUNNY_DAY},{MOVE_FLARE_BLITZ,MOVE_EXTREME_SPEED,MOVE_WILD_CHARGE,MOVE_WILL_O_WISP},{MOVE_SURF,MOVE_ICE_BEAM,MOVE_DARK_PULSE,MOVE_SHELL_SMASH},{MOVE_GIGA_DRAIN,MOVE_SLUDGE_BOMB,MOVE_EARTH_POWER,MOVE_SLEEP_POWDER},{MOVE_FLAMETHROWER,MOVE_AIR_SLASH,MOVE_DRAGON_PULSE,MOVE_ROOST}};
    static const u16 items[]={ITEM_LIFE_ORB,ITEM_SITRUS_BERRY,ITEM_LEFTOVERS,ITEM_WHITE_HERB,ITEM_BLACK_SLUDGE,ITEM_HEAVY_DUTY_BOOTS};
    u32 i,j;u8 ev=252,zero=0;u32 seed;
    if(!GymLeaderFinalOakEligible())return FALSE;
    memset(trainer,0,sizeof(*trainer));memset(entries,0,PARTY_SIZE*sizeof(*entries));
    trainer->trainerClass=TRAINER_CLASS_PKMN_PROF_FRLG;trainer->trainerPic=TRAINER_PIC_PROFESSOR_OAK_FRLG;trainer->partySize=6;trainer->party=entries;
    trainer->aiFlags=AI_FLAG_CHECK_BAD_MOVE|AI_FLAG_CHECK_VIABILITY|AI_FLAG_TRY_TO_FAINT;StringCopy(trainer->trainerName,COMPOUND_STRING("OAK"));
    ZeroEnemyPartyMons();seed=GleMix(GymLeaderState()->seed^0x0A8F1A1u);
    for(i=0;i<6;i++){
        struct Pokemon *mon=&gParties[B_TRAINER_OPPONENT_A][i];u16 item=items[i];
        CreateMonWithIVs(mon,species[i],GymLeaderLevelCap(),GleMix(seed^i),OTID_STRUCT_PRESET(seed),31);
        SetMonData(mon,MON_DATA_ABILITY_NUM,&zero);SetMonData(mon,MON_DATA_HELD_ITEM,&item);
        SetMonData(mon,i==0||i==2?MON_DATA_ATK_EV:MON_DATA_SPATK_EV,&ev);
        SetMonData(mon,i==1||i==4?MON_DATA_HP_EV:MON_DATA_SPEED_EV,&ev);
        for(j=0;j<4;j++)SetMonMoveSlot(mon,moves[i][j],j);
        CalculateMonStats(mon);entries[i].species=species[i];entries[i].lvl=GymLeaderLevelCap();
    }
    gPartiesCount[B_TRAINER_OPPONENT_A]=6;return TRUE;
}
void GymLeaderSettleFinalOak(bool32 won)
{
    struct GleState *s=GymLeaderState();if(!won||!GymLeaderFinalOakEligible())return;
    s->uniqueObtained[27]|=128;FlagSet(FLAG_BADGE08_GET);FlagSet(FLAG_SYS_GAME_CLEAR);
    AddMoney(&gSaveBlock1Ptr->money,20000);s->reputation=min(65535,s->reputation+50);
}
void GleFinalOakInfo(void)
{
    gSpecialVar_Result=0;if(!IsGymLeaderRun())return;
    gSpecialVar_Result=GymLeaderFinalOakComplete()?2:GymLeaderFinalOakEligible()?1:0;
    StringCopy(gStringVar1,GymLeaderState()->honor>=100?COMPOUND_STRING("Your honorable work has earned trust."):GymLeaderState()->honor<=-100?COMPOUND_STRING("Your reputation leaves hard questions."):COMPOUND_STRING("Your choices have shaped your town."));
}
void GleFinalOakCredits(void)
{
    gSpecialVar_Result=0;if(!GymLeaderFinalOakComplete()){GymLeaderResumeScriptNextFrame();return;}
    // Never call the ordinary Hall of Fame: its sectors hold Kanto archives.
    // Save first because the native credits return through a soft reset.
    SaveMapView();
    if(TrySavingData(SAVE_NORMAL)!=SAVE_STATUS_OK){GymLeaderResumeScriptNextFrame();return;}
    gSpecialVar_Result=1;CleanupOverworldWindowsAndTilemaps();SetMainCallback2(CB2_StartCreditsSequence);
}
