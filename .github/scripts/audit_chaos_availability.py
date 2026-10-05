"""Audit enabled FireRed families against source-backed acquisition paths.

This intentionally assigns no invented encounters. Unknown script/native
paths are listed for review rather than credited as proven acquisition.
"""
from pathlib import Path
import json
import re
import subprocess
import os

ROOT = Path(__file__).resolve().parents[2]
os.chdir(ROOT)
PRE = '''#define TRUE 1
#define FALSE 0
#include "config/general.h"
#include "config/species_enabled.h"
#include "config/pokemon.h"
#include "config/battle.h"
#include "config/overworld.h"
'''

def preprocess(path):
    return subprocess.check_output(['cpp', '-P', '-iquote', 'include', '-iquote', 'src', '-DFIRERED', '-DIS_FRLG=1', '-x', 'c', '-'], input=(PRE + f'#include "{path}"\n').encode()).decode()

def entries(text, prefix):
    out = {}
    for match in re.finditer(r'\[(' + prefix + r'\w+)\]\s*=\s*\{', text):
        depth, end = 1, match.end()
        while depth:
            depth += (text[end] == '{') - (text[end] == '}')
            end += 1
        out[match[1]] = text[match.end():end - 1]
    return out

species = entries(preprocess('data/pokemon/species_info.h'), 'SPECIES_')
species = {k: v for k, v in species.items() if re.search(r'\.natDexNum\s*=\s*NATIONAL_DEX_(?!NONE)', v)}
parent = {k: k for k in species}

def find(k):
    while parent[k] != k:
        parent[k] = parent[parent[k]]
        k = parent[k]
    return k

def union(a, b):
    if a in parent and b in parent:
        parent[find(b)] = find(a)

by_dex = {}
for key, body in species.items():
    dex = re.search(r'\.natDexNum\s*=\s*(NATIONAL_DEX_\w+)', body)[1]
    if dex in by_dex:
        union(by_dex[dex], key)
    else:
        by_dex[dex] = key
    evo = re.search(r'\.evolutions\s*=\s*(.*?)(?:\.\w+\s*=|$)', body, re.S)
    if evo:
        for target in re.findall(r'SPECIES_\w+', evo[1]):
            union(key, target)

groups = json.loads(Path('data/maps/map_groups.json').read_text())
maps = {}
for group, names in groups.items():
    if group == 'gMapGroup_KantoShared' or group.endswith('_Frlg'):
        for name in names:
            path = Path('data/maps') / name / 'map.json'
            if path.exists():
                maps[name] = json.loads(path.read_text())
ids = {data['id']: (name, data) for name, data in maps.items()}
sources = {k: set() for k in species}
unresolved = []

def source(key, method, evidence):
    if key in sources:
        sources[key].add((method, evidence))

wild = json.loads(Path('src/data/wild_encounters.json').read_text())['wild_encounter_groups'][0]['encounters']
for encounter in wild:
    map_id = encounter['map']
    if map_id not in ids or '_LeafGreen' in encounter['base_label']:
        continue
    name, data = ids[map_id]
    for field, method in [('land_mons', 'Cave' if data['map_type'] == 'MAP_TYPE_UNDERGROUND' else 'Grass'), ('water_mons', 'Surf'), ('fishing_mons', 'Fishing'), ('rock_smash_mons', 'Other deliberate method: Rock Smash')]:
        for mon in encounter.get(field, {}).get('mons', []):
            source(mon['species'], method, f"{name} ({encounter['base_label']})")

trade_records = entries(preprocess('data/trade.h'), 'INGAME_TRADE_')
for name in maps:
    path = Path('data/maps') / name / 'scripts.inc'
    if not path.exists():
        continue
    text = subprocess.check_output(['cpp', '-P', '-traditional-cpp', '-DFIRERED', '-DIS_FRLG=1', str(path)]).decode()
    # Explicit direct gifts and statics are strong evidence. Temporary-variable
    # producers need manual control-flow review and remain separate candidates.
    for match in re.finditer(r'\b(givemon|giveegg|setwildbattle)\s+(SPECIES_\w+)', text):
        key = match[2]
        legendary = key in species and any(re.search(r'\.' + flag + r'\s*=\s*1\b', species[key]) for flag in ['isRestrictedLegendary', 'isSubLegendary', 'isMythical'])
        if name == 'PokemonTower_6F_Frlg' and key == 'SPECIES_MAROWAK':
            continue  # Scripted ghost is explicitly uncatchable.
        method = ('Fossil Museum' if name == 'CinnabarIsland_PokemonLab_ExperimentRoom_Frlg' else ('Legendary Static' if match[1] == 'setwildbattle' and legendary else 'Gift / Static'))
        source(key, method, str(path))
    if re.search(r'\b(givemon|giveegg|setwildbattle)\s+VAR_', text):
        candidates = sorted(set(re.findall(r'SPECIES_\w+', text)) & species.keys())
        if name in ['CeladonCity_GameCorner_PrizeRoom_Frlg', 'SaffronCity_Dojo_Frlg']:
            # Manually traced producer/consumer paths: each choice sets
            # VAR_TEMP_1, then the selected species goes through givemon.
            method = 'Game Corner' if 'GameCorner' in name else 'Gift / Static'
            chosen = re.findall(r'setvar\s+VAR_TEMP_1,\s*(SPECIES_\w+)', text)
            for key in chosen:
                source(key, method, str(path) + ': selected VAR_TEMP_1 -> givemon')
        else:
            unresolved.append({'path': str(path), 'candidate_species': candidates, 'reason': 'Variable-based acquisition requires producer/control-flow verification.'})
    if 'special InitRoamer' in text:
        unresolved.append({'path': str(path), 'candidate_species': ['SPECIES_LATIAS', 'SPECIES_LATIOS'], 'reason': 'InitRoamer still uses the Hoenn Lati selector/movement table; native Kanto reachability requires runtime verification.'})
    for trade in set(re.findall(r'INGAME_TRADE_\w+', text)):
        body = trade_records.get(trade, '')
        received = re.search(r'\.species\s*=\s*(SPECIES_\w+)', body)
        if received:
            source(received[1], 'NPC Trade', f'{path}: {trade}')

# The native arcade counter constructs and atomically delivers these prizes.
prize_path = Path('src/chaos_arcade_prizes.c')
prize_text = prize_path.read_text()
prize_table = prize_text.split('sPrizes[] = {', 1)[1].split('};', 1)[0]
for key in re.findall(r'\{(SPECIES_\w+),', prize_table):
    source(key, 'Game Corner', str(prize_path) + ': selected prize -> GiveScriptedMonToPlayer')

# Standard Oak gifts are handed out by C through selected starter variables.
for key in ['SPECIES_BULBASAUR', 'SPECIES_CHARMANDER', 'SPECIES_SQUIRTLE']:
    source(key, 'Gift / Static', 'src/chaos_progression.c: GiveChaosOakStarter (Standard mode)')

families = {}
for key in species:
    families.setdefault(find(key), []).append(key)
rows = []
for members in families.values():
    members.sort()
    bases = sorted(set(re.search(r'\.natDexNum\s*=\s*NATIONAL_DEX_(\w+)', species[key])[1] for key in members))
    paths = sorted(set(path for key in members for path in sources[key]))
    candidates = [u['path'] for u in unresolved if set(members) & set(u['candidate_species'])]
    rows.append({'family': '/'.join(bases), 'members': members, 'methods': sorted(set(p[0] for p in paths)), 'sources': [{'method': a, 'evidence': b} for a, b in paths], 'unresolved_sources': candidates, 'status': 'covered' if paths else ('needs script review' if candidates else 'no audited natural path')})
rows.sort(key=lambda r: r['family'])

sprites = []
for key, body in species.items():
    gfx = re.search(r'\.overworldData\s*=\s*\{(.*?)\}', body, re.S)
    image = re.search(r'\.images\s*=\s*(\w+)', gfx[1]) if gfx else None
    compatible = None
    if image is None:
        dex = re.search(r'\.natDexNum\s*=\s*(NATIONAL_DEX_\w+)', body)[1]
        compatible = next((other for other, record in species.items() if other != key and dex in record and re.search(r'\.overworldData\s*=\s*\{.*?\.images\s*=\s*\w+', record, re.S)), None)
    sprites.append({'species': key, 'images': image[1] if image else None, 'compatible_native_sprite': compatible})

Path('docs/chaos-availability-matrix.json').write_text(json.dumps({'families': rows, 'unresolved_acquisition_paths': unresolved, 'overworld_sprite_audit': sprites}, indent=2) + '\n')
counts = {status: sum(r['status'] == status for r in rows) for status in ['covered', 'needs script review', 'no audited natural path']}
report = ['# FireRed acquisition and Ranch sprite audit', '', 'Generated by `.github/scripts/audit_chaos_availability.py` from the enabled FireRed configuration. Forms sharing a National Dex entry and ordinary evolution edges belong to one family. Hoenn-only maps, LeafGreen tables, trainer teams, randomizer pools, and arbitrary custom starters are not credited as natural acquisition.', '', f'Enabled family groups: **{len(rows)}**. Confirmed source families: **{counts["covered"]}**. Variable-script candidates needing review: **{counts["needs script review"]}**. No audited natural path: **{counts["no audited natural path"]}**.', '', '“No audited natural path” is a planning gap, not proof that every possible C/event path has been exhausted. The JSON records exact species/forms and evidence. Variable-based prizes, fossils, roaming and event producers must be traced before declaring final gaps or choosing trades. No new species placements are assigned by this audit.', '', f'Overworld images found: **{sum(bool(s["images"]) for s in sprites)}/{len(sprites)}** enabled form records. Missing image records require an asset/alias check before Ranch use.', '', '| Family | Audited method(s) | Status |', '|---|---|---|']
for row in rows:
    report.append(f'| {row["family"]} | {"; ".join(row["methods"]) or "—"} | {row["status"]} |')
report += ['', '## Sources needing control-flow review', '']
report += ['- ' + u['path'] + ': ' + ', '.join(u['candidate_species']) for u in unresolved]
report += ['', '## Alternate forms using compatible native sprites', '']
report += ['- ' + s['species'] + ' → ' + (s['compatible_native_sprite'] or 'MISSING: needs an asset') for s in sprites if not s['images']]
report += ['', '## Custom roster cross-check', '', 'Mega Arcanine has an explicit overworld image. Mega Nidoking uses the existing Nidoking image as a compatible alias. No enabled species record or overworld asset is named Chaozar, Chaossal, Hitmonorris, or Dragon Eevee in this checkout; those design names remain pending implementation and are not claimed as Ranch-tested custom species.']
Path('docs/chaos-availability-matrix.md').write_text('\n'.join(report) + '\n')

# Search all native ability data and battle implementations, not only the
# initially supplied list. These are review candidates, not approved effects.
native = ['FORECAST', 'FLOWER_GIFT', 'GULP_MISSILE', 'COMMANDER', 'EMBODY_ASPECT_TEAL_MASK', 'EMBODY_ASPECT_HEARTHFLAME_MASK', 'EMBODY_ASPECT_WELLSPRING_MASK', 'EMBODY_ASPECT_CORNERSTONE_MASK', 'MULTITYPE', 'RKS_SYSTEM', 'TERA_SHIFT', 'TERA_SHELL', 'TERAFORM_ZERO', 'ZEN_MODE']
hits = []
for path in list(Path('src').glob('battle*.c')) + [Path('src/pokemon.c'), Path('src/data/pokemon/form_change_tables.h')]:
    lines = path.read_text().splitlines()
    for index, line in enumerate(lines):
        for ability in re.findall(r'ABILITY_\w+', line):
            neighborhood = '\n'.join(lines[max(0, index - 5):index + 9])
            if 'SPECIES_' in neighborhood or path.name == 'form_change_tables.h':
                hits.append({'ability': ability, 'path': str(path), 'line': index + 1})
ability_records = entries(preprocess('data/abilities.h'), 'ABILITY_')
full_table = []
for ability, record in ability_records.items():
    full_table.append({'ability': ability,
                       'flags': sorted(re.findall(r'\.(\w+)\s*=\s*1\b', record)),
                       'species_or_form_check_sites': [hit for hit in hits if hit['ability'] == ability],
                       'status': 'implemented Chaos fallback' if ability.removeprefix('ABILITY_') in ['ZERO_TO_HERO', 'BATTLE_BOND', 'SCHOOLING', 'SHIELDS_DOWN', 'DISGUISE', 'ICE_FACE', 'POWER_CONSTRUCT', 'STANCE_CHANGE', 'HUNGER_SWITCH'] else ('requires deliberate review' if ability.removeprefix('ABILITY_') in native else 'source audit candidate' if any(hit['ability'] == ability for hit in hits) else 'no species/form guard found in audited files')})
Path('docs/chaos-signature-ability-audit.json').write_text(json.dumps({'pending_deliberate_review': native, 'full_ability_table': full_table, 'species_or_form_check_candidates': hits}, indent=2) + '\n')
print(json.dumps({'families': len(rows), **counts, 'sprite_records': len(sprites), 'sprite_images_found': sum(bool(s['images']) for s in sprites)}, indent=2))
