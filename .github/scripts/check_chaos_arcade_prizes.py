#!/usr/bin/env python3
"""Exercise actual arcade prize preparation and atomic delivery routines."""
from pathlib import Path
import re, subprocess, tempfile
root=Path(__file__).resolve().parents[2]
s=(root/'src/chaos_arcade_prizes.c').read_text()
def fn(name):
 m=re.search(r'^(?:static )?(?:void|bool32) '+name+r'\([^;]*?\)\n\{',s,re.M); assert m,name
 i=m.end();depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[m.start():i]
tables=s[s.index('struct ArcadePrize'):s.index('void ChaosArcadePrizeCancel')]
species=sorted(set(re.findall(r'\bSPECIES_\w+',tables)))
moves=sorted(set(re.findall(r'\bMOVE_\w+',tables)))
code=r'''
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char u8,bool8;typedef unsigned short u16;typedef unsigned u32;typedef int bool32;
#define TRUE 1
#define FALSE 0
#define EWRAM_DATA
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define COMPOUND_STRING(s) ((const u8 *)(s))
#define CHAOS_ABILITY_NATIVE_HIDDEN 511
#define MON_DATA_ABILITY_NUM 1
#define MON_DATA_CHAOS_STARTER_ABILITY 2
#define MON_DATA_IS_SHINY 3
#define STR_CONV_MODE_LEFT_ALIGN 0
#define OTID_STRUCT_PLAYER_ID 0
#define PARTY_SIZE 6
#define MON_CANT_GIVE 2
#define FLAG_BADGE04_GET 1
#define ITEM_COIN_CASE 1
enum Ability {ABILITY_NONE,ABILITY_NORMAL,ABILITY_HIDDEN};
struct Pokemon {u16 species,ability,marker,move;u8 level,slot,shiny;};
struct {u16 abilities[3];const u8 *speciesName;} gSpeciesInfo[100];
struct {const u8 *name;} gAbilitiesInfo[3]={{(const u8 *)"NONE"},{(const u8 *)"NORMAL"},{(const u8 *)"HIDDEN"}};
u16 gSpecialVar_Result,coins;u8 gStringVar1[32],gStringVar2[32],gStringVar3[32];
int badge=1,coinCase=1,legal=1,delivery,deliveries,charges;struct Pokemon received;
void Free(void *p){free(p);}
bool32 FlagGet(int f){return badge;}
bool32 CheckBagHasItem(int i,int n){return coinCase;}
u16 GetCoins(void){return coins;}
void RemoveCoins(u16 n){assert(coins>=n);coins-=n;charges++;}
u32 GiveScriptedMonToPlayer(struct Pokemon *p,u8 slot){assert(slot==6);deliveries++;if(delivery!=2)received=*p;return delivery;}
void CreateMon(struct Pokemon *p,int species,u8 level,u32 personality,int ot){memset(p,0,sizeof(*p));p->species=species;p->level=level;p->ability=ABILITY_NORMAL;}
void SetMonData(struct Pokemon *p,int field,const void *v){if(field==1){p->slot=*(const u8 *)v;p->marker=0;}else if(field==2)p->marker=*(const u16 *)v;else p->shiny=*(const u8 *)v;}
void SetMonMoveSlot(struct Pokemon *p,int m,int slot){p->move=m;}
bool32 TrySetMonAbilityToActiveRunFilter(struct Pokemon *p){return legal;}
bool32 DoesMonMatchActiveRunFilter(struct Pokemon *p){return legal;}
int GetMonAbility(struct Pokemon *p){return p->marker==511?ABILITY_HIDDEN:p->ability;}
u8 *StringCopy(u8 *d,const u8 *t){strcpy((char *)d,(const char *)t);return d+strlen((char *)d);}
u8 *ConvertIntToDecimalStringN(u8 *d,int v,int mode,int digits){return d+sprintf((char *)d,"%d",v);}
'''
code+='enum Species {'+','.join(species)+'};\nenum Move {'+','.join(moves)+'};\n'
code+=tables+fn('ChaosArcadePrizeCancel')+fn('HasHidden')+fn('ChaosArcadePrizePrepare')+fn('ChaosArcadePrizeBuy')+r'''
static void draft(int prize,int tier){
 sPrizeDraft=calloc(1,sizeof(*sPrizeDraft));sPrizeDraft->prize=prize;sPrizeDraft->species=sPrizes[prize].species;
 gSpecialVar_Result=tier;ChaosArcadePrizePrepare();
}
int main(void){
 for(unsigned i=0;i<100;i++){gSpeciesInfo[i].speciesName=(const u8 *)"TEST";gSpeciesInfo[i].abilities[0]=1;gSpeciesInfo[i].abilities[2]=2;}
 for(unsigned prize=0;prize<ARRAY_COUNT(sPrizes);prize++) for(unsigned tier=0;tier<4;tier++){
  coins=9999;delivery=0;draft(prize,tier);assert(gSpecialVar_Result==0&&sPrizeDraft->prepared);
  unsigned price=sPrizes[prize].price+sSurcharges[tier];assert(sPrizeDraft->price==price);
  assert(sPrizeDraft->mon.shiny==!!(tier&2));assert(sPrizeDraft->mon.marker==((tier&1)?511:0));
  ChaosArcadePrizeBuy();assert(gSpecialVar_Result==0&&coins==9999-price&&sPrizeDraft==NULL);
  int old=charges;ChaosArcadePrizeBuy();assert(gSpecialVar_Result==4&&charges==old);
 }
 delivery=1;coins=5000;draft(0,0);ChaosArcadePrizeBuy();assert(gSpecialVar_Result==1&&coins==3500);
 delivery=2;coins=5000;draft(0,0);int old=charges;ChaosArcadePrizeBuy();assert(gSpecialVar_Result==3&&coins==5000&&charges==old);
 coins=1499;delivery=0;draft(0,0);old=deliveries;ChaosArcadePrizeBuy();assert(gSpecialVar_Result==2&&coins==1499&&deliveries==old);
 for(int gate=0;gate<2;gate++){coins=5000;draft(0,0);badge=gate;coinCase=!gate;old=deliveries;ChaosArcadePrizeBuy();assert(gSpecialVar_Result==4&&coins==5000&&deliveries==old);}badge=coinCase=1;
 coins=5000;draft(0,3);ChaosArcadePrizeCancel();assert(coins==5000&&sPrizeDraft==NULL);
 gSpeciesInfo[sPrizes[1].species].abilities[2]=1;draft(1,1);assert(gSpecialVar_Result==2&&!sPrizeDraft->prepared);ChaosArcadePrizeCancel();
 legal=0;draft(0,1);assert(gSpecialVar_Result==1&&sPrizeDraft->mon.marker==511);ChaosArcadePrizeCancel();legal=1;
 for(unsigned form=1;form<ARRAY_COUNT(sRotomForms);form++){
  sPrizeDraft=calloc(1,sizeof(*sPrizeDraft));sPrizeDraft->prize=1;sPrizeDraft->species=sRotomForms[form];sPrizeDraft->form=form;
  gSpecialVar_Result=2;ChaosArcadePrizePrepare();assert(sPrizeDraft->mon.move==sRotomMoves[form]&&sPrizeDraft->price==5000);ChaosArcadePrizeCancel();
 }
 puts("PASS: all 40 prize/tier totals; native HA marker; shiny flags; five Rotom moves; party/PC delivery; full storage, funds, badge/Case, cancel and replay guards; filter warning.");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'check.c';p.write_text(code)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/check'],check=True)
 subprocess.run([d+'/check'],check=True)
