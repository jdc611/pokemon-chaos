from qa import *
import struct,json,collections
l=struct.unpack('<37I',(ROOT/'v2-layout.bin').read_bytes())
assert lib.boot(str(repo/'pokefirered.gba').encode());state('pass-world',1)
size,params,psize,total,end,friend,tradeparty,region,trade,script,item=l[:11]
methods=collections.Counter();conditions=collections.Counter();issues=[];lines=[]
for species in range(1,total):
 if not call('IsSpeciesEnabled',species):continue
 e=call('GetSpeciesEvolutions',species)
 if not e:continue
 for i in range(30):
  a=e+i*size;m=rd(a,2)
  if m==end:break
  target=rd(a+4,2);param=rd(a+2,2);c=rd(a+params);cs=[]
  if c:
   for j in range(20):
    cond=rd(c+j*psize,2)
    if cond==39:break
    cs.append(cond);conditions[cond]+=1
  methods[m]+=1;lines.append([species,m,param,target,cs])
  if m==script or friend in cs or tradeparty in cs or region in cs:issues.append(lines[-1])
for line in lines:
 if line[1]==trade and not any(x[0]==line[0] and x[3]==line[3] and x[1]!=trade for x in lines):issues.append(line)
print('LAYOUT',l[:11]);print('EVOLUTIONS',len(lines),'METHODS',dict(methods),'CONDITIONS',dict(conditions),'INACCESSIBLE',issues,flush=True)
(ROOT/'v2-evolution-audit.json').write_text(json.dumps({'evolutions':lines,'issues':issues},indent=2))
assert not issues
# Level independence, IVs and ability-sensitive current-form ratings.
p=symbols['gParties'];scratch=0x0203e000
ratings=[]
for sp in [10,129,123,93,94,143,6,150,384]:
 call('ZeroPlayerPartyMons');call('ScriptGiveMon',sp,20,0);a=call('ChaosCurrentStageRating',p)
 wr(scratch,50);call('SetMonData',p,l[15],scratch);b=call('ChaosCurrentStageRating',p)
 assert a==b,(sp,a,b)
 ratings.append([sp,a]);print('RATING',sp,a,flush=True)
(ROOT/'v2-rating-calibration.json').write_text(json.dumps(ratings))
