"""Cross-ROM save compatibility; never use emulator states across builds."""
import ctypes as C, hashlib, os, subprocess
from pathlib import Path
repo=Path(__file__).resolve().parents[3]
root=Path(os.environ.get('CHAOS_QA_DIR','/tmp/chaos-native-qa'))
lib=C.CDLL(str(root/'emulator.so'))
lib.boot.argtypes=[C.c_char_p];lib.save_restore.argtypes=[C.c_char_p]
lib.rd.restype=lib.call.restype=C.c_uint
baseline=Path(os.environ['CHAOS_BASELINE_ROM_DIR']);save=Path(os.environ['CHAOS_BASELINE_SAVE'])
fingerprints=[]
for folder in (baseline,repo):
 syms={p[2]:int(p[0],16) for line in subprocess.check_output([os.environ['CHAOS_ARM_NM'],'-n',str(folder/'pokefirered.elf')],text=True).splitlines() if len(p:=line.split())==3}
 assert lib.boot(str(folder/'pokefirered.gba').encode());lib.frames(3000,0)
 assert lib.save_restore(str(save).encode())
 assert lib.call(syms['LoadGameSave'],0,0,0,0)==1
 def blob(ptr,size):return bytes(lib.rd(ptr+i,1) for i in range(size))
 blocks=[blob(lib.rd(syms[name],4),size) for name,size in (
  ('gSaveBlock1Ptr',15756),('gSaveBlock2Ptr',3884),('gSaveBlock3Ptr',1564),('gPokemonStoragePtr',34144))]
 blocks.append(blob(syms['gParties'],600))
 fingerprints.append([hashlib.sha256(block).hexdigest() for block in blocks])
 if folder==repo:assert lib.call(syms['IsIronmonRun'],0,0,0,0)==0
assert fingerprints[0]==fingerprints[1],fingerprints
print('PASS actual old .sav across baseline/current ROM: Save1, Save2, original Save3 prefix, all stored Pokémon and player party match byte-for-byte; IronMON stays off.')
