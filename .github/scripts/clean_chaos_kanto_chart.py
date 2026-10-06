"""Build a clean chart from native route cells, preserving marker coordinates."""
from pathlib import Path
import re
from PIL import Image
root=Path(__file__).resolve().parents[2]
p=root/'graphics/pokenav/region_map'
atlas=Image.open(p/'map_kanto_source.png')
source=(p/'map_kanto_source.bin').read_bytes()
chart=Image.new('P',(512,512));chart.putpalette(atlas.getpalette())
for i,t in enumerate(source):
 chart.paste(atlas.crop((t%16*8,t//16*8,t%16*8+8,t//16*8+8)),(i%64*8,i//64*8))
original=chart.copy()
rows=[]
for line in (root/'src/data/region_map/region_map_layout_kanto.h').read_text().splitlines():
 ids=re.findall(r'MAPSEC_[A-Z0-9_]+',line)
 if len(ids)==28:rows.append(ids)
sea={'MAPSEC_ROUTE_19','MAPSEC_ROUTE_20','MAPSEC_ROUTE_21'}
cities={'MAPSEC_PALLET_TOWN','MAPSEC_VIRIDIAN_CITY','MAPSEC_PEWTER_CITY','MAPSEC_CERULEAN_CITY','MAPSEC_LAVENDER_TOWN','MAPSEC_VERMILION_CITY','MAPSEC_CELADON_CITY','MAPSEC_FUCHSIA_CITY','MAPSEC_SAFFRON_CITY','MAPSEC_INDIGO_PLATEAU'}
mask={};markers=set()
for y,row in enumerate(rows):
 for x,id in enumerate(row):
  if id in cities or id.startswith('MAPSEC_ROUTE_') and id not in sea:
   for dy in range(8):
    for dx in range(8):mask[((x+1)*8+dx,(y+2)*8+dy)]=id
  if id in cities or id.endswith('POKECENTER'):
   for dy in range(8):
    for dx in range(8):markers.add(((x+1)*8+dx,(y+2)*8+dy))
# Roads sit above terrain, with one-pixel continuous light/shadow edges.
# Native water routes and coastlines outside this exact logical road mask stay
# untouched. City/Center icon pixels retain their original positions/colors.
terrain={121,122,125,127,130,132,133,134,138}
for (x,y),id in mask.items():
 c=124
 if id in {'MAPSEC_ROUTE_17','MAPSEC_ROUTE_18'}:c=117 if y%2==0 else 123
 if (x,y-1) not in mask or (x-1,y) not in mask:c=115
 if (x,y+1) not in mask or (x+1,y) not in mask:c=129
 old=original.getpixel((x,y))
 if old not in terrain:c=old
 chart.putpixel((x,y),c)
# Repack as an affine (8-bit tile ID) map; every destination remains in place.
tiles=[];ids={};tilemap=bytearray()
for y in range(0,512,8):
 for x in range(0,512,8):
  t=chart.crop((x,y,x+8,y+8));key=t.tobytes()
  if key not in ids:ids[key]=len(tiles);tiles.append(t)
  tilemap.append(ids[key])
assert len(tiles)<=256
out=Image.new('P',(128,((len(tiles)+15)//16)*8));out.putpalette(atlas.getpalette())
for i,t in enumerate(tiles):out.paste(t,(i%16*8,i//16*8))
out.save(p/'map_kanto.png');(p/'map_kanto.bin').write_bytes(tilemap)
print('Clean chart:',len(tiles),'tiles; all land routes have continuous edges.')
