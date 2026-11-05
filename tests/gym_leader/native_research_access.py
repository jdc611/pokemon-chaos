"""Terrain/connectivity audit, not a claim of a complete walked campaign."""
import runpy,json,collections,hashlib
ctx=runpy.run_path('tests/gym_leader/native_opening.py');globals().update(ctx)
sites=json.load(open('docs/gym-leader-edition/field-research-sites.json'))
# Read map records by their authoritative IDs; storage directory names are not IDs.
maps={}
layouts={d['id']:d for d in json.load(open('data/layouts/layouts.json'))['layouts']}
for path in (repo/'data/maps').glob('*/map.json'):
 d=json.loads(path.read_text());maps[d['id']]=d
edges=collections.defaultdict(set)
for key,d in maps.items():
 for w in d.get('warp_events',[]):
  if w['dest_map'] in maps:edges[key].add(w['dest_map'])
 for con in d.get('connections') or []:
  if con['map'] in maps:edges[key].add(con['map'])
# Ferry edges are transport assumptions; Five Island's actual sailing has its own test.
harbors=[key for key in maps if key.endswith('_HARBOR') and any(n in key for n in ['ONE_ISLAND','TWO_ISLAND','THREE_ISLAND','FOUR_ISLAND','FIVE_ISLAND','SIX_ISLAND','SEVEN_ISLAND'])]
for key in ['MAP_VERMILION_CITY',*harbors]:edges[key].update(harbors+['MAP_VERMILION_CITY'])
reachable={'MAP_SAFFRON_CITY'};queue=collections.deque(reachable)
while queue:
 for dest in edges[queue.popleft()]:
  if dest not in reachable:reachable.add(dest);queue.append(dest)
wr(current()+v[2],8,1);wr(current()+v[-1],8,1);wr('sWildEncountersDisabled',1,1)
for trainer in range(768):call('SetTrainerFlag',trainer)
results=[]
for site in sites:
 key=site['map'];d=maps[key];m=site['map_id']
 lib.call6(symbols['SetWarpDestination'],m>>8,m&255,-1,site['x'],site['y']+1,0);call('WarpIntoMap');call('SetMainCallback2',symbols['CB2_LoadMap']|1);frames(500)
 actual=rd('gSaveBlock1Ptr')+v[25];assert (rd(actual,1)<<8|rd(actual+1,1))==m
 width=rd('gBackupMapLayout');height=rd(symbols['gBackupMapLayout']+4);p=rd(symbols['gBackupMapLayout']+8)
 assert 14<width<512 and 14<height<512 and p
 expected=layouts[d['layout']];assert (width-15,height-14)==(expected['width'],expected['height']),site
 blocks=[rd(p+i*2,2) for i in range(width*height)]
 def free(x,y):return 7<=x<width-8 and 7<=y<height-7 and not (blocks[y*width+x]&0xc00)
 # Every inbound native warp contributes its actual destination tile.
 seeds=set()
 for source in reachable:
  for w in maps[source].get('warp_events',[]):
   if w['dest_map']!=key:continue
   index=w['dest_warp_id']
   if str(index).isdigit() and int(index)<len(d.get('warp_events',[])):
    target=d['warp_events'][int(index)];x,y=target['x']+7,target['y']+7
    for dx,dy in [(0,0),(1,0),(-1,0),(0,1),(0,-1)]:
     if free(x+dx,y+dy):seeds.add((x+dx,y+dy))
 for con in d.get('connections') or []:
  if con['map'] not in reachable:continue
  direction={'up':'north','down':'south','left':'west','right':'east'}.get(con['direction'],con['direction'])
  if direction in ['north','south']:
   y=7 if direction=='north' else height-8
   seeds.update((x,y) for x in range(7,width-7) if free(x,y))
  elif direction in ['east','west']:
   x=width-9 if direction=='east' else 7
   seeds.update((x,y) for y in range(7,height-7) if free(x,y))
 seen=set(seeds);q=collections.deque(seeds)
 while q:
  x,y=q.popleft()
  for dx,dy in [(0,1),(0,-1),(1,0),(-1,0)]:
   point=x+dx,y+dy
   if point not in seen and free(*point):seen.add(point);q.append(point)
 target=site['x']+7,site['y']+8
 used_ledges=False
 if target not in seen:
  # Native directional ledges cross a blocked tile and land two cells away.
  q=collections.deque(seen);jump_cache={}
  directions=[(0,1,'South'),(0,-1,'North'),(1,0,'East'),(-1,0,'West')]
  while q:
   x,y=q.popleft()
   for dx,dy,name in directions:
    point=x+dx,y+dy
    if not free(*point):
     landing=x+2*dx,y+2*dy
     if not free(*landing):continue
     cachekey=(*point,name)
     if cachekey not in jump_cache:jump_cache[cachekey]=call('MetatileBehavior_IsJump'+name,call('MapGridGetMetatileBehaviorAt',*point))
     if not jump_cache[cachekey]:continue
     point=landing;used_ledges=True
    if point not in seen:seen.add(point);q.append(point)
 (ROOT/('terrain-'+str(site['index'])+'.json')).write_text(json.dumps({'width':width,'height':height,'blocks':blocks,'seeds':sorted(seeds),'target':target,'site':site}))
 result={'index':site['index'],'map':key,'species':site['species'],'graph_reachable':key in reachable,'entry_tiles':len(seeds),'approach_component_reachable':target in seen,'component_tiles':len(seen),'includes_directional_ledges':used_ledges}
 results.append(result);print('ACCESS',result,flush=True)
report={'rom_sha256':hashlib.sha256((repo/'pokefirered.gba').read_bytes()).hexdigest(),'scope':'Static warp/connection graph with assumed ferry edges, plus loaded native collision-grid components. Actors, elevation transitions, Surf actions and puzzle/story prerequisites are not fully simulated. Direct warp used to inspect each map; not a controller-walked campaign.','ordinary_trainers_completed_fixture':True,'native_map_dimensions_verified':True,'results':results}
(ROOT/'research-access-audit.json').write_text(json.dumps(report,indent=2)+'\n')
print('AUDIT COMPLETE',sum(r['approach_component_reachable'] and r['graph_reachable'] for r in results),'/',len(results),flush=True)

assert len(results)==36 and all(r['approach_component_reachable'] and r['graph_reachable'] for r in results)
