#!/usr/bin/env python3
"""Exercise production arcade item transactions, including failed deliveries."""
from pathlib import Path
import re, subprocess, tempfile
root=Path(__file__).resolve().parents[2]
s=(root/'src/chaos_arcade_prizes.c').read_text()
def fn(name):
 start=s.index(name+'(');start=s.rfind('\n',0,start)+1
 brace=s.index('{',start);i=brace+1;depth=1
 while depth:depth+=(s[i]=='{')-(s[i]=='}');i+=1
 return s[start:i]
tables=s[s.index('struct ArcadeItemPrize'):s.index('void ChaosArcadeItemCategories')]
items=sorted(set(re.findall(r'\bITEM_\w+',tables)))
code=r'''
#include <assert.h>
#include <stdio.h>
#include <stddef.h>
typedef unsigned char u8,bool8;typedef unsigned short u16;typedef unsigned u32;
#define EWRAM_DATA
#define TRUE 1
#define FALSE 0
#define ARRAY_COUNT(a) (sizeof(a)/sizeof((a)[0]))
#define FLAG_BADGE04_GET 1
#define ITEM_COIN_CASE 1000
u16 gSpecialVar_Result,coins;int badge=1,coinCase=1,bagSpace=1,owned,charges,deliveries;
int FlagGet(int f){return badge;}
int CheckBagHasItem(int i,int n){return i==ITEM_COIN_CASE?coinCase:owned;}
unsigned GetCoins(void){return coins;}
int AddBagItem(int i,int n){if(!bagSpace)return 0;deliveries++;return 1;}
void RemoveCoins(unsigned price){assert(coins>=price);coins-=price;charges++;}
'''
code+='enum Item {'+','.join(items)+'};\n'+tables+fn('ChosenItem')+fn('ChaosArcadeItemBuy')+r'''
int main(void){
 for(unsigned category=0;category<3;category++){
  unsigned count=category==2?ARRAY_COUNT(sEquipment):ARRAY_COUNT(sTmPrizes);
  for(unsigned i=0;i<count;i++){
   const struct ArcadeItemPrize *p=category==2?&sEquipment[i]:&sTmPrizes[i];
   if(category<2&&p->support!=(category==0))continue;
   sItemCategory=category;sItemChoice=i;coins=9999;owned=0;
   int old=charges,delivered=deliveries;ChaosArcadeItemBuy();
   assert(gSpecialVar_Result==0&&coins==9999-p->price&&charges==old+1&&deliveries==delivered+1);
   ChaosArcadeItemBuy();assert(gSpecialVar_Result==4&&charges==old+1&&deliveries==delivered+1);
   for(int failure=0;failure<4;failure++){
    sItemChoice=i;coins=failure==0?p->price-1:9999;badge=failure!=1;coinCase=failure!=2;bagSpace=failure!=3;
    old=charges;delivered=deliveries;unsigned balance=coins;ChaosArcadeItemBuy();
    assert(gSpecialVar_Result==(failure==0?2:failure==3?3:4)&&charges==old&&deliveries==delivered&&coins==balance);
   }
   badge=coinCase=bagSpace=1;coins=9999;
   if(category<2){owned=1;sItemChoice=i;old=charges;ChaosArcadeItemBuy();assert(gSpecialVar_Result==5&&charges==old&&coins==9999);owned=0;}
  }
 }
 sItemCategory=255;sItemChoice=0;coins=9999;ChaosArcadeItemBuy();assert(gSpecialVar_Result==4&&coins==9999);
 puts("PASS all 30 TMs/9 equipment: exact charges, failed Bag/funds/badge/Case atomicity, owned TM and replay guards.");
}
'''
with tempfile.TemporaryDirectory() as d:
 p=Path(d)/'check.c';p.write_text('int IsIronmonRun(void){return 0;}\n' + code)
 subprocess.run(['cc','-std=gnu11','-Wall','-Werror',str(p),'-o',d+'/check'],check=True)
 subprocess.run([d+'/check'],check=True)
