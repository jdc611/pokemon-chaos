#!/usr/bin/env python3
"""Build exact GBA tiles for the arcade table, preserving native indexed art."""
from pathlib import Path
import struct
from PIL import Image
root=Path(__file__).resolve().parents[2]
p=root/'data/tilesets/secondary/game_corner_frlg'
im=Image.open(p/'tiles.png')
def encode(pixels,width,height):
 out=bytearray()
 for y in range(0,height,8):
  for x in range(0,width,8):
   for yy in range(8):
    for xx in range(0,8,2):out.append(pixels(x+xx,y+yy)|(pixels(x+xx+1,y+yy)<<4))
 return out
raw=encode(lambda x,y:im.getpixel((x,y)),im.width,im.height)
def pixel(x,y):
 if y<2 or y>30 or x<1 or x>46:return 0
 if y>=28:return 12 if (x<5 or x>42) else 1
 if y in (2,27) or x in (1,46):return 12
 if 12<=x<36 and 3<=y<27:
  color=4 if ((x-12)//3+(y-3)//3)%2==0 else 1
  if (x-12)%3==1 and (y-3)%3==1 and ((x-12)//3+(y-3)//3)%2 and ((y-3)//3<2 or (y-3)//3>5):color=13 if y<9 else 8
  return color
 return 14 if y<5 else 13
raw+=encode(pixel,48,32)
(p/'chaos_checkers_tiles.4bpp').write_bytes(raw)
# New upper tiles preserve the native floor beneath their transparent pixels.
metas=(p/'metatiles.bin').read_bytes()[:152*16]
floor=struct.unpack_from('<4H',metas,(0x291-0x280)*16)
for row in range(2):
 for col in range(3):
  top=640+176+row*12+col*2
  metas+=struct.pack('<8H',*floor,*[0x8000+top+i for i in (0,1,6,7)])
(p/'metatiles.bin').write_bytes(metas)
a=(p/'metatile_attributes.bin').read_bytes()[:152*4]
a+=bytes(6*4)
(p/'metatile_attributes.bin').write_bytes(a)
