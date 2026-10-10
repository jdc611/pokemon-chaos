"""Seed/mode/level/BST mapping audit, not evolution-animation playthroughs."""
from qa_ironmon_core import *

checked=0;fallbacks=[]
BST=struct.unpack('<15I',(ROOT/'ironmon-profile-layout.bin').read_bytes())[1]
for choice in (1,2):
    for seed in (1,123456,0xffffffff):
        start(4,seed)
        wr(s+BST,choice,1);call('IronmonInitializeRun')
        for species in range(1,N_SPECIES):
            if not call('IsSpeciesEnabled',species):continue
            target=call('GetRandomEvolutionTargetForSettings',species,4,seed)
            assert target==call('GetRandomEvolutionTargetForSettings',species,5,seed)
            if not target:continue
            assert target!=species and call('IsSpeciesEnabled',target)
            wr('gRngValue',seed ^ species)
            assert target==call('GetRandomEvolutionTargetForSettings',species,4,seed)
            level=call('GetRandomEvolutionLevelForSettings',species,seed)
            assert 2<=level<=100,(species,seed,level)
            assert level==call('GetRandomEvolutionLevelForSettings',species,seed)
            before=call('GetSpeciesBaseStatTotal',species)
            after=call('GetSpeciesBaseStatTotal',target)
            if after<before:fallbacks.append((seed,species,target,before,after))
            checked+=1
        print('PASS randomized evolution mapping profile/seed',choice,seed,flush=True)
assert checked>0
print('PASS',checked,'enabled evolutionary mappings; same Normal/Hardcore target, RNG independence, attainable stable levels.')
print('Documented bounded-pool BST fallbacks:',fallbacks)
