from qa import *
import struct,json
l=struct.unpack('<37I',(ROOT/'v2-layout.bin').read_bytes());v=struct.unpack('<14I',(ROOT/'v2-integration.bin').read_bytes());assert lib.boot(str(repo/'pokefirered.gba').encode());p=symbols['gParties'];s=0x0203e000
def key(k=1,n=30):frames(2,k);frames(n)
def advance(n=70):
 for _ in range(n):key()
def reset():state('pass-world',1);call('VarSet',0x408e,0)
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
# Native saved first trash-switch survives wrong cans and a Gym exit/re-entry.
reset();call('FlagClear',v[0]);call('FlagClear',v[1]);call('VarSet',0x4096,0);warp(44,6,5,12)
one=call('VarGet',v[2]);two=call('VarGet',v[3]);print('Switches',one,two,flush=True)
call('ScriptContext_SetupScript',symbols['VermilionCity_Gym_EventScript_TrashCan'+str(one)]);advance();assert call('VarGet',0x4096)==1
wrong=next(i for i in range(1,16) if i not in [one,two]);call('ScriptContext_SetupScript',symbols['VermilionCity_Gym_EventScript_TrashCan'+str(wrong)]);advance();assert call('VarGet',0x4096)==1 and call('FlagGet',v[1])
warp(38,5,10,10);warp(44,6,5,12);assert call('FlagGet',v[1]) and call('VarGet',v[2])==one and call('VarGet',v[3])==two
call('ScriptContext_SetupScript',symbols['VermilionCity_Gym_EventScript_TrashCan'+str(two)]);advance();assert call('FlagGet',v[0]);print('PASS actual Surge wrong second/re-entry/correct second switch scripts.',flush=True)
# Native aides hand over all five original rewards with an empty Pokédex and no repeats.
entries=[(38,15,'Route2_EastBuilding_EventScript_Aide'),(57,1,'Route11_EastEntrance_2F_EventScript_Aide'),(53,1,'Route10_PokemonCenter_1F_EventScript_Aide'),(59,1,'Route15_WestEntrance_2F_EventScript_Aide'),(60,2,'Route16_NorthEntrance_2F_EventScript_Aide')]
# Route10 Center's map is resolved from the map table instead of guessed.
groups=json.loads((repo/'data/maps/map_groups.json').read_text())
for idx,(g,n,event) in enumerate(entries):
 reset();call('ClearBag');item,flag=v[4+2*idx:6+2*idx];call('FlagClear',flag)
 if idx==2:
  for gi,group in enumerate(groups['group_order']):
   if 'Route10_PokemonCenter_1F_Frlg' in groups[group]:g,n=gi,groups[group].index('Route10_PokemonCenter_1F_Frlg');break
 warp(g,n,3,5);wr('gSpecialVar_LastTalked',1,2);call('ScriptContext_SetupScript',symbols[event]);advance(130)
 assert call('CheckBagHasItem',item,1) and call('FlagGet',flag),(event,item,flag)
 call('RemoveBagItem',item,1);call('ScriptContext_SetupScript',symbols[event]);advance();assert not call('CheckBagHasItem',item,1)
 print('PASS aide reward/replay guard',event,item,flush=True)
# Full PC must reject an egg without charging or marking a purchase.
reset();call('ZeroPlayerPartyMons')
for sp in [59,3,9,6,25,123]:call('ScriptGiveMon',sp,20,0)
for box in range(14):
 for pos in range(30):call('SetBoxMonAt',box,pos,p)
money=rd('gSaveBlock1Ptr')+l[31];call('SetMoney',money,10000);call('VarSet',0x4093,0);wr('gSpecialVar_0x8004',0,2);call('ChaosBuyMysteryEgg');assert rd('gSpecialVar_Result',2)==2 and call('GetMoney',money)==10000 and call('VarGet',0x4093)==0
print('PASS full-party/full-PC egg rejected without money loss or receipt.',flush=True)
# Disabled milestone remains claimable upon returning with care enabled.
reset();call('ClearBag');call('VarSet',0x4092,0);warp(37,17,32,5);assert not call('ChaosTryCareMilestone',32,5);assert call('VarGet',0x4092)&16 and not call('VarGet',0x4092)&2
call('VarSet',0x408e,1);assert call('ChaosTryCareMilestone',32,5);assert call('VarGet',0x4092)&2;assert call('CountTotalItemQuantityInBag',l[20])==999
print('PASS care OFF milestone / later ON claim / candy maximum.',flush=True)
# Play Style page and FINAL EVOLUTION page use real native UIs.
reset();call('CB2_RunSetupForFireRed');frames(220);snap('v2-play-style')
reset();call('ZeroPlayerPartyMons');call('ScriptGiveMon',150,40,0);call('CB2_PartyMenuFromStartMenu');frames(180);key();key();frames(140)
for _ in range(3):key(16,140)
snap('v2-growth-final');key();key(2,180);assert rd(symbols['gMain']+4)==symbols['CB2_UpdatePartyMenu']|1
print('PASS final-stage Growth Summary opens/cycles/exits; Play Style screenshot.',flush=True)
