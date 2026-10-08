from qa import *
import struct
l=struct.unpack('<37I',(ROOT/'v2-layout.bin').read_bytes());z=struct.unpack('<11I',(ROOT/'battle-layout.bin').read_bytes());p=symbols['gParties'];b=symbols['gBattleMons'];scratch=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode())
def key(k=1,n=25):frames(2,k);frames(n)
for on,mgm,atcap in [(1,0,False),(2,0,False),(1,1,False),(1,0,True)]:
 state('pass-world',1);call('FlagClear' if atcap else 'FlagSet',0x820);call('VarSet',0x4091,on);wr(rd('gSaveBlock3Ptr')+l[30],mgm,1);call('ZeroPlayerPartyMons')
 for sp in [59,59,59]:call('ScriptGiveMon',sp,15 if atcap else 10,0)
 wr(scratch,0);call('SetMonData',p+200,10,scratch)
 call('SetMonMoveSlot',p,129,0)
 before=[call('GetMonData3',p+100*i,32,0) for i in range(3)]
 call('CreateWildMon',19,5);call('BattleSetup_StartWildBattle');frames(600)
 for n in range(12000):
  if rd('gBattlerControllerFuncs')==symbols['HandleInputChooseAction']|1:break
  frames(1,1 if n%25==0 else 0)
 wr(b+z[0]+z[3],1,2)
 for _ in range(150):key()
 after=[call('GetMonData3',p+100*i,32,0) for i in range(3)];gains=[after[i]-before[i] for i in range(3)]
 evs=[[call('GetMonData3',p+100*i,l[13]+j,0) for j in range(6)] for i in range(3)]
 print('EXP',on,'MGM',mgm,'gains',gains,'EVs',evs,flush=True)
 if atcap:
  assert gains==[0,0,0];print('PASS strict-cap participants/nonparticipants receive zero EXP.',flush=True);continue
 assert gains[0]>0 and gains[2]==0
 if on==1:assert abs(gains[0]-gains[1]*2)<=2 and gains[1]>0
 else:assert gains[1]==0
 if mgm:assert sum(sum(x) for x in evs)==0
 elif on==1:assert evs[0]==evs[1] and sum(evs[0])>0 and sum(evs[2])==0
print('PASS native EXP All full participant / half nonparticipant / fainted exclusion / OFF / normal EVs / MGM.',flush=True)
