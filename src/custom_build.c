#include "global.h"
#include "custom_build.h"
#include "oaks_draft.h"
#include "ironmon.h"
#include "run_settings.h"
#include "pokemon_storage_system.h"
#include "data.h"
#include "move.h"
#include "random.h"
#include "event_data.h"
#include "malloc.h"
#include "item.h"
#include "pokedex.h"
#include "script_pokemon_util.h"
#include "string_util.h"
#include "constants/species.h"
#include "constants/pokemon.h"
#include "constants/form_change_types.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/vars.h"
#include "constants/hold_effects.h"
STATIC_ASSERT(sizeof(struct CustomBuildRecord)==26,CustomRecordSize);
STATIC_ASSERT(sizeof(((struct SaveBlock1 *)0)->filler1)==2*sizeof(struct CustomBuildRecord),CustomDexBuffer1);
STATIC_ASSERT(sizeof(((struct SaveBlock2 *)0)->pokedex.filler)==4*sizeof(struct CustomBuildRecord),CustomDexBuffer2);
static bool8 sCreating;
static EWRAM_DATA u16 sSpecies[18];
static EWRAM_DATA u32 sSeed;
static EWRAM_DATA bool8 sSpeciesValid;
static struct CustomBuildRecord *Record(u32 index)
{
 if(index<2)return (void *)(gSaveBlock1Ptr->filler1+index*26);
 return (void *)(gSaveBlock2Ptr->pokedex.filler+(index-2)*26);
}
bool32 IsCustomBuild(void){return IS_FRLG && gSaveBlock3Ptr && gSaveBlock3Ptr->starterMode==RUN_STARTER_CUSTOM_BUILD && gSaveBlock3Ptr->draft.magic==OAKS_DRAFT_MAGIC;}
void CustomBuildInitialize(void)
{
 if(!IsCustomBuild())return;
 for(u32 i=0;i<6;i++)memset(Record(i),0,26);
 gSaveBlock3Ptr->bstMode=RUN_BST_OFF;gSaveBlock3Ptr->abilityMode=RUN_ABILITIES_NORMAL;
 gSaveBlock3Ptr->movesetMode=RUN_MOVESETS_RANDOM;gSaveBlock3Ptr->itemRandomization=TRUE;
}
struct CustomBuildRecord *CustomBuildFind(u32 pid)
{
 if(!IsCustomBuild() || !pid)return NULL;
 for(u32 i=0;i<6;i++)if(Record(i)->personality==pid)return Record(i);
 return NULL;
}
static u32 NativeBST(enum Species sp)
{
 const struct SpeciesInfo *s=&gSpeciesInfo[sp];return s->baseHP+s->baseAttack+s->baseDefense+s->baseSpeed+s->baseSpAttack+s->baseSpDefense;
}
static bool32 NormalEvolution(const struct Evolution *e)
{
 return e->method!=EVO_NONE && e->method!=EVO_SPLIT_FROM_EVO && e->method!=EVO_LEVEL_BATTLE_ONLY && e->targetSpecies>0 && e->targetSpecies<NUM_SPECIES && IsSpeciesEnabled(e->targetSpecies);
}
static u32 Reserve(enum Species sp,u32 depth,bool32 points)
{
 if(depth>=4 || !sp || sp>=NUM_SPECIES)return 0;
 const struct Evolution *e=gSpeciesInfo[sp].evolutions;u32 result=0;
 if(e)for(u32 i=0;e[i].method!=EVOLUTIONS_END;i++)if(NormalEvolution(e+i)) {
  s32 diff=(s32)NativeBST(e[i].targetSpecies)-NativeBST(sp);
  u32 gain=points?max(10,diff):1;
  result=max(result,gain+Reserve(e[i].targetSpecies,depth+1,points));
 }
 return result;
}
u32 CustomBuildReserve(enum Species sp){return Reserve(sp,0,TRUE);}
u32 CustomBuildDepth(enum Species sp){return Reserve(sp,0,FALSE);}
static bool32 HasMega(enum Species sp,u32 depth)
{
 if(depth>3)return FALSE;
 const struct FormChange *f=GetSpeciesFormChanges(sp);
 if(f)for(u32 i=0;f[i].method!=FORM_CHANGE_TERMINATOR;i++)if(gSpeciesInfo[f[i].targetSpecies].isMegaEvolution)return TRUE;
 const struct Evolution *e=gSpeciesInfo[sp].evolutions;
 if(e)for(u32 i=0;e[i].method!=EVOLUTIONS_END;i++)if(NormalEvolution(e+i) && HasMega(e[i].targetSpecies,depth+1))return TRUE;
 return FALSE;
}
bool32 CustomBuildHasMega(enum Species sp){return HasMega(sp,0);}
static bool32 Eligible(enum Species sp)
{
 if(!sp || sp>=NUM_SPECIES || !IsSpeciesEnabled(sp) || !SpeciesToNationalPokedexNum(sp) || SpeciesToNationalPokedexNum(sp)>NATIONAL_DEX_COUNT)return FALSE;
 const struct SpeciesInfo *s=&gSpeciesInfo[sp];
 if(GET_BASE_SPECIES_ID(sp)!=sp && !s->isAlolanForm && !s->isGalarianForm && !s->isHisuianForm && !s->isPaldeanForm)return FALSE;
 if(s->isMegaEvolution || s->isPrimalReversion || s->isUltraBurst || s->isGigantamax || s->isTeraForm || s->isTotem)return FALSE;
 return TRUE;
}
void CustomBuildGenerate(u16 out[18])
{
 // One seeded reservoir sample per category, without allocating a species-sized pool.
 if(sSpeciesValid && sSeed==gSaveBlock3Ptr->worldSeed){memcpy(out,sSpecies,sizeof(sSpecies));return;}
 memset(out,0,18*sizeof(u16));
 u8 *pre=AllocZeroed((NUM_SPECIES+7)/8);
 if(!pre)return;
 for(u32 sp=1;sp<NUM_SPECIES;sp++)if(Eligible(sp)) {
  const struct Evolution *e=gSpeciesInfo[sp].evolutions;
  if(e)for(u32 j=0;e[j].method!=EVOLUTIONS_END;j++)if(NormalEvolution(e+j))pre[e[j].targetSpecies/8]|=1<<(e[j].targetSpecies%8);
 }
 u16 *pool=Alloc(NUM_SPECIES*sizeof(u16));u32 count=0;
 if(!pool){Free(pre);return;}
 for(u32 sp=1;sp<NUM_SPECIES;sp++) {
  if(!Eligible(sp) || (pre[sp/8]&(1<<(sp%8))) || (pre[GET_BASE_SPECIES_ID(sp)/8]&(1<<(GET_BASE_SPECIES_ID(sp)%8))))continue;
  u32 depth=CustomBuildDepth(sp),cat=depth?0:CustomBuildHasMega(sp)?2:1;
  if(depth<=2 && CustomBuildReserve(sp)<=520)pool[count++]=sp|(cat<<11);
 }
 for(u32 index=0;index<18;index++) {
  u32 best=0xffffffff;
  for(u32 entry=0;entry<count;entry++) {
   u32 sp=pool[entry]&0x7ff,cat=pool[entry]>>11;
   if(cat!=index%3)continue;
   bool32 used=FALSE;
   for(u32 j=0;j<index;j++)if(GET_BASE_SPECIES_ID(out[j])==GET_BASE_SPECIES_ID(sp))used=TRUE;
   if(used)continue;
   u32 score=IronmonMix(gSaveBlock3Ptr->worldSeed ^ (index*0x9e3779b9u) ^ sp);
   if(score<best){best=score;out[index]=sp;}
  }
 }
 Free(pool);
 Free(pre);memcpy(sSpecies,out,sizeof(sSpecies));sSeed=gSaveBlock3Ptr->worldSeed;sSpeciesValid=TRUE;
}
u32 CustomBuildSum(const struct CustomBuildRecord *r){u32 sum=0;for(u32 i=0;i<6;i++)sum+=r->stats[i];return sum;}
void CustomBuildFloors(const struct CustomBuildRecord *r,u16 floors[6])
{
 u32 bst=CustomBuildSum(r),low=bst<250?10:bst<350?15:bst<450?20:bst<550?25:30;
 u32 high=bst<250?25:bst<350?35:bst<450?45:bst<550?55:65,total=0;
 for(u32 i=0;i<6;i++){floors[i]=low+IronmonMix(r->personality^(i+0x464c4f52u))%(high-low+1);total+=floors[i];}
 u32 ceiling=bst*3/4;
 for(u32 i=0;total>ceiling;i=(i+1)%6)if(floors[i]>low){floors[i]--;total--;}
}
void CustomBuildAbilities(const struct CustomBuildRecord *r,u16 choices[3],bool32 retain)
{
 for(u32 i=0;i<3;i++) {
  choices[i]=i==0 && retain?r->ability:1+IronmonMix(r->personality ^ r->builtSpecies ^ (0x4142494cu+i))%(ABILITIES_COUNT-1);
  for(u32 n=0;n<ABILITIES_COUNT;n++) {
   bool32 valid=ChaosUsableAbility(choices[i]);for(u32 j=0;j<i;j++)if(choices[j]==choices[i])valid=FALSE;
   if(valid || (i==0 && retain))break;
   choices[i]=choices[i]%(ABILITIES_COUNT-1)+1;
  }
 }
}
static void AutoAllocate(struct CustomBuildRecord *r,u32 bst)
{
 for(u32 i=0;i<6;i++)r->stats[i]=bst/6+(i<bst%6);
 u16 floors[6];CustomBuildFloors(r,floors);
 u32 left=bst;for(u32 i=0;i<6;i++){r->stats[i]=floors[i];left-=floors[i];}
 for(u32 i=0;left;i++){r->stats[IronmonMix(r->personality+i)%6]++;left--;}
}
void CustomBuildCandidate(struct Pokemon *mon,struct CustomBuildRecord *r,u32 index,enum Species gift,u32 level)
{
 u16 all[18];enum Species sp=gift;
 if(!sp){CustomBuildGenerate(all);sp=index<18?all[index]:SPECIES_NONE;}
 memset(r,0,sizeof(*r));if(!sp){ZeroMonData(mon);return;}
 u32 potentialSeed=gSaveBlock3Ptr->worldSeed ^ (gift?0x47494654u^(gift<<8)^((u32)gSaveBlock1Ptr->location.mapGroup<<24)^((u32)gSaveBlock1Ptr->location.mapNum<<16):0x504f4b45u+index);
 u32 nonce=0;if(gift)for(u32 i=0;i<6;i++)nonce^=Record(i)->personality;
 r->personality=IronmonMix(potentialSeed^nonce);
 if(!r->personality)r->personality=1;
 r->rootSpecies=r->builtSpecies=sp;r->megaGain=40+IronmonMix(potentialSeed^0x4d454741u)%121;
 u32 reserve=CustomBuildReserve(sp),minimum=max(300,180+reserve);
 if(!reserve && CustomBuildHasMega(sp))minimum=max(minimum,180+r->megaGain);
 r->finalBST=minimum+IronmonMix(potentialSeed^0x425354u)%(701-minimum);
 if(!reserve && CustomBuildHasMega(sp))r->finalBST=max(180,(s32)r->finalBST-r->megaGain);
 AutoAllocate(r,r->finalBST-reserve);
 u16 abilities[3];CustomBuildAbilities(r,abilities,FALSE);r->ability=abilities[IronmonMix(r->personality^0x5049434bu)%3];
 rng_value_t old=gRngValue,old2=gRng2Value;SeedRng(r->personality);SeedRng2(r->personality);
 bool8 wasCreating=sCreating;sCreating=TRUE;
 CreateMonWithIVs(mon,sp,level,r->personality,OTID_STRUCT_PLAYER_ID,USE_RANDOM_IVS);
 sCreating=wasCreating;
 u32 moveSeed=r->personality;
 for(u32 slot=0;slot<4;slot++) {
  enum Move move=1+IronmonMix(moveSeed+slot)%(MOVES_COUNT-1);
  for(u32 n=0;n<MOVES_COUNT;n++) {
   bool32 ok=ChaosUsableMove(move);for(u32 j=0;j<slot;j++)if(GetMonData(mon,MON_DATA_MOVE1+j)==move)ok=FALSE;
   if(slot==0 && !IronmonMoveIsStarterAttack(move))ok=FALSE;
   if(ok)break;move=move%(MOVES_COUNT-1)+1;
  }
  SetMonMoveSlot(mon,move,slot);
 }
 SetMonData(mon,MON_DATA_CHAOS_STARTER_ABILITY,&r->ability);
 gRngValue=old;gRng2Value=old2;
}
static EWRAM_DATA struct CustomBuildRecord sNpc;
static EWRAM_DATA u16 sNpcSpecies;
static struct CustomBuildRecord *Lookup(struct Pokemon *mon)
{
 if(!IsCustomBuild() || sCreating)return NULL;
 u32 pid=GetMonData(mon,MON_DATA_PERSONALITY);struct CustomBuildRecord *r=CustomBuildFind(pid);if(r)return r;
 enum Species species=GetMonData(mon,MON_DATA_SPECIES);
 if(sNpc.personality==pid && sNpcSpecies==species)return &sNpc;
 for(u32 i=0;i<18;i++)if(IronmonMix(gSaveBlock3Ptr->worldSeed^(0x504f4b45u+i))==pid) {
  struct Pokemon temp;CustomBuildCandidate(&temp,&sNpc,i,0,5);sNpcSpecies=species;
  u32 budget=gSpeciesInfo[species].isMegaEvolution?sNpc.finalBST+sNpc.megaGain:sNpc.finalBST-CustomBuildReserve(species);
  AutoAllocate(&sNpc,budget);return &sNpc;
 }
 return NULL;
}
u32 CustomBuildBaseStat(struct Pokemon *mon,u32 stat)
{
 struct CustomBuildRecord *r=Lookup(mon);return r && stat<6?r->stats[stat]:GetSpeciesBaseStat(GetMonData(mon,MON_DATA_SPECIES),stat);
}
u32 CustomBuildAbility(struct BoxPokemon *mon)
{
 struct CustomBuildRecord *r=CustomBuildFind(GetBoxMonData(mon,MON_DATA_PERSONALITY));return r?r->ability:ABILITY_NONE;
}
bool32 CustomBuildGrowth(struct Pokemon *mon,struct CustomBuildRecord *out)
{
 struct CustomBuildRecord *r=CustomBuildFind(GetMonData(mon,MON_DATA_PERSONALITY));
 enum Species species=GetMonData(mon,MON_DATA_SPECIES);
 if(!r || species==r->builtSpecies)return FALSE;
 bool32 legitimate=FALSE;
 const struct Evolution *e=gSpeciesInfo[r->builtSpecies].evolutions;
 if(e)for(u32 i=0;e[i].method!=EVOLUTIONS_END;i++)if(NormalEvolution(e+i) && e[i].targetSpecies==species)legitimate=TRUE;
 if(!legitimate && !(gSpeciesInfo[species].isMegaEvolution && CustomBuildHasMega(r->builtSpecies)))return FALSE;
 *out=*r;u32 bst=CustomBuildSum(r),budget;
 if(gSpeciesInfo[species].isMegaEvolution) {
  if(r->flags&1)return FALSE;
  budget=r->finalBST+r->megaGain;out->flags|=1;
 } else budget=r->finalBST-CustomBuildReserve(species);
 if(budget<=bst)return FALSE; // temporary weather/ability forms do not grant growth.
 out->builtSpecies=species;return TRUE;
}
bool32 CustomBuildCommit(struct Pokemon *mon,const struct CustomBuildRecord *r,u32 choice,u32 giftSlot,enum Item giftItem)
{
 u32 slot=gSaveBlock3Ptr->draft.round;
 if(choice<3) {
  if(slot>=6 || gPartiesCount[B_TRAINER_PLAYER]!=slot)return FALSE;
  gSaveBlock3Ptr->draft.picks[slot]=choice;
  u32 rival=choice==0?1:0,best=0;
  for(u32 c=0;c<3;c++)if(c!=choice){struct Pokemon candidate;struct CustomBuildRecord profile;CustomBuildCandidate(&candidate,&profile,slot*3+c,0,5);u32 score=CustomBuildSum(&profile);if(score>best){best=score;rival=c;}}
  gSaveBlock3Ptr->draft.rival[slot]=rival;
  memcpy(Record(slot),r,26);gParties[B_TRAINER_PLAYER][slot]=*mon;gPartiesCount[B_TRAINER_PLAYER]=++gSaveBlock3Ptr->draft.round;
  struct Pokemon rejected;struct CustomBuildRecord other;CustomBuildCandidate(&rejected,&other,(slot*3+rival),0,5);
  GetMonData(&rejected,MON_DATA_NICKNAME,gStringVar1);StringCopy(gStringVar2,GetSpeciesName(GetMonData(&rejected,MON_DATA_SPECIES)));
 } else if(giftSlot<6) {
  if(giftSlot>=gPartiesCount[B_TRAINER_PLAYER] || !GetMonData(&gParties[B_TRAINER_PLAYER][giftSlot],MON_DATA_HP))return FALSE;
  struct CustomBuildRecord *old=CustomBuildFind(GetMonData(&gParties[B_TRAINER_PLAYER][giftSlot],MON_DATA_PERSONALITY));if(!old)return FALSE;
  enum Item held=GetMonData(&gParties[B_TRAINER_PLAYER][giftSlot],MON_DATA_HELD_ITEM);
  if(held && !AddBagItem(held,1))return FALSE;
  *old=*r;gParties[B_TRAINER_PLAYER][giftSlot]=*mon;VarSet(VAR_TEMP_TRANSFERRED_SPECIES,GetMonData(mon,MON_DATA_SPECIES));
 } else {
  struct CustomBuildRecord *old=CustomBuildFind(r->personality);if(!old)return FALSE;*old=*r;
 }
 struct Pokemon *actual=choice<3?&gParties[B_TRAINER_PLAYER][slot]:giftSlot<6?&gParties[B_TRAINER_PLAYER][giftSlot]:mon;
 if(giftItem)SetMonData(actual,MON_DATA_HELD_ITEM,&giftItem);
 CalculateMonStats(actual);HandleSetPokedexFlagFromMon(actual,FLAG_SET_SEEN);HandleSetPokedexFlagFromMon(actual,FLAG_SET_CAUGHT);
 return TRUE;
}

enum Species CustomBuildMegaTarget(struct Pokemon *mon,enum Item item)
{
 if(!IsCustomBuild() || !CustomBuildFind(GetMonData(mon,MON_DATA_PERSONALITY)) || !CheckBagHasItem(ITEM_MEGA_RING,1) || PlayerPartyHasPermanentMega())return SPECIES_NONE;
 enum Species species=GetMonData(mon,MON_DATA_SPECIES);const struct FormChange *f=GetSpeciesFormChanges(species);
 if(!f)return SPECIES_NONE;
 for(u32 i=0;f[i].method!=FORM_CHANGE_TERMINATOR;i++) {
  if(!gSpeciesInfo[f[i].targetSpecies].isMegaEvolution)continue;
  if(f[i].method==FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM && item==f[i].param1 && (CheckBagHasItem(item,1) || GetMonData(mon,MON_DATA_HELD_ITEM)==item))return f[i].targetSpecies;
  if(item==ITEM_MEGA_RING && f[i].method==FORM_CHANGE_BATTLE_MEGA_EVOLUTION_MOVE)
   for(u32 move=0;move<4;move++)if(GetMonData(mon,MON_DATA_MOVE1+move)==f[i].param1)return f[i].targetSpecies;
 }
 return SPECIES_NONE;
}
