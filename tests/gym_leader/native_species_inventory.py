"""Inventory linked voucher candidates and dedicated acquisitions by Dex number.

Candidate membership is not a claim that random voucher draws guarantee access.
The JSON deliberately separates missing sources from unverified world access.
"""
import runpy,re,json,struct,hashlib
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
source=(repo/'include/constants/species.h').read_text();ids={}
enum=re.sub(r'//[^\n]*|/\*.*?\*/','',source,flags=re.S).split('{',1)[1].split('}',1)[0]
number=-1
for entry in enum.split(','):
 entry=entry.strip()
 if not entry:continue
 if '=' in entry:
  name,value=map(str.strip,entry.split('=',1));number=int(value) if value.isdigit() else ids[value]
 else:name=entry;number+=1
 ids[name]=number
names={}
for name,number in ids.items():names.setdefault(number,name)
limit=ids['SPECIES_GLE_AWAKENED_SLAKING'];pool=call('Alloc_',4096,0);assert pool
candidates={}
for ty in range(18):
 for kind in range(3):
  count=lib.call6(symbols['GymLeaderBuildSpeciesPool'],ty,8,kind,0,pool,2048)
  assert count!=0xdeadbeef and count<=2048
  for i in range(count):candidates.setdefault(rd(pool+i*2,2),set()).add((ty,kind))
call('Free',pool)
catalog=(repo/'src/data/gym_leader_catalog.h').read_text()
exclusive={ids[n] for n in re.findall(r'SPECIES_\w+',catalog.split('sGleResearchSpecies')[0])}
reserved={ids[n] for n in re.findall(r'SPECIES_\w+',catalog.split('sGleResearchSpecies')[1])}
research=set()
for filename in ['gym_leader_field_research.c','gym_leader_research.c']:
 text=(repo/'src'/filename).read_text()
 # Only actual encounter record rows count; narrative references do not.
 research.update(ids[n] for n in re.findall(r'\{(SPECIES_\w+),\s*MAP_',text))
research.add(ids['SPECIES_TYPE_NULL']);research.add(ids['SPECIES_SILVALLY'])
text=(repo/'src/gym_leader_lineages.c').read_text()
research.update(ids[n] for n in re.findall(r'SPECIES_\w+',text))
metadata=(ROOT/'lineage-layout.bin').read_bytes();c=struct.unpack('<'+'I'*(len(metadata)//4),metadata)
# Evolution candidates are recorded separately: their conditions and reachable
# items are not certified by merely reading a target from the linked table.
cache={}
def descendants(roots):
 reachable=set(roots);derived=set()
 while True:
  added=set()
  for species in reachable:
   if species not in cache:
    targets=[];ptr=call('GetSpeciesEvolutions',species)
    if ptr:
     for i in range(64):
      evo=ptr+i*c[7]
      if rd(evo+c[8],2)==c[9]:break
      target=rd(evo+c[6],2)
      if 0<target<limit:targets.append(target)
    cache[species]=targets
   added.update(t for t in cache[species] if t not in reachable)
  if not added:return derived
  derived.update(added);reachable.update(added)
derived=descendants(set(candidates)|exclusive|research)
directory_file=ROOT/'directory-access.json'
directory=set(json.loads(directory_file.read_text())['species']) if directory_file.exists() else set()
deterministic_evolutions=descendants(directory|exclusive|research) if directory else set()
assert ids['SPECIES_SHAYMIN_SKY'] not in candidates and ids['SPECIES_KELDEO_RESOLUTE'] not in candidates

dexes={}
for species in range(1,limit):
 if rd(symbols['gSpeciesInfo']+species*v[194],1)==0:continue
 dex=call('SpeciesToNationalPokedexNum',species)
 if not dex:continue
 entry=dexes.setdefault(dex,{'dex':dex,'species':[],'voucher_candidates':[],'dedicated_acquisitions':[],'research_reservations':[],'evolution_candidates':[],'placement_directory':[],'deterministic_evolution_candidates':[]})
 name=names.get(species,str(species));entry['species'].append(name)
 if species in candidates:entry['voucher_candidates'].append(name)
 if species in exclusive or species in research:entry['dedicated_acquisitions'].append(name)
 if species in reserved:entry['research_reservations'].append(name)
 if species in derived:entry['evolution_candidates'].append(name)
 if species in directory:entry['placement_directory'].append(name)
 if species in deterministic_evolutions:entry['deterministic_evolution_candidates'].append(name)
missing=[e['dex'] for e in dexes.values() if not e['voucher_candidates'] and not e['dedicated_acquisitions'] and not e['evolution_candidates']]
missing_deterministic=[e['dex'] for e in dexes.values() if not e['placement_directory'] and not e['dedicated_acquisitions'] and not e['deterministic_evolution_candidates']] if directory else None
if directory:assert not missing_deterministic,missing_deterministic
report={'scope':'Linked non-custom species grouped by National Dex; voucher candidate union at tier 8 across 18 types and 3 ordinary categories; actual encounter rows, nursery/Type Null/Silvally placements and linked evolution candidates. Native wild/gift/evolution conditions/travel reachability still requires audit.',
 'rom_sha256':hashlib.sha256((repo/'pokefirered.gba').read_bytes()).hexdigest(),'save_version':rd(current()+20,2),'linked_dex_count':len(dexes),'candidate_species_count':len(candidates),'missing_candidate_or_dedicated_source_dex':missing,
 'complete_coverage_certified':False,'native_directory_species_count':len(directory),'missing_deterministic_source_dex':missing_deterministic,'species':sorted(dexes.values(),key=lambda e:e['dex'])}
(ROOT/'species-inventory.json').write_text(json.dumps(report,indent=2)+'\n')
print('Inventory: linked Dex',len(dexes),'voucher candidates',len(candidates),'missing candidate/dedicated source',[(d,dexes[d]['species'][0]) for d in missing],flush=True)
print('Native directory/dedicated/evolution source gaps:',missing_deterministic,flush=True)
print('PASS native source inventory produced; full campaign and all-form coverage remain uncertified',flush=True)
