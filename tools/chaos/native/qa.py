import ctypes as C, subprocess, os, shutil
from pathlib import Path
from PIL import Image
repo=Path(__file__).resolve().parents[3]
ROOT=Path(os.environ.get('CHAOS_QA_DIR','/tmp/chaos-native-qa'));ROOT.mkdir(parents=True,exist_ok=True)
symbols={}
for line in subprocess.check_output([os.environ.get('CHAOS_ARM_NM',shutil.which('arm-none-eabi-nm') or 'arm-none-eabi-nm'),'-n',str(repo/'pokefirered.elf')],text=True).splitlines():
 p=line.split()
 if len(p)==3:
  try:symbols[p[2]]=int(p[0],16)
  except:pass
# Resolve duplicate private controller function names by object order: Oak, Player, Safari.
_matches=[]
for line in subprocess.check_output([os.environ.get('CHAOS_ARM_NM',shutil.which('arm-none-eabi-nm') or 'arm-none-eabi-nm'),'-n',str(repo/'pokefirered.elf')],text=True).splitlines():
 parts=line.split()
 if len(parts)==3 and parts[2]=='HandleInputChooseAction':_matches.append(int(parts[0],16))
symbols['HandleInputChooseAction']=_matches[1]
lib=C.CDLL(str(ROOT/'emulator.so'));lib.boot.argtypes=[C.c_char_p];lib.rd.restype=C.c_uint;lib.call.restype=C.c_uint
lib.state.argtypes=[C.c_char_p,C.c_int];lib.screenshot.argtypes=[C.c_char_p]
def rd(a,n=4):return lib.rd(symbols.get(a,a),n)
def wr(a,v,n=4):return lib.wr(symbols.get(a,a),v,n)
def call(fn,*args):
 assert not isinstance(fn,str) or fn in symbols, fn
 result=lib.call(symbols.get(fn,fn),*(list(args)+[0]*4)[:4])
 assert result!=0xdeadbeef,fn
 return result
def frames(n,k=0):lib.frames(n,k)
def snap(name):
 raw=ROOT/(name+'.raw');lib.screenshot(str(raw).encode());Image.frombytes('RGBA',(240,160),raw.read_bytes()).convert('RGB').resize((960,640)).save(ROOT/(name+'.png'))
def state(name,load=0):lib.state(str(ROOT/(name+'.state')).encode(),load)
if __name__=='__main__':
 assert lib.boot(str(repo/'pokefirered.gba').encode())
 frames(180);wr('gDebugForceKantoNewGame',1,1);call('SetMainCallback2',symbols['CB2_NewGame']|1);frames(240);snap('qa-house');state('house')
 print('save3',hex(rd('gSaveBlock3Ptr')),'save1',hex(rd('gSaveBlock1Ptr')),'CB2',hex(rd(symbols['gMain']+4)))
