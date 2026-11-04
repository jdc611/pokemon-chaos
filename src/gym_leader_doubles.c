#include "global.h"
#include "gym_leader.h"
#include "gym_leader_doubles.h"
#include "event_data.h"
#include "money.h"
#include "string_util.h"

// Append-only unique ledger: 224..231 are per-rank double victories;
// 232..239 retain one uncollected voucher at its original earned rank.
static bool32 Bit(u32 n){return !!(GymLeaderState()->uniqueObtained[n/8]&(1u<<(n%8)));}
static void Set(u32 n){GymLeaderState()->uniqueObtained[n/8]|=1u<<(n%8);}
static void Collect(void)
{
    struct GleState *s=GymLeaderState();u32 rank,bit,kind;
    for(rank=1;rank<=8;rank++)if(Bit(bit=231+rank)){
        kind=rank<=3?GLE_STANDARD:rank<=6?GLE_RARE:GLE_ELITE;
        if(GleAwardVoucher(s,kind,rank)!=GLE_OK)return;
        s->uniqueObtained[bit/8]&=~(1u<<(bit%8));
    }
}
bool32 GymLeaderDoubleAvailable(void){return IsGymLeaderRun()&&!Bit(223+GymLeaderState()->rank);}
void GymLeaderSettleRivalDouble(bool32 won)
{
    struct GleState *s=GymLeaderState();u32 bit=223+s->rank;if(!won||!GymLeaderDoubleAvailable())return;
    Set(bit);Set(231+s->rank);AddMoney(&gSaveBlock1Ptr->money,2000*s->rank);s->reputation=min(65535,s->reputation+4);Collect();
}
void GleRivalDoubleInfo(void)
{
    gSpecialVar_Result=2;if(!IsGymLeaderRun())return;Collect();
    ConvertIntToDecimalStringN(gStringVar2,GymLeaderLevelCap(),STR_CONV_MODE_LEFT_ALIGN,3);
    ConvertIntToDecimalStringN(gStringVar3,GymLeaderPartyLimit(),STR_CONV_MODE_LEFT_ALIGN,1);
    gSpecialVar_Result=GymLeaderDoubleAvailable()?0:1;
}
