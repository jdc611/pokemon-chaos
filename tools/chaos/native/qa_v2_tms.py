from qa import *
import struct
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
v=struct.unpack('<'+str((ROOT/'v2-tms.bin').stat().st_size//4)+'I',(ROOT/'v2-tms.bin').read_bytes());size,desc=v[:2];s=0x0203e000
bad=[]
for item,move in zip(v[2::2],v[3::2]):
 a=rd(symbols['gMovesInfo']+size*move+desc);raw=[]
 for i in range(450):
  c=rd(a+i,1)
  if c==255:break
  raw.append(c)
 assert raw,(item,move)
 for i,c in enumerate(raw+[255]):wr(s+i,c,1)
 call('WrapFontIdToFit',s,s+len(raw),0,216)
 wrapped=[]
 for i in range(510):
  c=rd(s+i,1)
  if c==255:break
  wrapped.append(c)
 lines=1+wrapped.count(254)
 # The priority footer is counted separately by the actual display.
 priority=(rd(symbols['gMovesInfo']+size*move+16)>>0)&15
 if priority:lines+=1
 if lines>5:bad.append([item,move,lines])
print('TM count',len(v[2:])//2,'oversized descriptions',bad,flush=True)
assert not bad
