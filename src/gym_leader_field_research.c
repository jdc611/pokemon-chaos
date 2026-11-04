#include "global.h"
#include "gym_leader.h"
#include "gym_leader_research.h"
#include "gym_leader_script.h"
#include "battle.h"
#include "battle_setup.h"
#include "event_data.h"
#include "overworld.h"
#include "pokemon.h"
#include "pokemon_storage_system.h"
#include "random.h"
#include "script.h"
#include "string_util.h"
#include "constants/event_objects.h"
#include "constants/event_object_movement.h"
#include "constants/maps.h"

// Fixed reservations leave room for later projects without renumbering saves.
#define FIELD_FIRST_CAPTURE 95
#define FIELD_FIRST_DISCOVERY 130
#define FIELD_FIRST_SEAL 165
#define FIELD_LOCAL_ID 200
struct GleFieldSite {
    u16 species,map;
    u8 x,y,rank,project,answer;
    const u8 *location,*clue,*answers[3];
};
static const struct GleFieldSite sSites[]={
    {SPECIES_WO_CHIEN, MAP_VIRIDIAN_FOREST, 23, 31, 5, 7, 0, COMPOUND_STRING("VIRIDIAN FOREST"), COMPOUND_STRING("A grudge is carved into old leaves.\nWhat lets the forest heal?"), {COMPOUND_STRING("Forgiveness"),COMPOUND_STRING("More grudges"),COMPOUND_STRING("Burning it")}},
    {SPECIES_CHIEN_PAO, MAP_SEAFOAM_ISLANDS_B2F, 14, 15, 5, 7, 1, COMPOUND_STRING("SEAFOAM B2F"), COMPOUND_STRING("A shattered sword sleeps in ice.\nWhat should guide a drawn blade?"), {COMPOUND_STRING("Anger"),COMPOUND_STRING("Restraint"),COMPOUND_STRING("Greed")}},
    {SPECIES_TING_LU, MAP_ROCK_TUNNEL_1F, 28, 22, 5, 7, 2, COMPOUND_STRING("ROCK TUNNEL 1F"), COMPOUND_STRING("An ancient vessel holds old fear.\nWhat makes its burden lighter?"), {COMPOUND_STRING("Wealth"),COMPOUND_STRING("Silence"),COMPOUND_STRING("Compassion")}},
    {SPECIES_CHI_YU, MAP_POKEMON_MANSION_B1F, 24, 12, 5, 7, 0, COMPOUND_STRING("MANSION B1F"), COMPOUND_STRING("Envious beads feed a hidden flame.\nWhat breaks the curse of envy?"), {COMPOUND_STRING("Gratitude"),COMPOUND_STRING("Hoarding"),COMPOUND_STRING("Revenge")}},
    {SPECIES_TAPU_KOKO, MAP_ROUTE10, 17, 33, 4, 9, 1, COMPOUND_STRING("ROUTE 10 SHRINE"), COMPOUND_STRING("Thunder guards this old shrine.\nHow should a leader use power?"), {COMPOUND_STRING("Domination"),COMPOUND_STRING("Protection"),COMPOUND_STRING("Showing off")}},
    {SPECIES_TAPU_LELE, MAP_ROUTE25, 50, 7, 5, 9, 2, COMPOUND_STRING("ROUTE 25 SHRINE"), COMPOUND_STRING("This guardian senses your intent.\nWhat earns a guardian's trust?"), {COMPOUND_STRING("Flattery"),COMPOUND_STRING("Payment"),COMPOUND_STRING("Sincerity")}},
    {SPECIES_TAPU_BULU, MAP_SAFARI_ZONE_WEST, 14, 14, 6, 9, 0, COMPOUND_STRING("SAFARI WEST SHRINE"), COMPOUND_STRING("Roots bind this ancient shrine.\nWhat keeps a habitat thriving?"), {COMPOUND_STRING("Balance"),COMPOUND_STRING("Overgrowth"),COMPOUND_STRING("Exploitation")}},
    {SPECIES_TAPU_FINI, MAP_ROUTE19, 13, 8, 7, 9, 1, COMPOUND_STRING("ROUTE 19 SHRINE"), COMPOUND_STRING("Mist veils the guardian's pool.\nWhat clears a troubled mind?"), {COMPOUND_STRING("Haste"),COMPOUND_STRING("Reflection"),COMPOUND_STRING("Denial")}},
    {SPECIES_UXIE, MAP_ROUTE22, 20, 12, 3, 8, 2, COMPOUND_STRING("ROUTE 22 LAKE"), COMPOUND_STRING("Knowledge sleeps beside the lake.\nWhat makes wisdom grow?"), {COMPOUND_STRING("Guessing"),COMPOUND_STRING("Pretending"),COMPOUND_STRING("Learning")}},
    {SPECIES_MESPRIT, MAP_SAFARI_ZONE_EAST, 21, 20, 4, 8, 0, COMPOUND_STRING("SAFARI EAST LAKE"), COMPOUND_STRING("A feeling echoes over the water.\nHow can another heart be heard?"), {COMPOUND_STRING("Empathy"),COMPOUND_STRING("Indifference"),COMPOUND_STRING("Boasting")}},
    {SPECIES_AZELF, MAP_ROUTE12, 13, 53, 5, 8, 1, COMPOUND_STRING("ROUTE 12 LAKE"), COMPOUND_STRING("Resolve stirs beside the water.\nWhat carries a leader through?"), {COMPOUND_STRING("Giving up"),COMPOUND_STRING("Perseverance"),COMPOUND_STRING("Shortcuts")}},
    {SPECIES_OKIDOGI, MAP_ROUTE15, 42, 5, 5, 13, 2, COMPOUND_STRING("ROUTE 15"), COMPOUND_STRING("Broken chains scar this ground.\nWhat makes a bond worth keeping?"), {COMPOUND_STRING("Fear"),COMPOUND_STRING("Strength alone"),COMPOUND_STRING("Loyalty")}},
    {SPECIES_MUNKIDORI, MAP_ROUTE8, 26, 9, 6, 13, 0, COMPOUND_STRING("ROUTE 8"), COMPOUND_STRING("A sly pact lingers in the air.\nWhat exposes a poisonous promise?"), {COMPOUND_STRING("Truth"),COMPOUND_STRING("Blind trust"),COMPOUND_STRING("Silence")}},
    {SPECIES_FEZANDIPITI, MAP_ROUTE18, 22, 9, 7, 13, 1, COMPOUND_STRING("ROUTE 18"), COMPOUND_STRING("Lovely feathers hide a dark pact.\nHow should a leader judge others?"), {COMPOUND_STRING("Appearance"),COMPOUND_STRING("Actions"),COMPOUND_STRING("Rumors")}},
    {SPECIES_ZARUDE, MAP_VIRIDIAN_FOREST, 40, 48, 4, 11, 2, COMPOUND_STRING("DEEP VIRIDIAN FOREST"), COMPOUND_STRING("The guardian protects its young.\nHow should you enter its home?"), {COMPOUND_STRING("Make threats"),COMPOUND_STRING("Take a trophy"),COMPOUND_STRING("Show respect")}},
    {SPECIES_CYCLIZAR, MAP_ROUTE16, 30, 9, 2, 1, 0, COMPOUND_STRING("ROUTE 16 STUDY SITE"), COMPOUND_STRING("A swift partner watches the road.\nHow should a researcher begin?"), {COMPOUND_STRING("With patience"),COMPOUND_STRING("With threats"),COMPOUND_STRING("With a trap")}},
    {SPECIES_KORAIDON, MAP_MT_MOON_B2F, 16, 21, 5, 1, 1, COMPOUND_STRING("MT. MOON ANCIENT TRACE"), COMPOUND_STRING("Ancient tracks cross the stone.\nHow should this past be studied?"), {COMPOUND_STRING("Erase it"),COMPOUND_STRING("Preserve it"),COMPOUND_STRING("Ignore it")}},
    {SPECIES_MIRAIDON, MAP_POWER_PLANT, 35, 12, 6, 1, 2, COMPOUND_STRING("POWER PLANT SIGNAL"), COMPOUND_STRING("An unknown signal fills the air.\nHow should new knowledge be used?"), {COMPOUND_STRING("For control"),COMPOUND_STRING("For profit only"),COMPOUND_STRING("For discovery")}},
    {SPECIES_COBALION, MAP_ROCK_TUNNEL_B1F, 8, 9, 3, 6, 0, COMPOUND_STRING("ROCK TUNNEL WATCH"), COMPOUND_STRING("An iron guardian protects the weak.\nWhat makes strength honorable?"), {COMPOUND_STRING("Protecting others"),COMPOUND_STRING("Winning at any cost"),COMPOUND_STRING("Threatening others")}},
    {SPECIES_TERRAKION, MAP_VICTORY_ROAD_2F, 20, 7, 4, 6, 1, COMPOUND_STRING("VICTORY ROAD PASS"), COMPOUND_STRING("A stone blade guards a narrow pass.\nWhen should a defender stand firm?"), {COMPOUND_STRING("For pride"),COMPOUND_STRING("To protect others"),COMPOUND_STRING("For a reward")}},
    {SPECIES_VIRIZION, MAP_ROUTE25, 15, 4, 4, 6, 2, COMPOUND_STRING("ROUTE 25 GROVE"), COMPOUND_STRING("A green blade leaves no scars.\nHow should a forest be defended?"), {COMPOUND_STRING("Burn every path"),COMPOUND_STRING("Cut it all down"),COMPOUND_STRING("With care")}},
    {SPECIES_KELDEO, MAP_SEAFOAM_ISLANDS_B1F, 20, 14, 5, 6, 0, COMPOUND_STRING("SEAFOAM TRAINING POOL"), COMPOUND_STRING("A young sword seeks its purpose.\nWhat unites the three teachings?"), {COMPOUND_STRING("Courage with kindness"),COMPOUND_STRING("Power without limits"),COMPOUND_STRING("Fame above all")}},
    {SPECIES_RAIKOU, MAP_POWER_PLANT, 9, 12, 3, 10, 0, COMPOUND_STRING("POWER PLANT THUNDER TRACE"), COMPOUND_STRING("A storm leaves tracks in the dust.\nWhat makes a trace useful evidence?"), {COMPOUND_STRING("Recording its context"),COMPOUND_STRING("Taking a guess"),COMPOUND_STRING("Ignoring its source")}},
    {SPECIES_ENTEI, MAP_POKEMON_MANSION_2F, 20, 11, 4, 10, 1, COMPOUND_STRING("MANSION FIRE TRACE"), COMPOUND_STRING("Ash preserves an ancient footprint.\nHow should a fragile trace be kept?"), {COMPOUND_STRING("Brush it away"),COMPOUND_STRING("Document it carefully"),COMPOUND_STRING("Keep only rumors")}},
    {SPECIES_SUICUNE, MAP_ROUTE24, 12, 9, 3, 10, 2, COMPOUND_STRING("ROUTE 24 WATER TRACE"), COMPOUND_STRING("Clear water reflects a passing beast.\nWhat should a field study avoid?"), {COMPOUND_STRING("Patient observation"),COMPOUND_STRING("Keeping records"),COMPOUND_STRING("Disturbing the habitat")}},
    {SPECIES_RAGING_BOLT, MAP_ROCK_TUNNEL_B1F, 30, 19, 6, 10, 1, COMPOUND_STRING("ROCK TUNNEL ANCIENT STORM"), COMPOUND_STRING("Ancient thunder echoes a modern roar.\nWhat connects the two traces?"), {COMPOUND_STRING("A required capture"),COMPOUND_STRING("Shared field evidence"),COMPOUND_STRING("An evolution stone")}},
    {SPECIES_GOUGING_FIRE, MAP_MT_EMBER_SUMMIT, 7, 6, 6, 10, 2, COMPOUND_STRING("MT. EMBER ANCIENT FLAME"), COMPOUND_STRING("Old fire leaves a familiar pattern.\nWhat does this ancient beast prove?"), {COMPOUND_STRING("ENTEI must evolve"),COMPOUND_STRING("The present must vanish"),COMPOUND_STRING("The past has its own life")}},
    {SPECIES_WALKING_WAKE, MAP_SEAFOAM_ISLANDS_B4F, 24, 18, 6, 10, 0, COMPOUND_STRING("SEAFOAM ANCIENT CURRENT"), COMPOUND_STRING("An ancient wake resembles clear water.\nHow can both beasts coexist?"), {COMPOUND_STRING("As separate partners"),COMPOUND_STRING("By replacing SUICUNE"),COMPOUND_STRING("Only after a capture")}},
    {SPECIES_DIALGA, MAP_POKEMON_TOWER_7F, 9, 7, 6, 2, 0, COMPOUND_STRING("TOWER TEMPORAL FRACTURE"), COMPOUND_STRING("A fractured moment repeats in stone.\nWhat anchors a study of time?"), {COMPOUND_STRING("A reliable observation"),COMPOUND_STRING("A hurried guess"),COMPOUND_STRING("Forgetting the past")}},
    {SPECIES_PALKIA, MAP_SEAFOAM_ISLANDS_B3F, 21, 18, 6, 2, 1, COMPOUND_STRING("SEAFOAM SPATIAL FRACTURE"), COMPOUND_STRING("Two distant echoes share one place.\nWhat anchors a study of space?"), {COMPOUND_STRING("Ignoring distances"),COMPOUND_STRING("Mapping the connection"),COMPOUND_STRING("Breaking the boundary")}},
    {SPECIES_GIRATINA, MAP_FIVE_ISLAND_LOST_CAVE_ROOM10, 6, 6, 7, 2, 2, COMPOUND_STRING("LOST CAVE DISTORTION SEAM"), COMPOUND_STRING("Time and space bend at this seam.\nHow should the distortion be entered?"), {COMPOUND_STRING("Without preparation"),COMPOUND_STRING("By forcing it apart"),COMPOUND_STRING("With both observations")}},
};
_Static_assert(ARRAY_COUNT(sSites)<=35,"Research bit reservations overlap");
static EWRAM_DATA u8 sFieldSlot=0;
static EWRAM_DATA u8 sFieldProject=0;
extern const u8 GleField_Approach[];
static u16 CurrentMap(void)
{
    return (u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);
}
static bool32 HasBit(u32 bit)
{
    return !!(GymLeaderState()->uniqueObtained[bit/8]&(1u<<(bit%8)));
}
static u32 ProjectCount(u32 project,u32 firstBit)
{
    u32 i,n=0;for(i=0;i<ARRAY_COUNT(sSites);i++)
        if(sSites[i].project==project && HasBit(firstBit+i))n++;
    return n;
}
static bool32 EarnedRank(u32 slot)
{
    const struct GleState *s=GymLeaderState();
    return slot<ARRAY_COUNT(sSites) && max(s->rank,s->highestRank)>=sSites[slot].rank;
}
static bool32 EvidenceReady(u32 slot)
{
    // The Loyal Three's common origin is revealed by all three sites, never
    // by catching a different member of the group.
    if(sSites[slot].project==1 && slot!=15)
        return HasBit(208) && (slot==16 || HasBit(FIELD_FIRST_DISCOVERY+16));
    if(slot==21)return HasBit(FIELD_FIRST_DISCOVERY+18)&&HasBit(FIELD_FIRST_DISCOVERY+19)&&HasBit(FIELD_FIRST_DISCOVERY+20);
    if(slot>=25&&slot<=27)return HasBit(FIELD_FIRST_DISCOVERY+slot-3);
    if(slot==30)return HasBit(FIELD_FIRST_DISCOVERY+28)&&HasBit(FIELD_FIRST_DISCOVERY+29);
    return sSites[slot].project!=13 || ProjectCount(13,FIELD_FIRST_DISCOVERY)==3;
}
void GymLeaderConfigureFieldResearch(struct MapEvents *events,struct ObjectEventTemplate *objects,u32 capacity)
{
    struct GleState *s=GymLeaderState();u32 i,count=events->objectEventCount;
    for(i=0;i<ARRAY_COUNT(sSites);i++)
    {
        const struct GleFieldSite *site=&sSites[i];
        if(site->map!=CurrentMap())continue;
        GleUniqueCapture(s,FIELD_FIRST_DISCOVERY+i);
        s->projectUnlocked[site->project]=1;
        s->projectStage[site->project]=ProjectCount(site->project,FIELD_FIRST_DISCOVERY);
        if(HasBit(FIELD_FIRST_CAPTURE+i) || count>=capacity)continue;
        objects[count++]=(struct ObjectEventTemplate){.localId=FIELD_LOCAL_ID+i,
            .graphicsId=OBJ_EVENT_MON|site->species,.x=site->x,.y=site->y,.elevation=3,
            .movementType=MOVEMENT_TYPE_FACE_DOWN,.script=GleField_Approach};
    }
    events->objectEvents=objects;events->objectEventCount=count;
}
static bool32 StudyCyclizar(void)
{
    u32 i,box;
    if(HasBit(208))return TRUE;
    for(i=0;i<PARTY_SIZE;i++)
        if(GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_SPECIES)==SPECIES_CYCLIZAR
            && !GetMonData(&gParties[B_TRAINER_PLAYER][i],MON_DATA_IS_EGG))goto found;
    for(box=0;box<TOTAL_BOXES_COUNT;box++)for(i=0;i<IN_BOX_COUNT;i++)
        if(GetBoxMonData(&gPokemonStoragePtr->boxes[box][i],MON_DATA_SPECIES)==SPECIES_CYCLIZAR
            && !GetBoxMonData(&gPokemonStoragePtr->boxes[box][i],MON_DATA_IS_EGG))goto found;
    return FALSE;
found:
    GleUniqueCapture(GymLeaderState(),208);return TRUE;
}
void GleFieldNotebook(void)
{
    u32 project=gSpecialVar_Result;gSpecialVar_Result=0;
    if(!IsGymLeaderRun())return;
    sFieldProject=project;
    switch(project)
    {
    case 1:
        if(StudyCyclizar())StringCopy(gStringVar4,COMPOUND_STRING("Paradox of Evolution\nOur live CYCLIZAR study is complete.\lYour partner remains unchanged.\pSurvey ancient traces in MT. MOON\nB2F, then investigate a strange\lsignal in the POWER PLANT.\pKORAIDON and MIRAIDON are separate\npartners, never evolutions.\lNo other capture is required."));
        else StringCopy(gStringVar4,COMPOUND_STRING("Paradox of Evolution\nA rare CYCLIZAR visits ROUTE 16.\pBring one in your party or PC\nfor a non-invasive live study.\lReturn to this notebook to begin.\pYour partner is never consumed\nor evolved by the research.\lEarly field evidence is remembered."));
        break;
    case 2:
        StringCopy(gStringVar4,COMPOUND_STRING("Fractures in Time and Space\nStudy a temporal fracture in\lPOKEMON TOWER 7F and a spatial\lfracture in SEAFOAM B3F.\pTheir observations reveal a\nDistortion seam in LOST CAVE\lROOM 10. No captures are required."));break;
    case 6:
        StringCopy(gStringVar4,COMPOUND_STRING("Swords of Justice\nSeek three teachings in ROCK\lTUNNEL B1F, VICTORY ROAD 2F,\land the ROUTE 25 grove.\pTheir discoveries reveal KELDEO's\ntraining pool in SEAFOAM B1F.\lCatching the teachers is optional."));break;
    case 10:
        StringCopy(gStringVar4,COMPOUND_STRING("Echoes of the Past\nRecord modern traces at the POWER\lPLANT, MANSION 2F and ROUTE 24.\pCorresponding ancient echoes wait\nin ROCK TUNNEL B1F, MT. EMBER's\lsummit and SEAFOAM B4F.\pEach pair shares field evidence.\nCaptures are independent. No beast\levolves into its ancient counterpart."));break;
    case 7:
        StringCopy(gStringVar4,COMPOUND_STRING("Cursed Treasures\nFour old grudges sealed in Kanto.\pSeek leaves in VIRIDIAN FOREST,\nice in SEAFOAM B2F, a vessel in\lROCK TUNNEL 1F, and MANSION fire.\pRead each seal and choose wisely.\nAll four encounters are independent."));break;
    case 8:
        StringCopy(gStringVar4,COMPOUND_STRING("Lakes of the Mind\nThree presences stir near water.\pSeek knowledge on ROUTE 22,\nemotion in SAFARI EAST, and\lwillpower beside ROUTE 12.\pEach lake tests a different virtue.\nNo other capture is required."));break;
    case 9:
        StringCopy(gStringVar4,COMPOUND_STRING("Guardian Shrines\nFour guardians watch over Kanto.\pThunder: ROUTE 10.\nPsychic light: ROUTE 25.\lRoots: SAFARI WEST.\pMist: ROUTE 19.\nRead their seals respectfully.\lEach shrine opens independently."));break;
    case 11:
        StringCopy(gStringVar4,COMPOUND_STRING("Jungle Guardian\nA fierce guardian has been seen\ldeep in VIRIDIAN FOREST.\pIt appears to protect young\nPOKéMON. Investigate its territory\land approach with respect."));break;
    case 13:
        StringCopy(gStringVar4,COMPOUND_STRING("The Toxic Pact\nThree strange chains share an origin.\pTrace the sites on ROUTE 15,\nROUTE 8 and ROUTE 18 in any order.\lEarly evidence is remembered.\pOnce all three sites are known,\nconfront their poisonous pact.\lCaptures are never prerequisites."));break;
    default:return;
    }
    gSpecialVar_Result=1;
}
void GleFieldMenu(void)
{
    const u8 *names[16];u32 i,n=0;
    for(i=0;i<ARRAY_COUNT(sSites);i++)if(sSites[i].project==sFieldProject)
        names[n++]=GetSpeciesName(sSites[i].species);
    names[n++]=COMPOUND_STRING("Back");GymLeaderMenu(names,n,FALSE);
}
void GleFieldReport(void)
{
    u32 selection=gSpecialVar_Result,i,n=0;gSpecialVar_Result=0;
    if(!IsGymLeaderRun())return;
    for(i=0;i<ARRAY_COUNT(sSites);i++)if(sSites[i].project==sFieldProject)
    {
        if(n++!=selection)continue;
        StringCopy(gStringVar1,GetSpeciesName(sSites[i].species));
        StringCopy(gStringVar2,sSites[i].location);
        StringCopy(gStringVar3,HasBit(FIELD_FIRST_CAPTURE+i)?COMPOUND_STRING("CAPTURE RECORDED"):
            HasBit(FIELD_FIRST_SEAL+i)?COMPOUND_STRING("SEAL OPENED"):
            HasBit(FIELD_FIRST_DISCOVERY+i)?COMPOUND_STRING("FIELD EVIDENCE RECORDED"):COMPOUND_STRING("AWAITING EXPLORATION"));
        StringExpandPlaceholders(gStringVar4,COMPOUND_STRING("{STR_VAR_1}\n{STR_VAR_2}\l{STR_VAR_3}\pSurvey rank: "));
        ConvertIntToDecimalStringN(gStringVar1,sSites[i].rank,STR_CONV_MODE_LEFT_ALIGN,1);
        StringAppend(gStringVar4,gStringVar1);
        StringAppend(gStringVar4,COMPOUND_STRING(".\nEarned access survives demotion."));
        gSpecialVar_Result=1;return;
    }
}
void GleFieldInfo(void)
{
    u32 slot=gSpecialVar_LastTalked-FIELD_LOCAL_ID;gSpecialVar_Result=0;
    sFieldSlot=GLE_NONE;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sSites) || CurrentMap()!=sSites[slot].map)return;
    sFieldSlot=slot;
    StringCopy(gStringVar1,GetSpeciesName(sSites[slot].species));
    ConvertIntToDecimalStringN(gStringVar2,GymLeaderLevelCap(),STR_CONV_MODE_LEFT_ALIGN,3);
    ConvertIntToDecimalStringN(gStringVar3,sSites[slot].rank,STR_CONV_MODE_LEFT_ALIGN,1);
    if(HasBit(FIELD_FIRST_CAPTURE+slot)){gSpecialVar_Result=2;return;}
    if(!EarnedRank(slot))return;
    if(!EvidenceReady(slot))
    {
        if(sSites[slot].project==1)StringCopy(gStringVar4,COMPOUND_STRING("The trace is not understood yet.\nBring CYCLIZAR in your party or PC\lto the GLA Paradox notebook.\pStudy MT. MOON's ancient trace\nbefore investigating the future.\lNo other capture is required."));
        else if(sSites[slot].project==6)StringCopy(gStringVar4,COMPOUND_STRING("KELDEO awaits all three teachings.\nSurvey ROCK TUNNEL B1F, VICTORY\lROAD 2F and ROUTE 25 first.\pDiscoveries are enough.\nNo other capture is required."));
        else if(sSites[slot].project==10)StringCopy(gStringVar4,COMPOUND_STRING("The ancient echo lacks context.\nSurvey its modern counterpart's\ltrace first. Consult the notebook.\pA field observation is enough.\nNo other capture is required."));
        else if(sSites[slot].project==2)StringCopy(gStringVar4,COMPOUND_STRING("The distortion is not understood.\nSurvey POKEMON TOWER 7F and\lSEAFOAM B3F first.\pBoth observations are enough.\nNo other capture is required."));
        else StringCopy(gStringVar4,COMPOUND_STRING("A poisonous chain holds firm.\nTrace the pact's other sites on\lROUTE 15, ROUTE 8 and ROUTE 18.\pDiscoveries are enough.\nNo other capture is required."));
        gSpecialVar_Result=4;return;
    }
    gSpecialVar_Result=HasBit(FIELD_FIRST_SEAL+slot)?1:3;
}
void GleFieldClue(void)
{
    if(sFieldSlot<ARRAY_COUNT(sSites))StringCopy(gStringVar4,sSites[sFieldSlot].clue);
}
void GleFieldSealMenu(void)
{
    if(sFieldSlot<ARRAY_COUNT(sSites))GymLeaderMenu(sSites[sFieldSlot].answers,3,FALSE);
    else GymLeaderResumeScriptNextFrame();
}
void GleFieldSolve(void)
{
    u32 choice=gSpecialVar_Result,slot=sFieldSlot;gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sSites) || CurrentMap()!=sSites[slot].map
        || !EarnedRank(slot) || !EvidenceReady(slot) || choice!=sSites[slot].answer)return;
    GleUniqueCapture(GymLeaderState(),FIELD_FIRST_SEAL+slot);gSpecialVar_Result=1;
}
u32 GymLeaderPrepareFieldBattle(void)
{
    u32 slot=sFieldSlot;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sSites) || CurrentMap()!=sSites[slot].map
        || !EarnedRank(slot) || !EvidenceReady(slot) || !HasBit(FIELD_FIRST_SEAL+slot)
        || HasBit(FIELD_FIRST_CAPTURE+slot))return 0;
    ZeroEnemyPartyMons();
    CreateMonWithIVs(&gParties[B_TRAINER_OPPONENT_A][0],sSites[slot].species,GymLeaderLevelCap(),Random32(),OTID_STRUCT_PLAYER_ID,USE_RANDOM_IVS);
    GiveMonInitialMoveset(&gParties[B_TRAINER_OPPONENT_A][0]);gPartiesCount[B_TRAINER_OPPONENT_A]=1;return 1;
}
void GleFieldStart(void)
{
    if(!GymLeaderPrepareFieldBattle()){gSpecialVar_Result=0;GymLeaderResumeScriptNextFrame();return;}
    BattleSetup_StartLegendaryBattle();
}
void GleFieldFinish(void)
{
    u32 slot=sFieldSlot;gSpecialVar_Result=0;
    if(!IsGymLeaderRun() || slot>=ARRAY_COUNT(sSites) || CurrentMap()!=sSites[slot].map || gBattleOutcome!=B_OUTCOME_CAUGHT)return;
    if(GleUniqueCapture(GymLeaderState(),FIELD_FIRST_CAPTURE+slot)==GLE_OK)gSpecialVar_Result=1;
}
