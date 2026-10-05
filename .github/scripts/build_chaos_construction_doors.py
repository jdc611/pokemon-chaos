#!/usr/bin/env python3
"""Build striped exterior boards while retaining native door behavior/layers."""
from pathlib import Path
import struct
from PIL import Image
root=Path(__file__).resolve().parents[2]
p=root/'data/tilesets/secondary/celadon_city_frlg';q=root/'data/tilesets/primary/general_frlg'
im=Image.open(p/'tiles.png');out=Image.new('P',(128,144));out.putpalette(im.getpalette());out.paste(im.crop((0,0,128,128)),(0,0))
for y in range(16):
 for x in range(16):
  value=0
  if 3<=y<7 or 10<=y<14:value=9 if ((x+y)//3)%2 else 4
  if y in (2,7,9,14):value=4
  if y in (4,11) and x in (1,14):value=1
  out.putpixel((x,128+y),value)
out.save(p/'tiles.png')
a=(p/'metatiles.bin').read_bytes()[:240*16];attrs=(p/'metatile_attributes.bin').read_bytes()[:240*4]
old=0x15b;entry=struct.unpack_from('<8H',(q/'metatiles.bin').read_bytes(),old*16)
for i in range(2):
 a+=struct.pack('<8H',*entry[:4],*[0x8000+640+t for t in (256,257,272,273)])
 attrs+=(q/'metatile_attributes.bin').read_bytes()[old*4:(old+1)*4]
(p/'metatiles.bin').write_bytes(a);(p/'metatile_attributes.bin').write_bytes(attrs)
