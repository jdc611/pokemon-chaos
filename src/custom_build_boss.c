#include "global.h"
#include "custom_build.h"
#include "oaks_draft.h"
#include "item.h"
#include "constants/hold_effects.h"
#include "data.h"
#include "battle.h"
#include "battle_setup.h"
#include "ironmon.h"
#include "random.h"
#include "trainer_util.h"
#include "string_util.h"
#include "move.h"
#include "event_data.h"
#include "constants/abilities.h"
#include "constants/items.h"
#include "constants/moves.h"
#include "constants/species.h"
#include "constants/trainers.h"
#include "constants/pokemon.h"
#include "constants/battle_ai.h"
#include "constants/event_objects.h"
#include "constants/vars.h"

// Encounter-only records: bosses receive a random spread with their ordinary
// species BST, never the player's allocation or a player-dependent counterbuild.
struct BossSpread {u32 pid;u16 species,stats[6];};
static EWRAM_DATA struct BossSpread sBoss[12];
static EWRAM_DATA u8 sNextBoss;

enum {PLAN_BALANCE,PLAN_SAND,PLAN_RAIN,PLAN_ELECTRIC,PLAN_SUN,PLAN_PSYCHIC,PLAN_DOUBLES};
static u32 Plan(const struct Trainer *t)
{
 switch(t->trainerPic) {
 case TRAINER_PIC_LEADER_BROCK_FRLG:case TRAINER_PIC_LEADER_GIOVANNI_FRLG:return PLAN_SAND;
 case TRAINER_PIC_LEADER_MISTY_FRLG:return PLAN_RAIN;
 case TRAINER_PIC_LEADER_LT_SURGE_FRLG:return PLAN_ELECTRIC;
 case TRAINER_PIC_LEADER_ERIKA_FRLG:case TRAINER_PIC_LEADER_BLAINE_FRLG:return PLAN_SUN;
 case TRAINER_PIC_LEADER_SABRINA_FRLG:return PLAN_PSYCHIC;
 case TRAINER_PIC_LEADER_KOGA_FRLG:return PLAN_DOUBLES;
 default:return PLAN_BALANCE;
 }
}
bool32 CustomBuildIsBoss(const struct Trainer *t)
{
 if(!IsCustomBuild() || !t)return FALSE;
 switch(t->trainerClass) {
 case TRAINER_CLASS_LEADER_FRLG:case TRAINER_CLASS_BOSS_FRLG:case TRAINER_CLASS_ELITE_FOUR_FRLG:
 case TRAINER_CLASS_CHAMPION_FRLG:case TRAINER_CLASS_RIVAL_EARLY_FRLG:case TRAINER_CLASS_RIVAL_LATE_FRLG:return TRUE;
 }
 // Authored Chaos story encounters retain their existing species and format.
 return t->trainerPic==TRAINER_PIC_CHAOS_COLE || t->trainerPic==TRAINER_PIC_CHAOS_VESPER
     || t->trainerPic==TRAINER_PIC_CHAOS_JESSIE || t->trainerPic==TRAINER_PIC_CHAOS_JAMES;
}
u32 CustomBuildBossSeed(const struct Trainer *t)
{
 u32 seed=gSaveBlock3Ptr->worldSeed^t->trainerPic^((u32)t->trainerClass<<16)^0x424f5353u;
 for(u32 i=0;i<t->partySize;i++)seed=IronmonMix(seed^t->party[i].species^((u32)t->party[i].lvl<<16));
 return seed;
}
u64 CustomBuildBossAI(const struct Trainer *t,u64 flags)
{
 if(!CustomBuildIsBoss(t))return flags;
 flags |= AI_FLAG_CHECK_BAD_MOVE|AI_FLAG_CHECK_VIABILITY|AI_FLAG_TRY_TO_FAINT|AI_FLAG_TRY_TO_2HKO
        |AI_FLAG_HP_AWARE|AI_FLAG_POWERFUL_STATUS|AI_FLAG_SMART_SWITCHING|AI_FLAG_SMART_MON_CHOICES
        |AI_FLAG_CONSERVATIVE|AI_FLAG_PP_STALL_PREVENTION;
 // Do not inspect the player's chosen action or hidden build information.
 flags &= ~(AI_FLAG_OMNISCIENT|AI_FLAG_MOVE_OMNISCIENCE|AI_FLAG_ITEM_OMNISCIENCE|AI_FLAG_ABILITY_OMNISCIENCE
          |AI_FLAG_KNOW_OPPONENT_PARTY|AI_FLAG_PREDICT_MOVE|AI_FLAG_PREDICT_SWITCH|AI_FLAG_PREDICT_INCOMING_MON
          |AI_FLAG_FORCE_SETUP_FIRST_TURN|AI_FLAG_SEQUENCE_SWITCHING|AI_FLAG_RANDOMIZE_PARTY_INDICES);
 if(t->trainerClass==TRAINER_CLASS_RIVAL_EARLY_FRLG || t->trainerClass==TRAINER_CLASS_RIVAL_LATE_FRLG
    || t->trainerClass==TRAINER_CLASS_CHAMPION_FRLG)flags|=AI_FLAG_ACE_POKEMON;
 if(OaksDraftOpeningBattle())flags &= ~AI_FLAG_ACE_POKEMON; // Let all six participate normally at level 5.
 return flags;
}
static void ComputeSpread(struct BossSpread *r,u32 pid,enum Species sp)
{
 r->pid=pid;r->species=sp;
 const struct SpeciesInfo *s=&gSpeciesInfo[sp];
 u32 bst=s->baseHP+s->baseAttack+s->baseDefense+s->baseSpeed+s->baseSpAttack+s->baseSpDefense;
 u32 weights[6],total=0,left=bst-90,spent=0;
 for(u32 j=0;j<6;j++){weights[j]=10+IronmonMix(pid^gSaveBlock3Ptr->worldSeed^(0x424f5353u+j))%91;total+=weights[j];}
 for(u32 j=0;j<6;j++){u32 add=left*weights[j]/total;r->stats[j]=15+add;spent+=add;}
 for(u32 j=0;j<left-spent;j++)r->stats[IronmonMix(pid+j)%6]++;
}
u32 CustomBuildBossBaseStat(struct Pokemon *mon,u32 stat)
{
 enum Species sp=GetMonData(mon,MON_DATA_SPECIES);u32 pid=GetMonData(mon,MON_DATA_PERSONALITY);
 if(IsCustomBuild() && pid && sp>SPECIES_NONE && stat<6)for(u32 i=0;i<ARRAY_COUNT(sBoss);i++)if(sBoss[i].pid==pid) {
  // Keep the same random weights through authored Mega/form changes, using
  // only that form's legitimate species total.
  if(sBoss[i].species!=sp)ComputeSpread(&sBoss[i],pid,sp);
  return sBoss[i].stats[stat];
 }
 return GetSpeciesBaseStat(sp,stat);
}
u32 CustomBuildBossAbility(struct BoxPokemon *mon)
{
 if(!IsCustomBuild())return ABILITY_NONE;
 enum Species sp=GetBoxMonData(mon,MON_DATA_SPECIES);u32 pid=GetBoxMonData(mon,MON_DATA_PERSONALITY);
 if(pid && gSpeciesInfo[sp].isMegaEvolution)for(u32 i=0;i<ARRAY_COUNT(sBoss);i++)if(sBoss[i].pid==pid)return gSpeciesInfo[sp].abilities[0];
 return ABILITY_NONE;
}
static void RegisterSpread(struct Pokemon *mon)
{
 struct BossSpread *r=&sBoss[sNextBoss++%ARRAY_COUNT(sBoss)];
 ComputeSpread(r,GetMonData(mon,MON_DATA_PERSONALITY),GetMonData(mon,MON_DATA_SPECIES));
}
static const u16 sPhysical[]={[TYPE_NORMAL]=MOVE_BODY_SLAM,[TYPE_FIGHTING]=MOVE_CLOSE_COMBAT,[TYPE_FLYING]=MOVE_BRAVE_BIRD,[TYPE_POISON]=MOVE_POISON_JAB,[TYPE_GROUND]=MOVE_EARTHQUAKE,[TYPE_ROCK]=MOVE_ROCK_SLIDE,[TYPE_BUG]=MOVE_X_SCISSOR,[TYPE_GHOST]=MOVE_SHADOW_CLAW,[TYPE_STEEL]=MOVE_IRON_HEAD,[TYPE_FIRE]=MOVE_FIRE_PUNCH,[TYPE_WATER]=MOVE_WATERFALL,[TYPE_GRASS]=MOVE_LEAF_BLADE,[TYPE_ELECTRIC]=MOVE_THUNDER_PUNCH,[TYPE_PSYCHIC]=MOVE_ZEN_HEADBUTT,[TYPE_ICE]=MOVE_ICE_PUNCH,[TYPE_DRAGON]=MOVE_DRAGON_CLAW,[TYPE_DARK]=MOVE_CRUNCH,[TYPE_FAIRY]=MOVE_PLAY_ROUGH};
static const u16 sSpecial[]={[TYPE_NORMAL]=MOVE_HYPER_VOICE,[TYPE_FIGHTING]=MOVE_AURA_SPHERE,[TYPE_FLYING]=MOVE_AIR_SLASH,[TYPE_POISON]=MOVE_SLUDGE_BOMB,[TYPE_GROUND]=MOVE_EARTH_POWER,[TYPE_ROCK]=MOVE_POWER_GEM,[TYPE_BUG]=MOVE_BUG_BUZZ,[TYPE_GHOST]=MOVE_SHADOW_BALL,[TYPE_STEEL]=MOVE_FLASH_CANNON,[TYPE_FIRE]=MOVE_FLAMETHROWER,[TYPE_WATER]=MOVE_SURF,[TYPE_GRASS]=MOVE_ENERGY_BALL,[TYPE_ELECTRIC]=MOVE_THUNDERBOLT,[TYPE_PSYCHIC]=MOVE_PSYCHIC,[TYPE_ICE]=MOVE_ICE_BEAM,[TYPE_DRAGON]=MOVE_DRAGON_PULSE,[TYPE_DARK]=MOVE_DARK_PULSE,[TYPE_FAIRY]=MOVE_MOONBLAST};
static const u16 sEarlyPhysical[]={[TYPE_NORMAL]=MOVE_HEADBUTT,[TYPE_FIGHTING]=MOVE_LOW_SWEEP,[TYPE_FLYING]=MOVE_AERIAL_ACE,[TYPE_POISON]=MOVE_POISON_FANG,[TYPE_GROUND]=MOVE_BULLDOZE,[TYPE_ROCK]=MOVE_ROCK_TOMB,[TYPE_BUG]=MOVE_BUG_BITE,[TYPE_GHOST]=MOVE_SHADOW_PUNCH,[TYPE_STEEL]=MOVE_METAL_CLAW,[TYPE_FIRE]=MOVE_FIRE_FANG,[TYPE_WATER]=MOVE_RAZOR_SHELL,[TYPE_GRASS]=MOVE_RAZOR_LEAF,[TYPE_ELECTRIC]=MOVE_SPARK,[TYPE_PSYCHIC]=MOVE_PSYCHO_CUT,[TYPE_ICE]=MOVE_ICE_FANG,[TYPE_DRAGON]=MOVE_DRAGON_TAIL,[TYPE_DARK]=MOVE_BITE,[TYPE_FAIRY]=MOVE_PLAY_ROUGH};
static const u16 sEarlySpecial[]={[TYPE_NORMAL]=MOVE_SWIFT,[TYPE_FIGHTING]=MOVE_VACUUM_WAVE,[TYPE_FLYING]=MOVE_AIR_CUTTER,[TYPE_POISON]=MOVE_SLUDGE,[TYPE_GROUND]=MOVE_MUD_SHOT,[TYPE_ROCK]=MOVE_ANCIENT_POWER,[TYPE_BUG]=MOVE_STRUGGLE_BUG,[TYPE_GHOST]=MOVE_HEX,[TYPE_STEEL]=MOVE_MIRROR_SHOT,[TYPE_FIRE]=MOVE_FLAME_BURST,[TYPE_WATER]=MOVE_WATER_PULSE,[TYPE_GRASS]=MOVE_MAGICAL_LEAF,[TYPE_ELECTRIC]=MOVE_SHOCK_WAVE,[TYPE_PSYCHIC]=MOVE_PSYBEAM,[TYPE_ICE]=MOVE_ICY_WIND,[TYPE_DRAGON]=MOVE_DRAGON_BREATH,[TYPE_DARK]=MOVE_SNARL,[TYPE_FAIRY]=MOVE_DRAINING_KISS};
static enum Move Attack(enum Type type,bool32 physical,bool32 early,bool32 doubles)
{
 if(type==TYPE_NONE || type==TYPE_MYSTERY || type>=ARRAY_COUNT(sPhysical))type=TYPE_NORMAL;
 enum Move m=early?(physical?sEarlyPhysical[type]:sEarlySpecial[type]):(physical?sPhysical[type]:sSpecial[type]);
 if(doubles && m==MOVE_EARTHQUAKE)m=MOVE_HIGH_HORSEPOWER;
 if(doubles && m==MOVE_SURF)m=MOVE_SCALD;
 return m;
}
static bool32 HasType(enum Species sp,enum Type type){return GetSpeciesType(sp,0)==type || GetSpeciesType(sp,1)==type;}
static void SetUnique(struct Pokemon *mon,u32 slot,enum Move move)
{
 static const u16 fallback[]={MOVE_PROTECT,MOVE_TAUNT,MOVE_QUICK_ATTACK,MOVE_SWIFT,MOVE_THUNDER_WAVE};
 for(u32 attempt=0;attempt<=ARRAY_COUNT(fallback);attempt++) {
  bool32 valid=ChaosUsableMove(move);
  for(u32 j=0;j<slot;j++)if(GetMonData(mon,MON_DATA_MOVE1+j)==move)valid=FALSE;
  if(valid)break;
  move=fallback[min(attempt,ARRAY_COUNT(fallback)-1)];
 }
 SetMonMoveSlot(mon,move,slot);
}

static void Tune(struct Pokemon *mon,u32 index,u32 plan,bool32 doubles)
{
 enum Species sp=GetMonData(mon,MON_DATA_SPECIES);u32 level=GetMonData(mon,MON_DATA_LEVEL);
 bool32 physical=CustomBuildBaseStat(mon,STAT_ATK)>=CustomBuildBaseStat(mon,STAT_SPATK),early=level<25;
 u32 speed=CustomBuildBaseStat(mon,STAT_SPEED),bulk=CustomBuildBaseStat(mon,STAT_HP)+CustomBuildBaseStat(mon,STAT_DEF)+CustomBuildBaseStat(mon,STAT_SPDEF);
 bool32 support=index==0 || (bulk>speed*4 && index!=5);
 enum Ability ability=GetMonAbility(mon);enum Item held=GetMonData(mon,MON_DATA_HELD_ITEM);
 enum Move utility=doubles?MOVE_PROTECT:support?MOVE_REFLECT:physical?MOVE_SWORDS_DANCE:MOVE_NASTY_PLOT;
 enum Move fourth=support?MOVE_LIGHT_SCREEN:MOVE_U_TURN;
 if(!ability || ability==ABILITY_NONE || !ChaosUsableAbility(ability))ability=ABILITY_CLEAR_BODY;
 // Retain signature/native abilities when useful, add only a few partners
 // supporting the chosen field rather than giving every member a top ability.
 if(index==0) {
  if(plan==PLAN_SAND)ability=ABILITY_SAND_STREAM;
  if(plan==PLAN_RAIN)ability=ABILITY_DRIZZLE;
  if(plan==PLAN_SUN)ability=ABILITY_DROUGHT;
  if(plan==PLAN_ELECTRIC)ability=ABILITY_ELECTRIC_SURGE;
  if(plan==PLAN_PSYCHIC)ability=ABILITY_PSYCHIC_SURGE;
  if(plan==PLAN_BALANCE && !gSpeciesInfo[sp].isMegaEvolution)ability=ABILITY_INTIMIDATE;
 }
 if(index==2 && !gSpeciesInfo[sp].isMegaEvolution) {
  if(plan==PLAN_RAIN && HasType(sp,TYPE_WATER))ability=ABILITY_SWIFT_SWIM;
  else if(plan==PLAN_SUN && HasType(sp,TYPE_GRASS))ability=ABILITY_CHLOROPHYLL;
  else if(plan==PLAN_SAND && (HasType(sp,TYPE_ROCK)||HasType(sp,TYPE_GROUND)||HasType(sp,TYPE_STEEL)))ability=ABILITY_SAND_RUSH;
  else if(plan==PLAN_ELECTRIC && HasType(sp,TYPE_ELECTRIC))ability=ABILITY_SURGE_SURFER;
  else if(plan==PLAN_BALANCE && support)ability=ABILITY_REGENERATOR;
 }
 if(ability==ABILITY_NONE)ability=ABILITY_CLEAR_BODY;
 // Authored permanent Mega abilities remain meaningful after random spreads.
 if(gSpeciesInfo[sp].isMegaEvolution)ability=GetAbilityBySpecies(sp,0);
 SetMonData(mon,MON_DATA_CHAOS_STARTER_ABILITY,&ability);
 // Draft records supply the ability through a separate override; NPCs have
 // no player ledger entry, so the encrypted individual override is sufficient.
 enum Type type=GetSpeciesType(sp,0),secondary=GetSpeciesType(sp,1);
 enum Type coverage=physical?TYPE_FIGHTING:TYPE_ICE;
 if(type==TYPE_FIGHTING || type==TYPE_ICE)coverage=TYPE_GROUND;
 if(type==TYPE_WATER)coverage=index%2?TYPE_ELECTRIC:TYPE_ICE;
 if(type==TYPE_ROCK || type==TYPE_GROUND)coverage=index%2?TYPE_GRASS:TYPE_GROUND;
 if(type==TYPE_FIRE)coverage=TYPE_GRASS;
 if(type==TYPE_GRASS)coverage=TYPE_GROUND;
 if(type==TYPE_PSYCHIC)coverage=TYPE_FAIRY;
 if(type==TYPE_POISON)coverage=TYPE_GROUND;
 if(secondary!=type && index%2)coverage=secondary;
 SetUnique(mon,0,Attack(type,physical,early,doubles));
 SetUnique(mon,1,Attack(coverage,physical,early,doubles));
 if(doubles) {utility=MOVE_PROTECT;fourth=index==1?MOVE_TAILWIND:index%2?MOVE_TAUNT:Attack(TYPE_DARK,physical,early,TRUE);}
 else if(support) {
  utility=index==0?MOVE_REFLECT:MOVE_THUNDER_WAVE;
  fourth=index==0?MOVE_LIGHT_SCREEN:MOVE_RECOVER;
  if(plan==PLAN_ELECTRIC || plan==PLAN_PSYCHIC)fourth=MOVE_VOLT_SWITCH;
 }
 else if(index==5) {fourth=physical?MOVE_QUICK_ATTACK:Attack(secondary,physical,early,FALSE);}
 else if(speed<55 && !early) {utility=physical?MOVE_BULK_UP:MOVE_CALM_MIND;fourth=MOVE_RECOVER;}
 else if(!physical)fourth=MOVE_VOLT_SWITCH;
 if(plan==PLAN_SUN && coverage==TYPE_GRASS && !physical && !early)SetMonMoveSlot(mon,MOVE_SOLAR_BEAM,1);
 if(plan==PLAN_RAIN && type==TYPE_ELECTRIC && !early)SetMonMoveSlot(mon,MOVE_THUNDER,0);
 SetUnique(mon,2,utility);SetUnique(mon,3,fourth);
 // Keep authored Mega Stones and signature equipment. Early teams use modest
 // berries; screens/weather setters receive their corresponding duration item.
 if(GetItemHoldEffect(held)!=HOLD_EFFECT_MEGA_STONE) {
  if(index==0 && !doubles)held=ITEM_LIGHT_CLAY;
  else if(index==2 && plan==PLAN_RAIN)held=ITEM_MYSTIC_WATER;
  else if(index==2 && plan==PLAN_SUN)held=HasType(sp,TYPE_GRASS)?ITEM_MIRACLE_SEED:ITEM_CHARCOAL;
  else if(!held)held=early?ITEM_ORAN_BERRY:level<40?ITEM_SITRUS_BERRY:support?ITEM_LEFTOVERS:ITEM_EXPERT_BELT;
  SetMonData(mon,MON_DATA_HELD_ITEM,&held);
 }
 CalculateMonStats(mon);HealPokemon(mon);
}
void CustomBuildTuneBossParty(struct Pokemon *party,const struct Trainer *t)
{
 if(!CustomBuildIsBoss(t))return;
 u32 count=0,level=5;for(u32 i=0;i<6;i++)if(GetMonData(&party[i],MON_DATA_SPECIES)){count++;level=max(level,GetMonData(&party[i],MON_DATA_LEVEL));}
 if(t->trainerClass==TRAINER_CLASS_LEADER_FRLG && count<6) {
  // Preserve every authored member and keep the signature ace last.
  struct Pokemon ace=party[count-1];u32 insert=count-1;
  enum Species extras[3]={SPECIES_NONE};
  switch(t->trainerPic) {
  case TRAINER_PIC_LEADER_BROCK_FRLG:extras[0]=SPECIES_ROGGENROLA;extras[1]=SPECIES_ANORITH;extras[2]=SPECIES_ARCHEN;break;
  case TRAINER_PIC_LEADER_MISTY_FRLG:extras[0]=SPECIES_WINGULL;extras[1]=SPECIES_MARILL;extras[2]=SPECIES_SHELLOS;break;
  case TRAINER_PIC_LEADER_LT_SURGE_FRLG:extras[0]=SPECIES_FLAAFFY;extras[1]=SPECIES_CHARJABUG;extras[2]=SPECIES_EMOLGA;break;
  case TRAINER_PIC_LEADER_ERIKA_FRLG:extras[0]=SPECIES_WHIMSICOTT;extras[1]=SPECIES_TANGELA;extras[2]=SPECIES_LEAFEON;break;
  case TRAINER_PIC_LEADER_SABRINA_FRLG:extras[0]=SPECIES_BRONZONG;extras[1]=SPECIES_ORANGURU;extras[2]=SPECIES_ESPEON;break;
  case TRAINER_PIC_LEADER_KOGA_FRLG:extras[0]=SPECIES_DRAPION;extras[1]=SPECIES_VENOMOTH;extras[2]=SPECIES_ARIADOS;break;
  case TRAINER_PIC_LEADER_BLAINE_FRLG:extras[0]=SPECIES_COALOSSAL;extras[1]=SPECIES_CAMERUPT;extras[2]=SPECIES_MAGMAR;break;
  case TRAINER_PIC_LEADER_GIOVANNI_FRLG:extras[0]=SPECIES_EXCADRILL;extras[1]=SPECIES_PALOSSAND;extras[2]=SPECIES_DUGTRIO;break;
  default:extras[0]=SPECIES_NONE;break;
  }
  for(u32 i=0;count<6 && i<3;i++)if(extras[i]) {
   u32 pid=IronmonMix(gSaveBlock3Ptr->worldSeed^t->trainerPic^(0x45585452u+i));
   CreateMon(&party[insert],extras[i],level,pid,OTID_STRUCT_PLAYER_ID);
   SetMonData(&party[insert],MON_DATA_IVS,&t->party[0].iv);insert++;count++;
  }
  party[count-1]=ace;
 }
 if(party==gParties[B_TRAINER_OPPONENT_A]){memset(sBoss,0,sizeof(sBoss));sNextBoss=0;}
 u32 plan=Plan(t);bool32 doubles=t->battleType==TRAINER_BATTLE_TYPE_DOUBLES;
 for(u32 i=0;i<count;i++) {
  // Drafted NPCs already have deterministic individual randomized bases.
  // Other bosses get random redistribution of the ordinary species total.
  if(t->trainerClass!=TRAINER_CLASS_RIVAL_EARLY_FRLG && t->trainerClass!=TRAINER_CLASS_RIVAL_LATE_FRLG && t->trainerClass!=TRAINER_CLASS_CHAMPION_FRLG)RegisterSpread(&party[i]);
  Tune(&party[i],i,plan,doubles);
 }
}
void CustomBuildRoomGraphics(void)
{
 VarSet(VAR_OBJ_GFX_ID_0,CustomBuildAceRules()?OBJ_EVENT_GFX_GOLDEN_DRAFT_BOX:OBJ_EVENT_GFX_MOVING_BOX);
}

void CustomBuildGoldenRevealCheck(void){gSpecialVar_Result=CustomBuildAceRules();}
