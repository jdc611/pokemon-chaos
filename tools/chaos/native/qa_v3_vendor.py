from qa import *
import struct
v=struct.unpack('<'+str(len((ROOT/'v3-world-layout.bin').read_bytes())//4)+'I',(ROOT/'v3-world-layout.bin').read_bytes());scratch=0x0203e000;p=symbols['gParties']
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=35):frames(2,k);frames(n)
def blob(a,n):return bytes(rd(a+i,1) for i in range(n))
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
for full in [False,True]:
 for nickname in [False,True]:
  state('pass-world',1);call('VarSet',0x408c,0);call('ZeroPlayerPartyMons')
  originals=[59,3,9,6,25,123] if full else [59,3]
  for sp in originals:call('ScriptGiveMon',sp,20,0)
  before=blob(p,100*len(originals));warp(51,0,2,3)
  wr(rd('gSaveBlock3Ptr')+v[1],1,1);wr(rd('gSaveBlock3Ptr')+v[2],123456)
  expected=call('GetRandomizedScriptedSpecies',129,5,0);assert expected!=129
  money=rd('gSaveBlock1Ptr')+v[14];call('SetMoney',money,10000);wr('gSpecialVar_0x8004',0,2);wr('gSpecialVar_Result',1,2);wr('gSpecialVar_LastTalked',2,2)
  call('ScriptContext_SetupScript',symbols['Route4_PokemonCenter_1F_EventScript_TryBuyMagikarp']);frames(100)
  for _ in range(120):
   if call('FuncIsActiveTask',symbols['Task_HandleYesNoInput']|1):break
   key()
  else:raise AssertionError('nickname choice not opened')
  key(1 if nickname else 2,180)
  if nickname:
   assert rd(symbols['gMain']+4)==symbols['CB2_NamingScreen']|1
   selected=call('GetSelectedBoxMonFromPcOrParty');assert call('GetBoxMonData3',selected,v[12],0)==expected
   assert rd('gSpecialVar_0x8004',2)==(254 if full else len(originals))
   call('SetCursorPos',0,0);key();key(8);key(1,300)
  for _ in range(60):key()
  assert blob(p,100*len(originals))==before
  if full:
   selected=call('GetBoxedMonPtr',rd('gSpecialVar_MonBoxId',2),rd('gSpecialVar_MonBoxPos',2))
  else:selected=p+100*len(originals)
  actual=call('GetBoxMonData3',selected,v[12],0);assert actual==expected,(full,nickname,actual,expected)
  assert call('GetMoney',rd('gSaveBlock1Ptr')+v[14])==9500
  assert call('FlagGet',0x249)
  print('PASS randomized Magikarp vendor', 'full party PC' if full else 'open party slot','nickname Yes' if nickname else 'nickname No','actual species',actual,'correct identity and one payment',flush=True)
print('Vendor probes complete',flush=True)
