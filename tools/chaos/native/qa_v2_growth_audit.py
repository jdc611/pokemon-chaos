from qa import *
import struct,collections
l=struct.unpack('<37I',(ROOT/'v2-layout.bin').read_bytes());assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1);s=0x0203e000
bad=[];branches=[];counts=collections.Counter()
for sp in range(1,l[3]):
 if not call('IsSpeciesEnabled',sp):continue
 e=call('GetSpeciesEvolutions',sp);n=0
 if not e:continue
 for i in range(100):
  a=e+i*l[0];m=rd(a,2)
  if m==l[4]:break
  if not m:continue
  n+=1;call('ChaosFormatEvolution',a,s)
  raw=[]
  for j in range(400):
   c=rd(s+j,1)
   if c==255:break
   raw.append(c)
  call('WrapFontIdToFit',s,s+len(raw),0,144);lines=1
  for j in range(510):
   c=rd(s+j,1)
   if c==255:break
   if c==254:lines+=1
  counts[lines]+=1
  if lines>3:bad.append([sp,i,lines])
 if n>32:branches.append([sp,n])
print('Growth requirement lines',dict(counts),'overflow',bad,'branch counts above32',branches,flush=True)
