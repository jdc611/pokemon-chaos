#include <mgba/flags.h>
#include <mgba/core/core.h>
#include <mgba/core/config.h>
#include <mgba/core/log.h>
#include <mgba/internal/arm/arm.h>
#include <mgba/internal/arm/isa-inlines.h>
#include <mgba-util/vfs.h>
#include <stdlib.h>
#include <string.h>
static struct mCore *c;
static void quiet(struct mLogger *l,int cat,enum mLogLevel level,const char *format,va_list args){}
static struct mLogger logger={.log=quiet};
static color_t pixels[240*160];
static unsigned traceTarget,traceCount;static unsigned char traceText[64][256];
void trace_set(unsigned target){traceTarget=target&~1;traceCount=0;}
unsigned trace_count(void){return traceCount;}
unsigned trace_byte(unsigned i,unsigned j){return i<64 && j<256?traceText[i][j]:255;}

int boot(const char *path) { mLogSetDefaultLogger(&logger);c=mCoreFind(path); if(!c || !c->init(c))return 0; mCoreInitConfig(c,NULL); c->setVideoBuffer(c,pixels,240); if(!mCoreLoadFile(c,path))return 0; c->reset(c); return 1; }
void frames(int count,int keys){c->setKeys(c,keys);while(count--)c->runFrame(c);c->setKeys(c,0);}
unsigned rd(unsigned a,int n){return n==1?c->busRead8(c,a):n==2?c->busRead16(c,a):c->busRead32(c,a);}
void wr(unsigned a,unsigned v,int n){if(n==1)c->busWrite8(c,a,v);else if(n==2)c->busWrite16(c,a,v);else c->busWrite32(c,a,v);}
void screenshot(const char *path){FILE *f=fopen(path,"wb");fwrite(pixels,sizeof(pixels),1,f);fclose(f);}
void state(const char *path,int load){size_t size=c->stateSize(c);void *b=malloc(size);FILE *f=fopen(path,load?"rb":"wb");if(load){fread(b,size,1,f);c->loadState(c,b);}else{c->saveState(c,b);fwrite(b,size,1,f);}fclose(f);free(b);}
unsigned call(unsigned fn,unsigned a,unsigned b,unsigned d,unsigned e){
 struct ARMCore *cpu=c->cpu;
 for(int safe=0; safe<1000000 && (cpu->privilegeMode != MODE_SYSTEM || cpu->executionMode != MODE_THUMB || (unsigned)cpu->gprs[15]<0x08000000); safe++) c->step(c);
 struct ARMRegisterFile regs=cpu->regs;
 int oldPrivilege=cpu->privilegeMode;
 int32_t oldBanks[6][7],oldSpsrs[6];memcpy(oldBanks,cpu->bankedRegisters,sizeof(oldBanks));memcpy(oldSpsrs,cpu->bankedSPSRs,sizeof(oldSpsrs));
 int oldMode=cpu->executionMode,oldHalted=cpu->halted;unsigned pre[2]={cpu->prefetch[0],cpu->prefetch[1]};
 unsigned ime=c->busRead16(c,0x04000208);c->busWrite16(c,0x04000208,0);
 cpu->halted=0;_ARMSetMode(cpu,MODE_THUMB);cpu->gprs[0]=a;cpu->gprs[1]=b;cpu->gprs[2]=d;cpu->gprs[3]=e;
 cpu->gprs[14]=0x03007f01;cpu->gprs[15]=fn&~1;ThumbWritePC(cpu);
 int i;for(i=0;i<20000000;i++){if((unsigned)cpu->gprs[15]==0x03007f02)break;
 if(traceTarget && (unsigned)cpu->gprs[15]==traceTarget+2 && traceCount<64){unsigned ptr=c->busRead32(c,cpu->gprs[13]+16);for(unsigned j=0;j<255;j++){unsigned ch=c->busRead8(c,ptr+j);traceText[traceCount][j]=ch;if(ch==255)break;}traceText[traceCount][255]=255;traceCount++;}c->step(c);}
 unsigned result=cpu->gprs[0];if(i==20000000)result=0xdeadbeef;
 cpu->regs=regs;cpu->privilegeMode=oldPrivilege;memcpy(cpu->bankedRegisters,oldBanks,sizeof(oldBanks));memcpy(cpu->bankedSPSRs,oldSpsrs,sizeof(oldSpsrs));cpu->executionMode=oldMode;cpu->halted=oldHalted;
 cpu->memory.setActiveRegion(cpu,(regs.gprs[15]-(oldMode==MODE_THUMB?2:4)));
 cpu->prefetch[0]=pre[0];cpu->prefetch[1]=pre[1];c->busWrite16(c,0x04000208,ime);return result;
}
unsigned reg(int r){struct ARMCore *cpu=c->cpu;return r==16?cpu->cpsr.packed:cpu->gprs[r];}
int save_dump(const char *path){void *data=NULL;size_t n=c->savedataClone(c,&data);if(!n||!data)return 0;FILE *f=fopen(path,"wb");if(!f){free(data);return 0;}int ok=fwrite(data,1,n,f)==n;fclose(f);free(data);return ok;}
int save_restore(const char *path){FILE *f=fopen(path,"rb");if(!f)return 0;fseek(f,0,SEEK_END);size_t n=ftell(f);rewind(f);void *data=malloc(n);int ok=fread(data,1,n,f)==n;fclose(f);if(ok)ok=c->savedataRestore(c,data,n,false);free(data);return ok;}
