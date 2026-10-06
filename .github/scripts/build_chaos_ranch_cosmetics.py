from pathlib import Path
import struct,shutil
from PIL import Image,ImageDraw
r=Path(__file__).resolve().parents[2]; p=r/'data/tilesets/secondary/chaos_ranch';src=r/'data/tilesets/secondary/viridian_city_frlg';p.mkdir(exist_ok=True);(p/'palettes').mkdir(exist_ok=True)
for f in (src/'palettes').glob('*.pal'):shutil.copyfile(f,p/'palettes'/f.name)
colors=[(0,0,0),(38,45,57),(71,81,95),(104,119,133),(148,164,177),(197,213,222),(238,246,255),(66,119,79),(98,164,98),(156,197,115),(189,74,106),(246,148,172),(49,131,189),(106,205,230),(230,205,131),(156,115,66)]
(p/'palettes/07.pal').write_text('JASC-PAL\n0100\n16\n'+'\n'.join(' '.join(map(str,c)) for c in colors)+'\n')
im=Image.open(src/'tiles.png'); tiles=[]
for y in range(0,im.height,8):
 for x in range(0,im.width,8):tiles.append(im.crop((x,y,x+8,y+8)))
base=len(tiles);metas=bytearray((src/'metatiles.bin').read_bytes());attrs=bytearray((src/'metatile_attributes.bin').read_bytes());meta_base=len(metas)//16
primary=(r/'data/tilesets/primary/general_frlg/metatile_attributes.bin').read_bytes(); spec=[]
def add_art(name,a,impass=True):
 first=len(metas)//16
 for y in range(0,a.height,16):
  for x in range(0,a.width,16):
   ids=[]
   for dy,dx in [(0,0),(0,8),(8,0),(8,8)]:
    ids.append(0x7000+640+len(tiles));tiles.append(a.crop((x+dx,y+dy,x+dx+8,y+dy+8)))
   metas.extend(struct.pack('<8H',*ids,0,0,0,0));attrs.extend(primary[8*4:9*4])
 spec.append((name,first,a.width//16,a.height//16,impass))
for name in ['rhydon','lapras','snorlax','venusaur','charizard','blastoise']:
 doll=Image.open(r/f'graphics/object_events/pics/dolls/big_{name}_doll.png').convert('RGB');a=Image.new('P',(32,32));a.putpalette(sum((list(c) for c in colors),[])+[0]*(768-48));d=ImageDraw.Draw(a);d.rectangle((1,25,30,31),fill=2);d.line((2,25,29,25),fill=6);d.rectangle((3,29,28,31),fill=1)
 orig=Image.open(r/f'graphics/object_events/pics/dolls/big_{name}_doll.png');bg=orig.getpixel((0,0))
 for y in range(28):
  for x in range(32):
   if orig.getpixel((x,y))!=bg:
    rr,gg,bb=doll.getpixel((x,y));light=(rr*3+gg*6+bb)//10;a.putpixel((x,y),1+min(5,light//44))
 add_art(name,a)
def canvas(w,h):
 a=Image.new('P',(w,h),0);a.putpalette(sum((list(c) for c in colors),[])+[0]*(768-48));return a,ImageDraw.Draw(a)
a,d=canvas(32,16);d.rectangle((2,3,29,11),fill=15);d.line((3,4,28,4),fill=14);d.line((3,8,28,8),fill=14);d.rectangle((5,12,7,15),fill=1);d.rectangle((24,12,26,15),fill=1);add_art('bench',a)
a,d=canvas(32,32);d.rectangle((1,1,30,30),fill=7)
for x,y in [(7,7),(23,7),(15,16),(7,25),(23,25)]:
 d.line((x,y,x,y+4),fill=9);d.ellipse((x-3,y-3,x+3,y+3),fill=10);d.point((x,y),fill=14)
add_art('flowers',a)
a,d=canvas(64,64);d.ellipse((3,12,60,58),fill=1);d.ellipse((4,10,59,54),fill=3);d.ellipse((8,13,55,48),fill=6);d.ellipse((12,16,51,45),fill=12);d.ellipse((16,18,47,41),fill=13);d.rectangle((28,9,35,32),fill=3);d.ellipse((22,4,41,16),fill=5);d.line((31,1,31,8),fill=13,width=3);d.arc((16,0,47,31),190,345,fill=13,width=2);d.line((19,19,23,22),fill=6);d.line((40,29,45,29),fill=6);add_art('fountain',a)
# All environments have native walkable grass behavior. No encounter/surf/sliding flags.
for name in ['forest','beach','snow','night']:
 a,d=canvas(16,16)
 if name=='forest':
  a.paste(7,(0,0,16,16));d.line((2,4,4,1),fill=8);d.line((10,11,12,7),fill=9);d.point((7,14),fill=8)
 elif name=='beach':
  a.paste(14,(0,0,16,16));d.line((2,5,5,5),fill=15);d.line((10,11,12,11),fill=15);d.point((7,2),fill=6)
 elif name=='snow':
  a.paste(6,(0,0,16,16));d.line((1,12,4,12),fill=5);d.line((11,4,14,4),fill=5);d.point((6,7),fill=13)
 else:
  a.paste(1,(0,0,16,16));d.line((2,4,4,1),fill=2);d.line((10,11,12,7),fill=3);d.point((6,6),fill=13)
 add_art(name,a,False)
# Place prop art on an actual terrain underlay, so transparent corners match
# the chosen environment instead of showing blue/green backdrop rectangles.
prop_entries=[struct.unpack_from('<8H',metas,i*16)[:4] for i in range(meta_base,meta_base+46)]
grass=struct.unpack_from('<8H',(r/'data/tilesets/primary/general_frlg/metatiles.bin').read_bytes(),8*16)[:4]
for j,art in enumerate(prop_entries):struct.pack_into('<8H',metas,(meta_base+j)*16,*grass,*art)
for theme in range(4):
 ground=struct.unpack_from('<8H',metas,(meta_base+46+theme)*16)[:4]
 for art in prop_entries:
  metas.extend(struct.pack('<8H',*ground,*art));attrs.extend(primary[8*4:9*4])
# Theme the grass under the sign and around all Center wall/roof edges.
# Reuse unused native secondary metatile slots; preserve the five roof tiles
# actually referenced by the Ranch. This stays inside the GBA 384-meta limit.
primary_metas=(r/'data/tilesets/primary/general_frlg/metatiles.bin').read_bytes()
composites=[1,3,72,73,74,75,80,88,91,96,97,390,391,399,407,415]
free_slots=iter(i for i in range(meta_base) if i not in range(48,53))
theme_composites=[]
for original in composites:
 original_words=list(struct.unpack_from('<8H',primary_metas,original*16))
 variants=[]
 for theme in range(4):
  ground=struct.unpack_from('<8H',metas,(meta_base+46+theme)*16)[:4]
  words=list(original_words)
  for q in range(4):
   if original==1 or words[q] in (0x13,0x27f):words[q]=ground[q]
  slot=next(free_slots);variants.append(640+slot)
  struct.pack_into('<8H',metas,slot*16,*words)
  attrs[slot*4:slot*4+4]=primary[original*4:original*4+4]
 theme_composites.append((original,variants))
out=Image.new('P',(128,((len(tiles)+15)//16)*8));out.putpalette(im.getpalette())
for i,t in enumerate(tiles):out.paste(t,(i%16*8,i//16*8))
out.save(p/'tiles.png');(p/'metatiles.bin').write_bytes(metas);(p/'metatile_attributes.bin').write_bytes(attrs)
print('ranch tiles',len(tiles),'metas',len(metas)//16,'spec',spec)
# Quiet beveled rails preserve native corner shapes and legible UI borders.
f=Image.open(r/'graphics/pokenav/region_map/frame.png')
for theme in ['midnight','rocket']:
 o=f.copy()
 pal=[(0,0,0),(18,27,52),(106,164,222),(42,65,104)] if theme=='midnight' else [(0,0,0),(30,27,38),(230,72,88),(88,43,56)]
 for yy in range(o.height):
  for xx in range(o.width):o.putpixel((xx,yy),{0:0,3:1,4:2,15:3}[f.getpixel((xx,yy))])
 o.putpalette(sum((list(c) for c in pal),[])+[0]*(768-len(pal)*3));o.save(r/f'graphics/pokenav/region_map/frame_{theme}.png')
# Generated C metatile constants are stable when script is rerun.
h=r/'include/constants/chaos_ranch_cosmetics.h';h.write_text('#ifndef GUARD_CHAOS_RANCH_COSMETICS_H\n#define GUARD_CHAOS_RANCH_COSMETICS_H\n'+''.join(f'#define RANCH_TILE_{name.upper()} {640+idx}\n' for name,idx,*_ in spec)+'#define RANCH_THEME_PROP_OFFSET 50\n#define RANCH_THEME_PROP_STRIDE 46\n#define RANCH_THEME_COMPOSITES {'+','.join('{'+str(original)+',{'+','.join(map(str,variants))+'}}' for original,variants in theme_composites)+'}\n#endif\n')

# Distinct map skins keep route geometry and destination coordinates intact.
map_image=Image.open(r/'graphics/pokenav/region_map/map_kanto.png')
base_colors=[tuple(map(int,line.split())) for line in (r/'graphics/pokenav/region_map/map_kanto.pal').read_text().splitlines()[3:51]]
for theme in ['midnight','rocket']:
 colors=list(base_colors)
 for i,(rr,gg,bb) in enumerate(colors):
  if i==0:continue
  if theme=='midnight':
   if bb>rr and bb>gg:colors[i]=(12+rr//8,22+gg//6,52+bb//5)
   elif gg>rr and gg>bb:colors[i]=(22+rr//8,45+gg//4,61+bb//5)
  else:
   if bb>rr and bb>gg:colors[i]=(24+rr//10,22+gg//10,37+bb//8)
   elif gg>rr and gg>bb:colors[i]=(45+rr//7,35+gg//8,49+bb//7)
   elif rr>gg and rr>bb:colors[i]=(min(255,rr),55+gg//6,64+bb//6)
 # Keep the original coastline, roads and water textures. A restrained
 # chart palette is readable without the old full-screen grid/star noise.
 out=map_image.copy()
 out.save(r/f'graphics/pokenav/region_map/map_kanto_{theme}.png')
 (r/f'graphics/pokenav/region_map/map_kanto_{theme}.pal').write_text('JASC-PAL\n0100\n48\n'+'\n'.join(' '.join(map(str,c)) for c in colors)+'\n')
