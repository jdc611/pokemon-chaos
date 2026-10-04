from pathlib import Path
import re,subprocess,json
root=Path(__file__).resolve().parents[2]
import os
os.chdir(root)
pre='''#define TRUE 1
#define FALSE 0
#include "config/general.h"
#include "config/species_enabled.h"
#include "config/pokemon.h"
#include "config/battle.h"
'''
def pp(inc):
 return subprocess.check_output(['cpp','-P','-iquote','include','-iquote','src','-DFIRERED','-DIS_FRLG=1','-x','c','-'],input=(pre+f'#include "{inc}"\n').encode()).decode()
def entries(s,prefix):
 out={}
 for m in re.finditer(r'\[('+prefix+r'\w+)\]\s*=\s*\{',s):
  depth=1;i=m.end()
  while depth:
   depth+=(s[i]=='{')-(s[i]=='}');i+=1
  out[m[1]]=s[m.end():i-1]
 return out
sp=entries(pp('data/pokemon/species_info.h'),'SPECIES_')
flag=lambda f:[k for k,v in sp.items() if re.search(r'\.'+f+r'\s*=\s*1\b',v)]
items=entries(pp('data/items.h'),'ITEM_')
mega=flag('isMegaEvolution')
stones=[k for k,v in items.items() if '.holdEffect = HOLD_EFFECT_MEGA_STONE' in v]
forms=pp('data/pokemon/form_change_tables.h')
links=re.findall(r'\{FORM_CHANGE_BATTLE_MEGA_EVOLUTION_ITEM,\s*(SPECIES_\w+),\s*(ITEM_\w+)',forms)
links=[(a,b) for a,b in links if a in mega and b in stones]
dex=lambda k:re.search(r'\.natDexNum\s*=\s*(NATIONAL_DEX_\w+)',sp[k])[1]
categories={}
for title,f in [('Restricted legendaries','isRestrictedLegendary'),('Sub-legendaries','isSubLegendary'),('Mythicals','isMythical'),('Ultra Beasts','isUltraBeast'),('Paradox','isParadox')]:
 members=flag(f);bases=sorted(set(dex(k) for k in members));categories[title]={'species':len(bases),'forms':len(members),'names':bases,'members':members}
report=['# Current native FireRed content inventory','', 'Generated from the enabled FireRed preprocessor configuration; alternate forms count separately.','',f'- Mega form records: **{len(mega)}**.',f'- Mega form records with an item transformation: **{len(set(a for a,b in links))}**.',f'- Implemented Mega Stone item types: **{len(stones)}**.',f'- Stones connected to an enabled Mega transformation: **{len(set(b for a,b in links))}**.', '- Mega Rayquaza uses a move rather than a stone.', '- Mega Arcanine exists and is used directly by Blaine. Arcanite is **not implemented** in this master.', '- Hitmonorris is a separate species, not a Mega.', '', '## Special species counts','', '| Category | Distinct Pokédex species | Enabled form records |','|---|---:|---:|']
for t,c in categories.items():report.append(f'| {t} | {c["species"]} | {c["forms"]} |')
report+=['','## Mega forms','']+['- '+k.removeprefix('SPECIES_') for k in mega]
report+=['','## Mega Stone items','']+['- '+k.removeprefix('ITEM_') for k in stones]
for t,c in categories.items():report+=['',f'## {t}','']+['- '+k.removeprefix('NATIONAL_DEX_') for k in c['names']]
report+=['','## Acquisition planning','',f'A minimum of {len(set(b for a,b in links))} distinct stone acquisition routes would cover current item-driven forms. Existing repeatable sources and the Oak/Nidoking rewards must be audited before calculating **new** placements. Arcanite requires its own item implementation before adding a placement. Exact placements remain undecided.','', 'This inventory counts source availability, not a promise that every form is currently obtainable in Kanto.']
Path('docs/chaos-content-inventory.md').write_text('\n'.join(report)+'\n')
print(json.dumps({'megaForms':len(mega),'stoneTypes':len(stones),'linkedForms':len(set(a for a,b in links)),'linkedStones':len(set(b for a,b in links)),'categories':{t:{'species':c['species'],'forms':c['forms']} for t,c in categories.items()}},indent=2))
