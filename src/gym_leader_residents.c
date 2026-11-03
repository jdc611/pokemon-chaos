#include "global.h"
#include "gym_leader.h"
#include "gym_leader_script.h"
#include "event_data.h"
#include "money.h"
#include "script.h"
#include "string_util.h"
#include "constants/maps.h"

static EWRAM_DATA u8 sResidentTask=0;
static bool32 AtHomeTown(void)
{
    static const u16 towns[]={MAP_SAFFRON_CITY,MAP_CELADON_CITY,MAP_FUCHSIA_CITY};
    u16 map=(u8)gSaveBlock1Ptr->location.mapNum|((u8)gSaveBlock1Ptr->location.mapGroup<<8);
    return IsGymLeaderRun() && map==towns[GymLeaderState()->town];
}
static const u8 *const sRequests[]={
    COMPOUND_STRING("A POKéMON CENTER would make\nnew families feel safe here.\pIf you build one, our neighbors\ncan contribute 500 POKéDOLLARS.\lAn existing CENTER counts too."),
    COMPOUND_STRING("I'd love a place for wild partners\nto thrive alongside our town.\pBuild any type of HABITAT and\nour nature club will contribute\l1,000 POKéDOLLARS."),
    COMPOUND_STRING("Travelers need somewhere to eat.\nA RESTAURANT would help us all.\pIf you open one, the merchants\nwill contribute 800 POKéDOLLARS."),
    COMPOUND_STRING("Our town needs a place to preserve\nits stories. Could we have a MUSEUM?\pThe history society can offer\n1,200 POKéDOLLARS once it opens."),
    COMPOUND_STRING("I found a visitor's lost purse.\nIts owner is looking for it.\pReturning it earns a 400 reward.\nWe could keep its 1,500 instead...\lWhat should I do?"),
    COMPOUND_STRING("A hungry youngster stole food,\nbut has offered to make amends.\pThe merchant offers 400 for a\nsecond-chance arrangement, or 800\lif you banish the youngster.\pHow should we settle this?"),
    COMPOUND_STRING("A resident uncovered an old relic.\nA museum donor offers 1,000\lto preserve it for the public.\pA secretive collector offers 5,000\nto take it away without records.\lWhich offer should we accept?"),
    COMPOUND_STRING("A merchant offers 2,000 if we\nban all competitors from town.\pWe can refuse and keep trade fair,\nor accept this private payment.\lWhat do you think?")
};
static const u8 *const sChoices[4][3]={
    {COMPOUND_STRING("Return the purse"),COMPOUND_STRING("Keep the money"),COMPOUND_STRING("Decide later")},
    {COMPOUND_STRING("Second chance"),COMPOUND_STRING("Banish them"),COMPOUND_STRING("Decide later")},
    {COMPOUND_STRING("Public collection"),COMPOUND_STRING("Unrecorded sale"),COMPOUND_STRING("Decide later")},
    {COMPOUND_STRING("Keep trade fair"),COMPOUND_STRING("Take the payment"),COMPOUND_STRING("Decide later")}
};
void GleResidentRequestInfo(void)
{
    struct GleState *s=GymLeaderState();u32 request=gSpecialVar_LastTalked-1;int status;
    gSpecialVar_Result=0;sResidentTask=255;
    if(!AtHomeTown() || request>=8 || request>=s->population/3)return;
    sResidentTask=request;status=GleResidentTaskReady(s,request);
    if(status==GLE_DUPLICATE){gSpecialVar_Result=3;return;}
    StringCopy(gStringVar4,sRequests[request]);
    gSpecialVar_Result=request>=4?4:status==GLE_OK?2:1;
}
void GleResidentRequestChoices(void)
{
    if(sResidentTask>=4 && sResidentTask<8)GymLeaderMenu(sChoices[sResidentTask-4],3,FALSE);
    else GymLeaderResumeScriptNextFrame();
}
void GleResidentRequestResolve(void)
{
    u32 choice=gSpecialVar_Result,money;
    gSpecialVar_Result=GLE_INVALID;
    if(!AtHomeTown() || sResidentTask>=8 || choice>1)return;
    money=GetMoney(&gSaveBlock1Ptr->money);
    gSpecialVar_Result=GleResolveResidentTask(GymLeaderState(),sResidentTask,choice,&money);
    if(gSpecialVar_Result==GLE_OK)SetMoney(&gSaveBlock1Ptr->money,money);
}
void GleTownStandingInfo(void)
{
    struct GleState *s=GymLeaderState();
    StringCopy(gStringVar1,s->honor>=100?COMPOUND_STRING("HONORABLE"):s->honor<=-100?COMPOUND_STRING("NOTORIOUS"):COMPOUND_STRING("NEUTRAL"));
    if(s->honor>=100)StringCopy(gStringVar4,COMPOUND_STRING("Your standing is HONORABLE.\nGLA supplies and tutor unlocks\lcost 10% less.\pResidents notice how you lead.\nYour Gym Rank and Town Reputation\lremain separate measures."));
    else if(s->honor<=-100)StringCopy(gStringVar4,COMPOUND_STRING("Your standing is NOTORIOUS.\nAce trainers refuse recruitment.\lGLA services use normal prices.\pThe Black Market building becomes\navailable through town planning.\lYour Gym Rank is separate."));
    else StringCopy(gStringVar4,COMPOUND_STRING("Your standing is NEUTRAL.\nGLA services use normal prices.\lAce trainers judge you normally.\pHelping residents can improve it.\nShady decisions can lower it.\lThis is separate from Gym Rank."));
}
