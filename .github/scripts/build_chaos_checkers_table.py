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
# Two complete cabinets with their own screens/trim. Append after the table so
# its existing metatile IDs, pixels and interaction area remain unchanged.
raw=(p/'chaos_checkers_tiles.4bpp').read_bytes()
metas=(p/'metatiles.bin').read_bytes();attrs=(p/'metatile_attributes.bin').read_bytes()
for cabinet in range(4):
 def cabinet_pixel(x,y):
  if x<2 or x>29:return 0
  if y<2:return 0
  if x in (2,29) or y in (2,44):return (8,15,6,9)[cabinet]
  if y<7:return 1
  if 5<=x<=26 and 9<=y<=27:
   if x in (5,26) or y in (9,27):return 4
   if cabinet in (0,2):
    return 15 if (x//5+y//5)%2 else 9
   return 9 if (x-15)**2+(y-18)**2<36 else 1
  if y<30:return 12
  if y in (32,33) and 7<=x<25:return 4
  if 36<=y<=39 and x in (9,10,22,23):return 15
  return 13 if y<44 else 1
 base=len(raw)//32+640
 raw+=encode(cabinet_pixel,32,48)
 for row in range(3):
  for col in range(2):
   tile=base+row*8+col*2
   metas+=struct.pack('<8H',*floor,*[0xA000+tile+i for i in (0,1,4,5)])
 attrs+=bytes(6*4)
(p/'chaos_checkers_tiles.4bpp').write_bytes(raw)
(p/'metatiles.bin').write_bytes(metas)
(p/'metatile_attributes.bin').write_bytes(attrs)

# Slot banks/chairs share native palette 8 with the approved table. Route only
# their original metatiles to the neon clone; the six table metatiles stay at 8.
metas=bytearray((p/'metatiles.bin').read_bytes())
for i in range(152*8):
 value=struct.unpack_from('<H',metas,i*2)[0]
 if value>>12==8:struct.pack_into('<H',metas,i*2,(value&0xFFF)|0xA000)
(p/'metatiles.bin').write_bytes(metas)
