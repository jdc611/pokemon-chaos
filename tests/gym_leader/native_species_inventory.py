"""Inventory linked voucher candidates and dedicated acquisitions by Dex number.

Candidate membership is not a claim that random voucher draws guarantee access.
The JSON deliberately separates missing sources from unverified world access.
"""
import runpy,re,json,struct
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
reachable=set(candidates)|exclusive|research;derived=set()
while True:
 added=set()
 for species in reachable:
  ptr=call('GetSpeciesEvolutions',species)
  if not ptr:continue
  for i in range(64):
   evo=ptr+i*c[7]
   if rd(evo+c[8],2)==c[9]:break
   target=rd(evo+c[6],2)
   if 0<target<limit and target not in reachable:added.add(target)
 if not added:break
 derived.update(added);reachable.update(added)
assert ids['SPECIES_SHAYMIN_SKY'] not in candidates and ids['SPECIES_KELDEO_RESOLUTE'] not in candidates

dexes={}
for species in range(1,limit):
 if rd(symbols['gSpeciesInfo']+species*v[194],1)==0:continue
 dex=call('SpeciesToNationalPokedexNum',species)
 if not dex:continue
 entry=dexes.setdefault(dex,{'dex':dex,'species':[],'voucher_candidates':[],'dedicated_acquisitions':[],'research_reservations':[],'evolution_candidates':[]})
 name=names.get(species,str(species));entry['species'].append(name)
 if species in candidates:entry['voucher_candidates'].append(name)
 if species in exclusive or species in research:entry['dedicated_acquisitions'].append(name)
 if species in reserved:entry['research_reservations'].append(name)
 if species in derived:entry['evolution_candidates'].append(name)
missing=[e['dex'] for e in dexes.values() if not e['voucher_candidates'] and not e['dedicated_acquisitions'] and not e['evolution_candidates']]
report={'scope':'Linked non-custom species grouped by National Dex; voucher candidate union at tier 8 across 18 types and 3 ordinary categories; actual encounter rows, nursery/Type Null/Silvally placements and linked evolution candidates. Native wild/gift/evolution conditions/travel reachability still requires audit.',
 'save_version':rd(current()+20,2),'linked_dex_count':len(dexes),'candidate_species_count':len(candidates),'missing_candidate_or_dedicated_source_dex':missing,
 'complete_coverage_certified':False,'species':sorted(dexes.values(),key=lambda e:e['dex'])}
(ROOT/'species-inventory.json').write_text(json.dumps(report,indent=2)+'\n')
print('Inventory: linked Dex',len(dexes),'voucher candidates',len(candidates),'missing candidate/dedicated source',[(d,dexes[d]['species'][0]) for d in missing],flush=True)
print('PASS native candidate inventory produced; full species coverage remains uncertified',flush=True)
