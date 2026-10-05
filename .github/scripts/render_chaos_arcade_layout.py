"""Render precise review diagrams; validate footprints against native event coordinates."""
from pathlib import Path
import json
from html import escape
from collections import deque
from PIL import Image, ImageDraw, ImageFont

root = Path(__file__).resolve().parents[2]
out = root / 'docs/game-corner-layout'
plan = json.loads((out / 'plan.json').read_text())

def cells(rect):
    x,y,w,h=rect
    return {(a,b) for a in range(x,x+w) for b in range(y,y+h)}

arc=plan['arcade']; story=cells(arc['protected_story']); used=set()
for z in arc['zones']:
    occupied=cells(z['rect'])
    assert not occupied & (used | story), z['name']
    assert all(0<=x<18 and 0<=y<15 for x,y in occupied)
    used |= occupied
# All activities have at least one reachable adjacent interaction tile.
story_obstacles={(11,1),(11,2),(15,2),(16,2),(17,2),(16,3),(17,3)}
walkable={(x,y) for x in range(18) for y in range(1,14)}-used-story_obstacles
q=deque(map(tuple,arc['entrances'])); reached=set(q)
while q:
    x,y=q.popleft()
    for p in [(x+1,y),(x-1,y),(x,y+1),(x,y-1)]:
        if p in walkable and p not in reached:reached.add(p);q.append(p)
for z in arc['zones']:
    assert any((x+dx,y+dy) in reached for x,y in cells(z['rect']) for dx,dy in [(1,0),(-1,0),(0,1),(0,-1)]),z['name']
assert {(11,3),(15,4)}<=reached  # approach to grunt and story stairs

ran=plan['ranch']
mapdata=json.loads((root/'data/maps/ChaosPokemonRanch/map.json').read_text())
roaming=set()
for m in mapdata['object_events']:
    roaming |= cells([m['x']-1,m['y']-1,3,3])
used=cells(ran['building'])|cells(ran['path'])|{tuple(ran['sign'])}
for d in ran['decorations']:
    occupied=cells(d['rect'])
    assert not occupied & (used|roaming),d['name']
    assert all(0<x<47 and 0<y<39 for x,y in occupied)
    used|=occupied

class Canvas:
    def __init__(self,w,h):
        self.im=Image.new('RGB',(w,h),'#f4f6fa');self.draw=ImageDraw.Draw(self.im)
        self.svg=[f'<svg xmlns="http://www.w3.org/2000/svg" width="{w}" height="{h}" viewBox="0 0 {w} {h}">',f'<rect width="{w}" height="{h}" fill="#f4f6fa"/>']
    def rect(self,x,y,w,h,fill,stroke=None):
        self.draw.rectangle((x,y,x+w,y+h),fill=fill,outline=stroke)
        self.svg.append(f'<rect x="{x}" y="{y}" width="{w}" height="{h}" fill="{fill}"'+(f' stroke="{stroke}"' if stroke else '')+'/>')
    def text(self,x,y,text,size=16,fill='#26394c',center=False):
        font=ImageFont.truetype('/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf',size)
        width=self.draw.textlength(text,font=font);xx=x-width/2 if center else x
        self.draw.text((xx,y),text,font=font,fill=fill)
        self.svg.append(f'<text x="{x}" y="{y+size}" font-family="DejaVu Sans,sans-serif" font-size="{size}" fill="{fill}"'+(' text-anchor="middle"' if center else '')+f'>{escape(text)}</text>')
    def save(self,name):
        self.im.save(out/(name+'.png'));(out/(name+'.svg')).write_text('\n'.join(self.svg+['</svg>']))

def grid(c,ox,oy,w,h,s,fill):
    for y in range(h):
        for x in range(w):c.rect(ox+x*s,oy+y*s,s,s,fill,'#cbd4dc')

def block(c,ox,oy,s,rect,fill,label=None,size=12):
    x,y,w,h=rect;c.rect(ox+x*s+2,oy+y*s+2,w*s-4,h*s-4,fill,'#ffffff')
    if label:
        if w==2 and ' · ' in label:
            number,title=label.split(' · ',1)
            c.text(ox+(x+w/2)*s,oy+(y+h/2)*s-15,number,11,'#ffffff',True)
            c.text(ox+(x+w/2)*s,oy+(y+h/2)*s+1,title,9,'#ffffff',True)
        else:c.text(ox+(x+w/2)*s,oy+(y+h/2)*s-size/2,label,size,'#ffffff',True)

c=Canvas(1200,700)
c.text(40,24,'GAME CORNER — proposed floor plan',28)
c.text(40,68,'Review diagram, not final tile art. Native map dimensions and story coordinates retained.',16)
ox,oy,s=40,145,26
grid(c,ox,oy,18,15,s,'#edf0e8')
block(c,ox,oy,s,[8,4,4,10],'#e2d8b7')
for y in [4,7,10]:block(c,ox,oy,s,[1,y,16,1],'#e2d8b7')
block(c,ox,oy,s,arc['protected_story'],'#b95356')
c.text(ox+14*s,oy+18,'ROCKET / POSTER',13,'white',True)
c.text(ox+14*s,oy+40,'Story tiles + exit route',11,'white',True)
for x,y,label in [(11,1,'P'),(11,2,'R'),(15,2,'S')]:
    c.rect(ox+x*s+4,oy+y*s+4,s-8,s-8,'#fff4e0');c.text(ox+(x+.5)*s,oy+y*s+4,label,12,'#9b3c3e',True)
for z in arc['zones']:block(c,ox,oy,s,z['rect'],z['color'],z['short'],10 if z['rect'][2]==2 else 12)
for x,y in arc['entrances']:block(c,ox,oy,s,[x,y,1,1],'#458b75')
c.text(ox+10.5*s,oy+14*s+7,'ENTRANCE',13,center=True)
c.rect(ox+8*s+5,oy+11*s+5,16,16,'#7356ab');c.text(ox+8.5*s,oy+11*s+4,'H',11,'white',True)
c.text(40,570,'H: welcome host; approaches only on first entry.',16)
c.text(40,601,'Construction worker outside until Erika. All activities open together.',15)
c.text(40,631,'The prize-room entrance uses the same badge gate; no early bypass.',15)
px,py=620,145
grid(c,px,py,9,10,s,'#edf0e8')
block(c,px,py,s,[1,1,7,2],'#4c79a8')
for x,label in [(2,'C'),(4,'P'),(6,'T')]:
    c.rect(px+x*s+4,py+2*s+4,s-8,s-8,'#fff4e0');c.text(px+(x+.5)*s,py+2*s+4,label,12,center=True)
for x,y in [(3,9),(4,8),(5,9)]:block(c,px,py,s,[x,y,1,1],'#458b75')
c.text(620,435,'NEXT-DOOR PRIZE ROOM',16)
c.text(620,469,'C · Cosmetics: Buy / Owned',14)
c.text(620,496,'P · Pokémon: forms + upgrades',14)
c.text(620,523,'T · Reusable TMs / battle items',14)
for i,z in enumerate(arc['zones']):c.text(925,145+i*28,z['short'].split(' · ')[0]+' — '+z['name'] if z['id'].isdigit() else z['name'],13)
c.text(925,565,'Gold: wager activities',14)
c.text(925,592,'Green: free skill games',14)
c.text(925,619,'Purple: challenge / board game',14)
c.save('arcade-proposal')

c=Canvas(1200,820)
c.text(40,20,'POKÉMON RANCH — proposed fixed decorations',27)
c.text(40,60,'All 30 Pokémon positions, central path, entrance and box sign remain clear.',16)
ox,oy,s=40,125,16
grid(c,ox,oy,48,40,s,'#d6ead4')
block(c,ox,oy,s,ran['path'],'#d4cbb6')
block(c,ox,oy,s,ran['building'],'#b95356','CENTER',14)
for m in mapdata['object_events']:
    x,y=m['x'],m['y'];block(c,ox,oy,s,[x-1,y-1,3,3],'#b9d4b7')
    c.rect(ox+x*s+4,oy+y*s+4,8,8,'#387253')
for i,d in enumerate(ran['decorations']):
    block(c,ox,oy,s,d['rect'],d['color'])
    if i<6:
        x,y,w,h=d['rect'];c.text(ox+(x+w/2)*s,oy+y*s+4,str(i+1),14,'white',True)
    elif d['id']=='fountain':
        x,y,w,h=d['rect'];c.text(ox+(x+w/2)*s,oy+(y+h/2)*s-6,'F',14,'white',True)
x,y=ran['sign'];c.rect(ox+x*s+3,oy+y*s+3,10,10,'#997143')
c.text(850,135,'Statue garden (one unlock each)',17)
for i,d in enumerate(ran['decorations'][:6]):c.text(850,174+i*30,f'{i+1} · '+d['name'],16)
for y,t in [(380,'Two benches · one unlock'),(412,'Two flower beds · one unlock'),(444,'F · small fountain'),(500,'Dark dots: actual Pokémon slots'),(532,'Pale squares: ±1 wandering area'),(584,'Decorations use static map tiles,'),(610,'so they add no roaming objects.'),(660,'Same decoration locations in'),(686,'Forest / Beach / Snow / Night.'),(728,'Bought props apply immediately;'),(754,'toggle them free in Owned.')]:c.text(850,y,t,16)
c.save('ranch-proposal')
print('PASS: non-overlapping zones, reachable activities, Rocket approach, all 30 roaming areas clear.')
