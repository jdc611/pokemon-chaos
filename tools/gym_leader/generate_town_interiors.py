#!/usr/bin/env python3
"""Reuse native FRLG layouts for constructed-building interiors."""
import json
from pathlib import Path
root=Path(__file__).resolve().parents[2]
groups_path=root/'data/maps/map_groups.json';groups=json.loads(groups_path.read_text())
configs=[('GleBuildingInterior','SaffronCity_House_Frlg','OBJ_EVENT_GFX_GENTLEMAN_FRLG',4,4,'GleBuilding_Clerk'),('GleCenterInterior','PewterCity_PokemonCenter_1F_Frlg','OBJ_EVENT_GFX_NURSE_FRLG',7,2,'GleCenter_Nurse'),('GleMartInterior','PewterCity_Mart_Frlg','OBJ_EVENT_GFX_CLERK',2,3,'GleMart_Clerk'),('GleRivalGym','GleGymInterior','OBJ_EVENT_GFX_BLUE',6,4,'GleRival_Leader')]
# The actual rival graphics constant is shared by existing FRLG scripts.
for name,source,gfx,x,y,script in configs:
 m=json.loads((root/f'data/maps/{source}/map.json').read_text())
 m['id']='MAP_'+''.join('_'+c if c.isupper() else c for c in name).lstrip('_').upper()
 m['name']=name;m['show_map_name']=False;m['allow_running']=True
 m['object_events']=[{'type':'object','graphics_id':gfx,'x':x,'y':y,'elevation':3,'movement_type':'MOVEMENT_TYPE_FACE_DOWN','movement_range_x':0,'movement_range_y':0,'trainer_type':'TRAINER_TYPE_NONE','trainer_sight_or_berry_tree_id':'0','script':script,'flag':'0'}]
 m['warp_events']=[dict(w,dest_map='MAP_DYNAMIC',dest_warp_id='0') for w in m['warp_events'][:3]]
 m['coord_events']=[];m['bg_events']=[]
 d=root/f'data/maps/{name}';d.mkdir(exist_ok=True)
 (d/'map.json').write_text(json.dumps(m,indent=2)+'\n');(d/'scripts.inc').write_text(name+'_MapScripts::\n\t.byte 0\n')
 if name not in groups['gMapGroup_KantoShared']:groups['gMapGroup_KantoShared'].append(name)
groups_path.write_text(json.dumps(groups,indent=2)+'\n')
p=root/'data/event_scripts.s';s=p.read_text()
for name,*_ in configs:
 include=f'\t.include "data/maps/{name}/scripts.inc"'
 if include not in s:s+='\n'+include+'\n'
include='\t.include "data/scripts/gym_leader_town.inc"'
if include not in s:s+='\n'+include+'\n'
p.write_text(s)
