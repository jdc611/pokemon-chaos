#include "global.h"
#include "gym_leader.h"
#include "gym_leader_script.h"
#include "constants/characters.h"
#include "gym_leader_colleagues.h"
#include "gym_leader_competitions_native.h"
#include "battle.h"
#include "event_data.h"
#include "malloc.h"
#include "hall_of_fame.h"
#include "hall_of_fame_frlg.h"
#include "main.h"
#include "overworld.h"
#include "palette.h"
#include "task.h"
#include "constants/rgb.h"
#include "money.h"
#include "item.h"
#include "move.h"
#include "pokemon.h"
#include "save.h"
#include "string_util.h"

#define ARCHIVE_MAGIC 0x474c4348u
struct Archive {u32 magic,seed,generation;u16 count,reserved;u8 teams[32][112];u8 padding[368];};
STATIC_ASSERT(sizeof(struct Archive)==SECTOR_DATA_SIZE,GleArchiveSectorSize);
static const u8 *const sEventNames[]={COMPOUND_STRING("YOU"),COMPOUND_STRING("RIVAL"),COMPOUND_STRING("BROCK"),COMPOUND_STRING("MISTY"),COMPOUND_STRING("LT. SURGE"),COMPOUND_STRING("ERIKA"),COMPOUND_STRING("KOGA"),COMPOUND_STRING("SABRINA"),COMPOUND_STRING("BLAINE")};
static bool32 Roster(u32 id,struct Trainer *trainer,struct TrainerMon *entries){if(id==1)return GymLeaderPrepareRivalRoster(trainer,entries);return id>=2&&id<=8&&GymLeaderPrepareEventColleague(id-2,trainer,entries);}
bool32 GymLeaderPrepareCompetitionRoster(struct Trainer *trainer,struct TrainerMon *entries){int id=GleCompetitionOpponent(GymLeaderState());return id>0&&Roster(id,trainer,entries);}
static void Capture(u32 winner)
{
    struct GleCompetitionState *c=&GymLeaderState()->competitions;u32 i,pid;u16 species;u8 *p=c->pendingTeam;
    memset(p,0,112);p[0]=c->count;p[1]=c->count>>8;p[2]=winner;p[3]=gPartiesCount[winner?B_TRAINER_OPPONENT_A:B_TRAINER_PLAYER];
    for(i=0;i<p[3]&&i<6;i++){
        struct Pokemon *mon=&gParties[winner?B_TRAINER_OPPONENT_A:B_TRAINER_PLAYER][i];u8 *q=p+4+i*18;
        pid=GetMonData(mon,MON_DATA_PERSONALITY);species=GetMonData(mon,MON_DATA_SPECIES);
        memcpy(q,&pid,4);memcpy(q+4,&species,2);q[6]=GetMonData(mon,MON_DATA_LEVEL);q[7]=GetMonData(mon,MON_DATA_IS_SHINY);
        {u8 name[POKEMON_NAME_LENGTH+1];GetMonData(mon,MON_DATA_NICKNAME,name);memcpy(q+8,name,10);}
    }
}
void GymLeaderCaptureCompetitionTeam(void)
{
    struct GleCompetitionState *c=&GymLeaderState()->competitions;
    if((c->current.kindRank&1)&&GleEventWinner(&c->current,4)!=15&&GleEventWinner(&c->current,5)!=15&&GleEventWinner(&c->current,6)==15)Capture(0);
}
void GymLeaderSettleCompetition(bool32 won){u32 money=GetMoney(&gSaveBlock1Ptr->money);GleCompetitionResult(GymLeaderState(),won,&money);SetMoney(&gSaveBlock1Ptr->money,money);}
void GymLeaderCompetitionArchive(void)
{
    struct GleState *s=GymLeaderState();struct GleCompetitionState *c=&s->competitions;struct Archive *a,*b,*best;u32 validA,validB,bank,winner,money;
    struct Trainer trainer;struct TrainerMon entries[6];
    if(!IsGymLeaderRun()||c->stage!=3||c->archiveCount>=32)return;
    winner=GleEventWinner(&c->current,6);
    if(winner){if(!Roster(winner,&trainer,entries))return;Capture(winner);}
    if(c->pendingTeam[3]==0||c->pendingTeam[2]!=winner)return;
    a=AllocZeroed(sizeof(*a));b=AllocZeroed(sizeof(*b));if(!a||!b){Free(a);Free(b);return;}
    validA=GleReadArchiveSector(0,(u8 *)a)==SAVE_STATUS_OK&&a->magic==ARCHIVE_MAGIC&&a->seed==s->seed&&a->count<=32;
    validB=GleReadArchiveSector(1,(u8 *)b)==SAVE_STATUS_OK&&b->magic==ARCHIVE_MAGIC&&b->seed==s->seed&&b->count<=32;
    bank=validB&&(!validA||b->generation>a->generation)?1:0;best=bank?b:a;
    if((!validA&&!validB)&&c->archiveCount){Free(a);Free(b);return;}
    if(!validA&&!validB){memset(best,0,sizeof(*best));best->magic=ARCHIVE_MAGIC;best->seed=s->seed;}
    if(best->count<c->archiveCount){Free(a);Free(b);return;}
    best->count=c->archiveCount+1;best->generation++;memcpy(best->teams[c->archiveCount],c->pendingTeam,112);
    if(GleWriteArchiveSector(bank^1,(u8 *)best)==SAVE_STATUS_OK){money=GetMoney(&gSaveBlock1Ptr->money);GleCompetitionFinalize(s,&money);SetMoney(&gSaveBlock1Ptr->money,money);}
    Free(a);Free(b);
}

static EWRAM_DATA u16 sHistoryCursor=0;
void GleCompetitionMenu(void){static const u8 *const names[]={COMPOUND_STRING("Event notice"),COMPOUND_STRING("Enter event"),COMPOUND_STRING("Next match"),COMPOUND_STRING("Results archive"),COMPOUND_STRING("Place wager"),COMPOUND_STRING("Championship Hall"),COMPOUND_STRING("Champion prize"),COMPOUND_STRING("Trophy display"),COMPOUND_STRING("Back")};GymLeaderCompetitionArchive();GymLeaderMenu(names,9,FALSE);}
void GleCompetitionInfo(void){
    struct GleCompetitionState *c=&GymLeaderState()->competitions;int opponent=GleCompetitionOpponent(GymLeaderState());
    if(c->stage==0){StringCopy(gStringVar4,c->count>=GLE_EVENT_HISTORY||c->archiveCount>=32?COMPOUND_STRING("The permanent archive is full.\nEarlier records remain protected.\lNo records will be replaced."):COMPOUND_STRING("No event is currently announced.\nThe GLA schedules events during\lyour travels. Check back later."));return;}
    StringCopy(gStringVar4,c->current.kindRank&1?COMPOUND_STRING("KANTO CHAMPIONSHIP"):COMPOUND_STRING("REGIONAL CUP"));
    StringAppend(gStringVar4,c->stage==1?COMPOUND_STRING("\nEntries are open."):c->stage==3?COMPOUND_STRING("\nArchiving the winning team."):COMPOUND_STRING("\nThe bracket is underway."));
    StringAppend(gStringVar4,c->joined?COMPOUND_STRING("\lYour entry is confirmed."):GleCompetitionEligible(GymLeaderState())?COMPOUND_STRING("\lYou may enter before it starts."):COMPOUND_STRING("\lRegional: Rank 2. Kanto: Rank 6\nand a previous Regional win."));
    if(opponent>0){StringAppend(gStringVar4,COMPOUND_STRING("\pYour next opponent: "));StringAppend(gStringVar4,sEventNames[opponent]);}
    if(c->stage==1)StringAppend(gStringVar4,COMPOUND_STRING("\pEvents proceed without you.\nAfter entering, your matches\lwait until you are ready."));
}
void GleCompetitionEnter(void){gSpecialVar_Result=GleCompetitionJoin(GymLeaderState());}
void GleCompetitionHistoryStart(void){sHistoryCursor=GymLeaderState()->competitions.count;gSpecialVar_Result=sHistoryCursor!=0;if(sHistoryCursor)sHistoryCursor--;}
void GleCompetitionHistoryInfo(void){
    struct GleCompetitionState *c=&GymLeaderState()->competitions;struct GleEventRecord *r;u32 i;u8 n[6];
    if(sHistoryCursor>=c->count){StringCopy(gStringVar4,COMPOUND_STRING("No completed events yet."));return;}r=&c->history[sHistoryCursor];
    StringCopy(gStringVar4,r->kindRank&1?COMPOUND_STRING("KANTO No. "):COMPOUND_STRING("REGIONAL No. "));ConvertIntToDecimalStringN(n,sHistoryCursor+1,STR_CONV_MODE_LEFT_ALIGN,3);StringAppend(gStringVar4,n);
    for(i=0;i<7;i++){StringAppend(gStringVar4,i?COMPOUND_STRING("\p"):COMPOUND_STRING("\n"));StringAppend(gStringVar4,i<4?COMPOUND_STRING("QF "):i<6?COMPOUND_STRING("SF "):COMPOUND_STRING("FINAL "));StringAppend(gStringVar4,sEventNames[i<4?GleEventEntrant(r,i*2):GleEventWinner(r,(i-4)*2)]);StringAppend(gStringVar4,COMPOUND_STRING(" / "));StringAppend(gStringVar4,sEventNames[i<4?GleEventEntrant(r,i*2+1):GleEventWinner(r,(i-4)*2+1)]);StringAppend(gStringVar4,COMPOUND_STRING("\nWinner: "));StringAppend(gStringVar4,sEventNames[GleEventWinner(r,i)]);}
}
void GleCompetitionHistoryMenu(void){static const u8 *const names[]={COMPOUND_STRING("Older event"),COMPOUND_STRING("Newer event"),COMPOUND_STRING("Back")};GymLeaderMenu(names,3,FALSE);}
void GleCompetitionHistoryMove(void){u32 choice=gSpecialVar_Result;if(choice==0&&sHistoryCursor)sHistoryCursor--;if(choice==1&&sHistoryCursor+1<GymLeaderState()->competitions.count)sHistoryCursor++;}
void GleCompetitionBetMenu(void){struct GleCompetitionState *c=&GymLeaderState()->competitions;const u8 *names[9];u32 i;if(c->stage!=1||c->wager){gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();return;}for(i=0;i<8;i++)names[i]=sEventNames[GleEventEntrant(&c->current,i)];names[8]=COMPOUND_STRING("Back");GymLeaderMenu(names,9,FALSE);}
void GleCompetitionPlaceBet(void){u32 choice=gSpecialVar_Result,money=GetMoney(&gSaveBlock1Ptr->money);gSpecialVar_Result=GLE_INVALID;if(choice<8)gSpecialVar_Result=GleCompetitionBet(GymLeaderState(),GleEventEntrant(&GymLeaderState()->competitions.current,choice),500,&money);SetMoney(&gSaveBlock1Ptr->money,money);}
static EWRAM_DATA u8 sHallCursor=0;
void GleCompetitionHallStart(void){sHallCursor=GymLeaderState()->competitions.archiveCount;if(sHallCursor)sHallCursor--;}
void GleCompetitionHallMove(void){u32 choice=gSpecialVar_Result;if(choice==0&&sHallCursor)sHallCursor--;if(choice==1&&sHallCursor+1<GymLeaderState()->competitions.archiveCount)sHallCursor++;}
void GleCompetitionHallInfo(void){
    struct GleState *s=GymLeaderState();struct Archive *a,*b,*best;bool32 va,vb;u32 i;u8 *p,n[12],level[4];
    StringCopy(gStringVar4,COMPOUND_STRING("The Championship Hall is empty.\nFuture Kanto winning teams\lwill be recorded here."));if(!s->competitions.archiveCount)return;
    a=AllocZeroed(sizeof(*a));b=AllocZeroed(sizeof(*b));if(!a||!b){Free(a);Free(b);return;}
    va=GleReadArchiveSector(0,(u8 *)a)==SAVE_STATUS_OK&&a->magic==ARCHIVE_MAGIC&&a->seed==s->seed&&a->count>=s->competitions.archiveCount&&a->count<=32;
    vb=GleReadArchiveSector(1,(u8 *)b)==SAVE_STATUS_OK&&b->magic==ARCHIVE_MAGIC&&b->seed==s->seed&&b->count>=s->competitions.archiveCount&&b->count<=32;
    if(!va&&!vb){StringCopy(gStringVar4,COMPOUND_STRING("The team archive could not be read.\nExisting history is protected.\lPlease retain your save backup."));Free(a);Free(b);return;}
    best=vb&&(!va||b->generation>a->generation)?b:a;p=best->teams[sHallCursor];
    StringCopy(gStringVar4,COMPOUND_STRING("KANTO CHAMPION: "));StringAppend(gStringVar4,sEventNames[p[2]<=8?p[2]:0]);
    for(i=0;i<p[3]&&i<6;i++){u8 *q=p+4+i*18;u16 species;memcpy(&species,q+4,2);memcpy(n,q+8,10);n[10]=EOS;ConvertIntToDecimalStringN(level,q[6],STR_CONV_MODE_LEFT_ALIGN,3);StringAppend(gStringVar4,COMPOUND_STRING("\p"));StringAppend(gStringVar4,n);StringAppend(gStringVar4,COMPOUND_STRING(" Lv. "));StringAppend(gStringVar4,level);StringAppend(gStringVar4,COMPOUND_STRING("\n"));StringAppend(gStringVar4,GetSpeciesName(species));if(q[7])StringAppend(gStringVar4,COMPOUND_STRING(" (Shiny)"));}
    Free(a);Free(b);
}

static EWRAM_DATA struct HallofFameTeam *sHallPreview=NULL;
static bool32 CopyHallTeam(struct HallofFameTeam *team)
{
    struct GleState *s=GymLeaderState();struct Archive *a,*b,*best;
    bool32 va,vb,ok=FALSE;u8 *p;u32 i;
    if(!IsGymLeaderRun() || sHallCursor>=s->competitions.archiveCount)return FALSE;
    a=AllocZeroed(sizeof(*a));b=AllocZeroed(sizeof(*b));if(!a||!b){Free(a);Free(b);return FALSE;}
    va=GleReadArchiveSector(0,(u8 *)a)==SAVE_STATUS_OK&&a->magic==ARCHIVE_MAGIC&&a->seed==s->seed&&a->count>=s->competitions.archiveCount&&a->count<=32;
    vb=GleReadArchiveSector(1,(u8 *)b)==SAVE_STATUS_OK&&b->magic==ARCHIVE_MAGIC&&b->seed==s->seed&&b->count>=s->competitions.archiveCount&&b->count<=32;
    if(va||vb){
        best=vb&&(!va||b->generation>a->generation)?b:a;p=best->teams[sHallCursor];
        if(p[2]<=8&&p[3]>=1&&p[3]<=6){
            memset(team,0,sizeof(*team));ok=TRUE;
            for(i=0;i<p[3];i++){
                u8 *q=p+4+i*18;u16 species;memcpy(&species,q+4,2);
                if(!species||species>=NUM_SPECIES||!gSpeciesInfo[species].baseHP||q[6]<1||q[6]>100){ok=FALSE;break;}
                memcpy(&team->mon[i].personality,q,4);team->mon[i].species=species;team->mon[i].lvl=q[6];team->mon[i].isShiny=!!q[7];
                memcpy(team->mon[i].nickname,q+8,10);
            }
            StringCopy(gStringVar4,sEventNames[p[2]]);StringAppend(gStringVar4,COMPOUND_STRING(" / KANTO"));
        }
    }
    Free(a);Free(b);return ok;
}
static void Task_HallArtwork(u8 taskId)
{
    struct HallofFameTeam *team;
    if(gPaletteFade.active)return;
    team=sHallPreview;sHallPreview=NULL;ResetTasks();CleanupOverworldWindowsAndTilemaps();
    ShowArchivedHallOfFameTeam(team,gStringVar4,CB2_ReturnToFieldContinueScriptPlayMapMusic);
}
void GleCompetitionHallMenu(void){static const u8 *const names[]={COMPOUND_STRING("View team"),COMPOUND_STRING("Older event"),COMPOUND_STRING("Newer event"),COMPOUND_STRING("Back")};GymLeaderMenu(names,ARRAY_COUNT(names),FALSE);}
void GleCompetitionHallView(void)
{
    gSpecialVar_Result=0;
    sHallPreview=AllocZeroed(sizeof(*sHallPreview));
    if(!sHallPreview || !CopyHallTeam(sHallPreview)){TRY_FREE_AND_SET_NULL(sHallPreview);GymLeaderResumeScriptNextFrame();return;}
    gSpecialVar_Result=1;BeginNormalPaletteFade(PALETTES_ALL,0,0,16,RGB_BLACK);CreateTask(Task_HallArtwork,10);
}

// Append-only research bits 209..216 license eight reusable championship TMs;
// 217..222 hold three cosmetic display choices, two bits per safe position.
static const u16 sChampionMoves[]={MOVE_EARTHQUAKE,MOVE_ICE_BEAM,MOVE_THUNDERBOLT,MOVE_FLAMETHROWER,MOVE_PSYCHIC,MOVE_SHADOW_BALL,MOVE_ENERGY_BALL,MOVE_DAZZLING_GLEAM};
static EWRAM_DATA u8 sPrizeOffers[8]={0};
static EWRAM_DATA u8 sPrizeCount=0;
static bool32 Licensed(u32 i){u32 bit=209+i;return !!(GymLeaderState()->uniqueObtained[bit/8]&(1u<<(bit%8)));}
bool32 GymLeaderChampionTMReusable(u16 item){u32 i;if(!IsGymLeaderRun())return FALSE;for(i=0;i<8;i++)if(Licensed(i)&&GetTMHMItemIdFromMoveId(sChampionMoves[i])==item)return TRUE;return FALSE;}
void GleCompetitionPrizeMenu(void){const u8 *names[9];u32 i,claimed=0;sPrizeCount=0;for(i=0;i<8;i++)claimed+=Licensed(i);if(GymLeaderState()->championshipWins<=claimed){gSpecialVar_Result=255;GymLeaderResumeScriptNextFrame();return;}for(i=0;i<8;i++)if(!Licensed(i)){sPrizeOffers[sPrizeCount]=i;names[sPrizeCount++]=GetMoveName(sChampionMoves[i]);}names[sPrizeCount]=COMPOUND_STRING("Back");GymLeaderMenu(names,sPrizeCount+1,FALSE);}
void GleCompetitionPrizeClaim(void){u32 choice=gSpecialVar_Result,i,claimed=0,bit;u16 item;gSpecialVar_Result=GLE_NOT_READY;for(i=0;i<8;i++)claimed+=Licensed(i);if(choice>=sPrizeCount||GymLeaderState()->championshipWins<=claimed)return;i=sPrizeOffers[choice];if(Licensed(i))return;item=GetTMHMItemIdFromMoveId(sChampionMoves[i]);if(!item)return;if(!CheckBagHasItem(item,1)&&!AddBagItem(item,1)){gSpecialVar_Result=GLE_NO_ROOM;return;}bit=209+i;GymLeaderState()->uniqueObtained[bit/8]|=1u<<(bit%8);if(claimed==7&&GymLeaderState()->championshipWins>8)AddMoney(&gSaveBlock1Ptr->money,5000*(GymLeaderState()->championshipWins-8));gSpecialVar_Result=GLE_OK;}
u32 GymLeaderTrophyAt(u32 slot){u32 bit=217+slot*2,i,value=0;if(!IsGymLeaderRun()||slot>=3)return 0;for(i=0;i<2;i++)value|=((GymLeaderState()->uniqueObtained[(bit+i)/8]>>((bit+i)%8))&1)<<i;return value;}
void GleCompetitionTrophySlots(void){static const u8 *const names[]={COMPOUND_STRING("Left pedestal"),COMPOUND_STRING("Center pedestal"),COMPOUND_STRING("Right pedestal"),COMPOUND_STRING("Back")};GymLeaderMenu(names,4,FALSE);}
void GleCompetitionTrophyTypes(void){static const u8 *const names[]={COMPOUND_STRING("Empty"),COMPOUND_STRING("Regional trophy"),COMPOUND_STRING("Kanto trophy"),COMPOUND_STRING("Back")};gSpecialVar_0x8007=gSpecialVar_Result;GymLeaderMenu(names,4,FALSE);}
void GleCompetitionTrophySet(void){struct GleState *s=GymLeaderState();u32 slot=gSpecialVar_0x8007,choice=gSpecialVar_Result,i,bit;gSpecialVar_Result=GLE_NOT_READY;if(slot>=3||choice>=3||slot>min(s->landmarkTier,2)||(choice==1&&!s->regionalWinner)||(choice==2&&!s->championshipWins))return;bit=217+slot*2;for(i=0;i<2;i++){s->uniqueObtained[(bit+i)/8]&=~(1u<<((bit+i)%8));s->uniqueObtained[(bit+i)/8]|=((choice>>i)&1)<<((bit+i)%8);}gSpecialVar_Result=GLE_OK;}
void GleCompetitionTrophyInfo(void){u32 id=gSpecialVar_LastTalked;StringCopy(gStringVar4,id>=236&&id<=238&&GymLeaderTrophyAt(id-236)==2?COMPOUND_STRING("KANTO CHAMPIONSHIP TROPHY\nA permanent title in your history.\lA cosmetic display, with no boosts."):COMPOUND_STRING("REGIONAL CUP TROPHY\nYour Regional victory is remembered.\lA cosmetic display, with no boosts."));}
