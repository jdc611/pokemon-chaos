from pathlib import Path
import re,json,subprocess
root=Path(__file__).resolve().parents[2]
def blocks(paths):
 out={}
 for p in paths:
  s=p.read_text();ms=list(re.finditer(r'^([A-Za-z_][\w]*)::?',s,re.M))
  for i,m in enumerate(ms):out[m[1]]=(str(p.relative_to(root)),s[m.end():ms[i+1].start() if i+1<len(ms) else len(s)])
 return out
paths=list((root/'data/maps').glob('*_Frlg/scripts.inc'))+list((root/'data/scripts').glob('*.inc'))
b=blocks(paths); roots=[]
for p in (root/'data/maps').glob('*_Frlg/map.json'):
 j=json.loads(p.read_text())
 for group in ('object_events','coord_events','bg_events'):
  for e in j.get(group,[]):
   if e.get('script'): roots.append(e['script'])
 roots.append(p.parent.name+'_MapScripts')
visited=set();q=roots[:]
while q:
 x=q.pop()
 if x in visited or x not in b:continue
 visited.add(x);q.extend(set(re.findall(r'\b[A-Za-z_]\w*\b',b[x][1]))&b.keys())
tms=re.findall(r'F\((\w+)\)',(root/'include/constants/tms_hms.h').read_text().split('#define FOREACH_HM')[0]); arc=re.findall(r'ITEM_TM_(\w+)',(root/'src/chaos_arcade_prizes.c').read_text().split('static const struct ArcadeItemPrize sTmPrizes[]')[1].split('};')[0]);sources={m:[] for m in tms}
for x in sorted(visited):
 path,body=b[x]
 for t in re.findall(r'ITEM_TM_(\w+)|ITEM_TM(\d+)\b',body):
  m=t[0] or (tms[int(t[1])-1] if int(t[1])<=len(tms) else '?')
  if m in sources:sources[m].append({'path':path,'label':x})
# The C arcade counter is reached via scripts; keep it distinct from retired vanilla prize-table labels.
for m in arc:sources[m].append({'path':'src/chaos_arcade_prizes.c','label':'Chaos Arcade'})
changes={'U_TURN':'REPLACE — Roar on Route 4; no longer Arcade stock','THUNDER_WAVE':'REPLACE — Attract on Route 24; no longer Arcade stock','TRAILBLAZE':'REPLACE — Secret Power','ACROBATICS':'REPLACE — Aerial Ace on Route 9','KNOCK_OFF':'REPLACE — Taunt in Rocket Hideout B2F','FOUL_PLAY':'REPLACE — Frustration in Rocket Hideout B3F','BODY_PRESS':'REPLACE — Bulk Up in Silph 7F','BULK_UP':'RELOCATE — S.S. Anne 1F Room 2','WILD_CHARGE':'REPLACE — Protect in Power Plant','GUNK_SHOT':'REPLACE — Sludge Bomb in Sevii Rocket Warehouse','PSYSHOCK':'REPLACE — Mr. Psychic reward','ELECTRIC_TERRAIN':'REPLACE — Fresh Water/Light Screen reward','MISTY_TERRAIN':'REPLACE — Soda Pop/Safeguard reward','GRASSY_TERRAIN':'REPLACE — Lemonade/Reflect reward','DRAINING_KISS':'REPLACE — Celadon 2F Brick Break stock','CHILLING_WATER':'REPLACE — Celadon 2F Dig stock','MACH_PUNCH':'REPLACE — physical karate tutor, reusable TM','VACUUM_WAVE':'REPLACE — special karate tutor, reusable TM'}
rows=[];dups=[]
for n,m in enumerate(tms,1):
 normal=[x for x in sources[m] if x['label']!='Chaos Arcade']
 if m in arc and normal:dups.append((m,normal))
 rows.append({'number':n,'move':m,'decision':changes.get(m,'KEEP' if sources[m] else 'No deliberate native acquisition; random TM pool only'),'sources':sources[m]})
(root/'docs/chaos-tm-audit.json').write_text(json.dumps(rows,indent=2)+'\n')
md=['# FireRed stabilization TM acquisition audit','',f'{len(tms)} supported reusable TMs; {len(arc)} unique Arcade TMs. This catalog follows native FireRed map object/coordinate/transition scripts and their referenced reward/stock labels. Retired vanilla Arcade tables and Hoenn-only rewards are excluded. Random Items may independently yield supported TMs.','', '| TM | Move | Decision | Acquisition |','|---|---|---|---|']
for r in rows:md.append(f"| {r['number']} | {r['move'].replace('_',' ').title()} | {r['decision']} | "+'; '.join(x['label'].replace('_',' ') for x in r['sources'])+' |')
(root/'docs/chaos-tm-audit.md').write_text('\n'.join(md)+'\n')
print('reachable',len(visited),'tmcount',len(tms),'nativeRewardTMs',sum(bool(r['sources']) for r in rows),'Arcade duplicates',dups)
shop=(root/'data/maps/CeladonCity_DepartmentStore_4F_Frlg/scripts.inc').read_text().split('ChaosCeladonSpecialStock_Items::')[1].split('ITEM_NONE')[0];stock=re.findall(r'ITEM_(\w+)',shop);print('mega shop',len(stock)-1,'plusAbilityPatch')
