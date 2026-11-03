#include "global.h"
#include "gym_leader.h"
#include "gym_leader_rocket.h"
#include "battle.h"
#include "event_data.h"
#include "malloc.h"
#include "money.h"
#include "overworld.h"
#include "pokemon.h"
#include "string_util.h"
#include "constants/maps.h"
#include "constants/battle_ai.h"
static bool32 HomeTown(void)
{
    static const u16 towns[]={MAP_SAFFRON_CITY,MAP_CELADON_CITY,MAP_FUCHSIA_CITY};
    u16 map=(u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);
    return IsGymLeaderRun() && map==towns[GymLeaderState()->town];
}
bool32 GymLeaderRocketBattleAvailable(void)
{
    return HomeTown() && GymLeaderState()->rocket.stage==1 && GymLeaderState()->rocket.remaining;
}
void GleRocketInfo(void)
{
    struct GleRocketState *r=&GymLeaderState()->rocket;
    static const u8 *const names[]={COMPOUND_STRING("STREET RAID"),COMPOUND_STRING("HABITAT RAID"),COMPOUND_STRING("MUSEUM HEIST"),COMPOUND_STRING("GIOVANNI'S RAID")};
    gSpecialVar_Result=0;if(!HomeTown() || !r->stage)return;
    ConvertIntToDecimalStringN(gStringVar1,r->remaining,STR_CONV_MODE_LEFT_ALIGN,1);
    ConvertIntToDecimalStringN(gStringVar2,r->staffHelp,STR_CONV_MODE_LEFT_ALIGN,1);
    StringCopy(gStringVar3,names[r->kind<4?r->kind:0]);
    if(r->heist)StringCopy(gStringVar4,COMPOUND_STRING("A museum artifact was already taken.\nWinning the battles ends this raid,\lbut cannot undo that theft.\pKeep tracing Rocket's leadership.\nTheir commander may lead you\lback to the stolen collection."));
    else if(r->kind==3)StringCopy(gStringVar4,COMPOUND_STRING("GIOVANNI is behind these raids.\nDefeat his remaining guards,\lthen face him yourself.\pTurning him over can recover\nartifacts stolen in earlier heists.\lThe town's buildings remain intact."));
    else StringCopy(gStringVar4,COMPOUND_STRING("The incident persists until resolved.\nTown income is temporarily halved,\land reputation is under pressure.\pYour active Gym trainers have\nhelped reduce the battles needed.\lReserve staff do not take part."));
    gSpecialVar_Result=r->stage;
}
void GleRocketJournal(void)
{
    struct GleRocketState *r=&GymLeaderState()->rocket;
    ConvertIntToDecimalStringN(gStringVar1,r->stolen,STR_CONV_MODE_LEFT_ALIGN,5);
    ConvertIntToDecimalStringN(gStringVar2,r->recovered,STR_CONV_MODE_LEFT_ALIGN,5);
    ConvertIntToDecimalStringN(gStringVar3,r->cleared,STR_CONV_MODE_LEFT_ALIGN,5);
    StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("Town security\nIncidents resolved: {STR_VAR_3}\lMissing artifacts: {STR_VAR_1}\pRecovered artifacts: {STR_VAR_2}."));
    StringAppend(gStringVar4,r->stage?COMPOUND_STRING("\nAn active incident remains\lnear the Gym. Report there."):COMPOUND_STRING("\nThe town is currently secure.\lKeep supporting its residents."));
}
void GleRocketDecisionMenu(void)
{
    static const u8 *const choices[]={COMPOUND_STRING("Turn over to GLA"),COMPOUND_STRING("Take hush money"),COMPOUND_STRING("Decide later")};GymLeaderMenu(choices,3,FALSE);
}
void GleRocketResolve(void)
{
    u32 choice=gSpecialVar_Result,money;gSpecialVar_Result=GLE_INVALID;
    if(!HomeTown() || choice>1)return;
    money=GetMoney(&gSaveBlock1Ptr->money);gSpecialVar_Result=GleResolveRocket(GymLeaderState(),choice,&money);
    if(gSpecialVar_Result==GLE_OK)SetMoney(&gSaveBlock1Ptr->money,money);
}
void GleRocketRefresh(void){if(HomeTown())GymLeaderConfigureTown(&gMapHeader);}
bool32 GymLeaderPrepareRocketRoster(struct Trainer *trainer,struct TrainerMon *entries)
{
    struct GleState *s=GymLeaderState();u16 *pool;u32 count,i,j,attempt,level,seed;bool32 boss;
    static const u8 types[]={7,15,8,0};
    if(!GymLeaderRocketBattleAvailable())return FALSE;
    pool=Alloc(GLE_SPECIES_LIMIT*sizeof(*pool));if(pool==NULL)return FALSE;
    boss=s->rocket.kind==3 && s->rocket.remaining==1;
    memset(trainer,0,sizeof(*trainer));memset(entries,0,PARTY_SIZE*sizeof(*entries));
    trainer->trainerClass=boss?TRAINER_CLASS_BOSS_FRLG:TRAINER_CLASS_TEAM_ROCKET_FRLG;
    trainer->trainerPic=boss?TRAINER_PIC_LEADER_GIOVANNI_FRLG:TRAINER_PIC_ROCKET_GRUNT_M_FRLG;
    StringCopy(trainer->trainerName,boss?COMPOUND_STRING("GIOVANNI"):COMPOUND_STRING("ROCKET"));
    trainer->partySize=boss?6:min(6,2+s->rank/2);trainer->party=entries;
    trainer->aiFlags=AI_FLAG_CHECK_BAD_MOVE|AI_FLAG_CHECK_VIABILITY|AI_FLAG_TRY_TO_FAINT;
    level=max(5,GymLeaderLevelCap()-2);seed=GleMix(s->seed^s->rocket.serial^(s->rocket.remaining<<16));ZeroEnemyPartyMons();
    for(i=0;i<trainer->partySize;i++)
    {
        u32 type=boss && i==0?8:types[GleMix(seed^i)%4];enum Species species;u8 ability=GleMix(seed^i)&1;
        struct Pokemon *mon=&gParties[B_TRAINER_OPPONENT_A][i];
        count=GymLeaderBuildSpeciesPool(type,s->rank,GLE_STANDARD,FALSE,pool,GLE_SPECIES_LIMIT);
        if(!count || count>GLE_SPECIES_LIMIT){Free(pool);return FALSE;}
        species=pool[GleMix(seed^i)%count];
        for(attempt=0;attempt<count;attempt++)
        {
            species=pool[(GleMix(seed^i)%count+attempt)%count];for(j=0;j<i;j++)if(entries[j].species==species)break;if(j==i)break;
        }
        if(boss && i==0)species=level>=55?SPECIES_RHYPERIOR:level>=42?SPECIES_RHYDON:SPECIES_RHYHORN;
        CreateMonWithIVs(mon,species,level,GleMix(seed^i^0x524F434Bu),OTID_STRUCT_PRESET(seed),16+s->rank);
        if(gSpeciesInfo[species].abilities[ability]==ABILITY_NONE)ability=0;SetMonData(mon,MON_DATA_ABILITY_NUM,&ability);
        GiveMonInitialMoveset(mon);entries[i].species=species;entries[i].lvl=level;
    }
    gPartiesCount[B_TRAINER_OPPONENT_A]=trainer->partySize;Free(pool);return TRUE;
}
