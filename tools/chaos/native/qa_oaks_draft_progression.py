"""Existing HM ownership/badge rules and real evolution identity preservation."""
from qa_oaks_draft import *
(CPTR,CUT,SURF,FLY,HMSURF,HMFLY,BULBA,IVY,NINCADA,NINJASK,EXP,N_SPECIES,PICHU,PIKA,EEVEE,VAPOREON,WATER)=struct.unpack('<17I',(ROOT/'draft-extra-layout.bin').read_bytes())
start()
for i in range(6):assert call('OaksDraftCommit',i%3)
call('OaksDraftFinishOpening')
for field,hm,badge in [(CUT,HMCUT,BADGE2),(SURF,HMSURF,0x824),(FLY,HMFLY,0x822)]:
 call('FlagSet',badge)
 for i in range(6):
  for move in range(4):call('SetMonMoveSlot',p+i*MS,165,move)
 # Execute the actual script-command handler with the bytecode operands.
 for j in range(160):wr(scratch+j,0,1)
 wr(scratch+CPTR,scratch+160);wr(scratch+160,field,1);wr(scratch+161,1,1)
 call('ScrCmd_checkfieldmove',scratch);assert rd('gSpecialVar_Result',2)==6
 call('AddBagItem',hm,1)
 wr(scratch+CPTR,scratch+160);call('ScrCmd_checkfieldmove',scratch);assert rd('gSpecialVar_Result',2)==0
 print('PASS field HM requires ownership/badge, never a compatible learned move',field,flush=True)
# Level evolution: controlled Bulbasaur identity goes through the real animation.
call('CreateRandomMon',p,BULBA,16);call('OaksDraftSetNickname',p,0)
name_before=data(p,PID);call('GetMonData3',p,NICK,scratch);nickname=bytesat(scratch,12)
call('SetCoins',2000);assert call('OaksDraftRerollAbility',0)
purchased=call('GetMonAbility',p)
call('SetMainCallback2',symbols['CB2_Overworld']|1);call('ScriptContext_Init');call('UnlockPlayerFieldControls')
call('EvolutionScene',p,IVY,0,0)
for i in range(200):frames(3,1);frames(30)
assert data(p,SP)==IVY and data(p,PID)==name_before and data(p,MEMBER)
assert data(p,REROLLED) and call('GetMonAbility',p)==purchased
call('GetMonData3',p,NICK,scratch);assert bytesat(scratch,12)==nickname
print('PASS actual level-evolution animation keeps identity and permanent nickname',flush=True)
# Item evolution eligibility comes from the existing Water Stone party flow.
call('CreateRandomMon',p,EEVEE,20);call('OaksDraftSetNickname',p,0);call('AddBagItem',WATER,1)
call('EvolutionScene',p,VAPOREON,0,0)
for i in range(200):frames(3,1);frames(30)
assert data(p,SP)==VAPOREON and data(p,MEMBER)
print('PASS actual item-evolution animation preserves Draft membership',flush=True)
# A split evolution must not create a seventh/new member or fill a Nuzlocke vacancy.
call('ZeroMonData',p+5*MS);call('CalculatePlayerPartyCount')
call('CreateRandomMon',p,NINCADA,20);call('OaksDraftSetNickname',p,0)
call('EvolutionScene',p,NINJASK,0,0)
for i in range(200):frames(3,1);frames(30)
assert data(p,SP)==NINJASK and rd('gPartiesCount',1)==5
print('PASS Nincada evolution cannot bypass the permanent-roster acquisition rule',flush=True)
