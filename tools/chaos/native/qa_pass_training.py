from qa import *
p=symbols['gParties'];scratch=0x0203e000
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
def key(k=1,n=25):frames(2,k);frames(n)
def advance(n):
 for _ in range(n):key()
def warp(g,n,x,y):
 call('SetWarpDestinationToMapWarp',g,n,255);wr(symbols['sWarpDestination']+4,x,2);wr(symbols['sWarpDestination']+6,y,2);call('DoWarp');frames(230)
# Post-Brock aide's actual interaction unlocks both existing tools.
call('FlagSet',0x820);warp(38,18,40,17);call('ScriptContext_SetupScript',symbols['PewterCity_EventScript_RunningShoesAide']);advance(125)
assert call('VarGet',0x408f)==1
state('pass-postbrock');snap('qa-pass-postbrock');print('PASS actual post-Brock aide script and saved training/relearner unlock.',flush=True)
# Whole party includes Caterpie's two native level evolutions and an unchanged above-cap partner.
call('ZeroPlayerPartyMons');call('ScriptGiveMon',10,5,0);call('ScriptGiveMon',25,8,0);call('ScriptGiveMon',59,30,0);call('ScriptGiveMon',19,5,0)
call('VarSet',0x408c,1);wr(scratch,0);call('SetMonData',p+300,10,scratch)
originalmoves=[call('GetMonData3',p,19+i,0) for i in range(4)]
call('ChaosTrainWholeParty');advance(500)
snap('qa-pass-train-whole')
print('Train CB',hex(rd(symbols['gMain']+4)),'active',call('ChaosTrainToCapActive'),'mon',[(call('GetMonData3',p+i*100,18,0),call('GetMonData3',p+i*100,64,0),call('GetMonData3',p+i*100,10,0)) for i in range(4)],flush=True)
assert not call('ChaosTrainToCapActive')
assert call('GetMonData3',p,18,0)==12 and call('GetMonData3',p,64,0)==22
assert call('GetMonData3',p+100,64,0)==22 and call('GetMonData3',p+200,64,0)==30
assert call('GetMonData3',p+300,64,0)==5 and call('GetMonData3',p+300,10,0)==0
assert rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
print('PASS whole-party cap, Caterpie -> Metapod -> Butterfree, above-cap unchanged, Nuzlocke dead skipped, field return.',flush=True)
state('pass-trained')
# One-mon picker: selected partner only, followed by cancel path.
state('pass-postbrock',1);call('ZeroPlayerPartyMons');call('ScriptGiveMon',25,5,0);call('ScriptGiveMon',19,5,0)
call('ChooseMonForTrainToCap');frames(120);key(128);key();advance(120)
assert call('GetMonData3',p,64,0)==5 and call('GetMonData3',p+100,64,0)==22
call('ChooseMonForTrainToCap');frames(120);key(2);advance(80)
assert call('GetMonData3',p,64,0)==5 and rd(symbols['gMain']+4)==symbols['CB2_Overworld']|1
print('PASS selected-one training and cancel, with other party slots unchanged.',flush=True)
