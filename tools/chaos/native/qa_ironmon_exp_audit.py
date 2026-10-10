"""Optional-inclusive available trainer EXP model using real seeded teams and engine scale table.

Assumes victories; no balance/playthrough claim. Excludes optional modifiers
(traded bonus, Lucky Egg, affection, Exp Charm, overdue evolution). Models a
solo main in each growth curve, with no pivots or evolution growth-curve change.
"""
from qa_ironmon_core import *
import re,json,statistics
info_size,yield_offset=struct.unpack('<2I',(ROOT/'ironmon-exp-audit-layout.bin').read_bytes())
ids={k:int(v) for k,v in re.findall(r'^#define\s+(TRAINER_\w+)\s+(\d+)\s*$',
    (repo/'include/constants/opponents_frlg.h').read_text(),re.M)}
def map_trainers(names):
    result=[]
    for name in names:
        text=(repo/'data/maps'/name/'scripts.inc').read_text()
        # Many route trainers live in the shared rematch script file, not in
        # their map scripts. Follow map object references to initial blocks.
        shared=(repo/'data/scripts/trainers_frlg.inc').read_text()
        objects=json.loads((repo/'data/maps'/name/'map.json').read_text())['object_events']
        for obj in objects:
            label=obj.get('script','')
            match=re.search(r'(?m)^'+re.escape(label)+r'::?\n',shared)
            if not match:continue
            rest=shared[match.end():]
            end=re.search(r'(?m)^\w+::?\n',rest)
            text+='\n'+rest[:end.start() if end else len(rest)]
        for trainer in re.findall(r'trainerbattle_\w+\s+(TRAINER_\w+)',text):
            if trainer.endswith('_REMATCH') or trainer not in ids:continue
            if trainer not in result:result.append(trainer)
    return result
forest=map_trainers(['ViridianForest_Frlg'])
brock=['TRAINER_CAMPER_LIAM','TRAINER_LEADER_BROCK']
forest=['TRAINER_RIVAL_OAKS_LAB_CHARMANDER','TRAINER_RIVAL_ROUTE22_EARLY_CHARMANDER']+forest
to_misty=map_trainers(['Route3_Frlg','MtMoon_1F_Frlg','MtMoon_B2F_Frlg','Route4_Frlg'])
to_misty+=['TRAINER_CHAOS_JESSIE_MOON','TRAINER_CHAOS_JAMES_MOON','TRAINER_RIVAL_CERULEAN_CHARMANDER']
to_misty+=map_trainers(['Route24_Frlg','Route25_Frlg','CeruleanCity_Gym_Frlg'])
to_misty=[t for t in to_misty if t!='TRAINER_LEADER_MISTY']+['TRAINER_LEADER_MISTY']
# Gym/lab-only is a conservative EXP lower bound, not an asserted navigable
# route. Story/overworld trainers also add EXP; optional-inclusive assumes all
# listed initial opponents are defeated, including northern routes before Misty.
sequences={'optional_inclusive':forest+brock+to_misty,
           'lab_gym_lower_bound':['TRAINER_RIVAL_OAKS_LAB_CHARMANDER']+brock+
               ['TRAINER_PICNICKER_DIANA','TRAINER_SWIMMER_MALE_LUIS','TRAINER_LEADER_MISTY']}
tables=[[rd(symbols['gExperienceTables']+(growth*101+level)*4) for level in range(101)]
        for growth in range(6)]
scale=[rd(symbols['sExperienceScalingFactors']+i*4) for i in range(211)]
records=[]
for seed in (1,2,3,4,5,123456,24680,0xffffffff):
    start(4,seed);teams={}
    for trainer in dict.fromkeys(forest+brock+to_misty):
        call('CreateNPCTrainerParty',p+6*MONSIZE,ids[trainer])
        team=[]
        for i in range(6):
            mon=p+(6+i)*MONSIZE;species=data(mon,SP)
            if not species:continue
            level=data(mon,LV)
            base=rd(symbols['gSpeciesInfo']+species*info_size+yield_offset,2)
            team.append((species,level,base))
        assert team,(seed,trainer)
        teams[trainer]=team
    for profile,sequence in sequences.items():
        for growth,table in enumerate(tables):
            xp=table[5];level=5;milestones={}
            for trainer in sequence:
                if trainer in ('TRAINER_LEADER_BROCK','TRAINER_LEADER_MISTY'):
                    milestones['before_'+trainer]=level
                for species,enemy_level,base in teams[trainer]:
                    amount=(base*enemy_level)//5
                    amount=(amount*scale[2*enemy_level+10])//scale[enemy_level+level+10]+1
                    xp+=amount
                    while level<100 and xp>=table[level+1]:level+=1
                if trainer in ('TRAINER_LEADER_BROCK','TRAINER_LEADER_MISTY'):
                    milestones['after_'+trainer]=level
            records.append(dict(seed=seed,growth=growth,profile=profile,**milestones))
    print('PASS EXP availability model seed',seed,'trainer identities',len(teams),flush=True)
report=dict(limitations=__doc__,trainers=sequences,
            results=records)
(ROOT/'ironmon-exp-audit.json').write_text(json.dumps(report,indent=2)+'\n')
for profile in sequences:
    for key in records[0]:
        if key in ('seed','growth','profile'):continue
        values=[r[key] for r in records if r['profile']==profile]
        print(profile,key,'minimum',min(values),'median',statistics.median(values),'maximum',max(values))
print('Not a survivability result. Floors remain provisional; real Brock/Misty and later audits pending.')
