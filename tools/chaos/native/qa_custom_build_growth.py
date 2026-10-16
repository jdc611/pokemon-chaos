"""Real normal evolution, permanent Mega animation and manual growth allocation."""
from qa_custom_build_ui import *
(BULBA,IVY,VENU,MEGAVENU,SCYTHER,SCIZOR,KLEAVOR,VENUSTONE,RING,LEFTOVERS,END_FORM,FAINT_FORM,MEGA,Z,DYNA,TERA,ZRING,DBAND,TORB,TSHARD,ORAN,ABILITY_SLOT,FILL1,FILL2,STORAGE_SIZE,S1SIZE,S2SIZE,S3SIZE,BOXSIZE)=struct.unpack('<29I',(ROOT/'custom-extra-layout.bin').read_bytes())
assert (STORAGE_SIZE,S1SIZE,S2SIZE,S3SIZE,BOXSIZE)==(35692,15872,3968,1616,80)
state('custom-ui-six',1)
call('OaksDraftFinishOpening');call('VarSet',LABSCENE,4)
work=scratch+768;r=work+MS
call6('CustomBuildCandidate',work,r,0,BULBA,16)
assert call6('CustomBuildCommit',work,r,255,0,0)
mon=p
record=call('CustomBuildFind',data(mon,PID));pid=data(mon,PID);ability=rd(record+22,2);final=rd(record+16,2)
def finish_growth(label):
 global record
 for step in range(250):
  frames(8,2);frames(25)
  if ui():break
 assert ui() and page()==4,(label,'growth missing')
 frames(100);snap(label+'-ability');assert rd(ui()+REC+22,2)==ability
 tap(1);assert page()==6
 old=[rd(ui()+FLOORS+i*2,2) for i in range(6)]
 assert old==[rd(record+4+i*2,2) for i in range(6)]
 gain=left();assert gain>0
 snap(label+'-growth');tap(1);tap(32)
 assert [rd(ui()+REC+4+i*2,2) for i in range(6)]==old # cannot subtract old points
 for _ in range(100):
  if not left():break
  tap(256,3)
 assert left()==0
 frames(20);snap(label+'-allocation');tap(8);tap(1);tap(1,180)
 assert not ui()
 record=call('CustomBuildFind',pid)
 assert rd(record+22,2)==ability and data(mon,PID)==pid,(label,record,rd(record+22,2),ability,data(mon,PID),pid)
 assert all(rd(record+4+i*2,2)>=old[i] for i in range(6))
 return gain
wr('gCB2_AfterEvolution',symbols['CB2_ReturnToField']|1)
call('EvolutionScene',mon,IVY,0,0)
gain1=finish_growth('custom-ivy');assert data(mon,SP)==IVY
call('EvolutionScene',mon,VENU,0,0)
gain2=finish_growth('custom-venusaur');assert data(mon,SP)==VENU
assert call('CustomBuildSum',record)==final
print('PASS actual two normal evolution animations, keep-ability choice, positive manually allocated growth and exact final potential',gain1,gain2,flush=True)
# Original stone and Mega Ring requirements, including the one-active-Mega rule.
assert call('CustomBuildMegaTarget',mon,VENUSTONE)==0
call('AddBagItem',VENUSTONE,1);call('AddBagItem',RING,1)
assert call('CustomBuildMegaTarget',mon,VENUSTONE)==MEGAVENU
# Open a genuine bag task, dispatch the stone's field-use handler, and choose
# the real party slot. This exercises callback ownership and party cleanup.
call('SetMainCallback2',symbols['CB2_BagMenuFromStartMenu']|1);frames(240)
task=call('FindTaskIdByFunc',symbols['Task_BagMenu_HandleInput']|1)
assert task<16
wr('gSpecialVar_ItemId',VENUSTONE,2)
call('ItemUseOutOfBattle_CustomMega',task);frames(240)
snap('custom-mega-party-use');tap(1,80)
gain3=finish_growth('custom-mega');assert 40<=gain3<=160
assert data(mon,SP)==MEGAVENU and call('PlayerPartyHasPermanentMega')
assert not call('TryFormChange',mon,END_FORM,0) and not call('TryFormChange',mon,FAINT_FORM,0)
assert data(mon,SP)==MEGAVENU
setdata(mon,HELD,LEFTOVERS);assert data(mon,HELD)==LEFTOVERS
call('SavePlayerParty');assert call('TrySavingData',0)==1
call('ClearSav3');assert call('LoadGameSave',0)==1;call('LoadPlayerParty')
assert data(mon,SP)==MEGAVENU and call('GetMonAbility',mon)==ability, (data(mon,SP),MEGAVENU,call('GetMonAbility',mon),ability,data(mon,PID),pid,call('IsCustomBuild'),call('CustomBuildFind',pid))
print('PASS permanent Mega growth/ability/allocation, ordinary held item, no faint/end-battle reversion and flash persistence',gain3,flush=True)
# Boxing and returning uses the same per-individual bases and ability.
call('SetBoxMonAt',0,0,mon);box=call('GetBoxedMonPtr',0,0)
call('BoxMonToMon',box,work)
assert call('CustomBuildBaseStat',work,0)==call('CustomBuildBaseStat',mon,0)
assert call('GetMonAbility',work)==ability
print('PASS boxed copy retains individual base stats and chosen ability',flush=True)
state('custom-evolved-mega')
