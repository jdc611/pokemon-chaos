"""Build original Delta skin artwork and mapping, not an unavailable user skin.

Requires Pillow. Actual Delta import/touch QA requires an iOS device.
Schema: https://noah978.gitbook.io/delta-docs/skins
"""
from pathlib import Path
import argparse
import json
import zipfile
from PIL import Image, ImageDraw, ImageFont

FONT = '/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf'

def frame(x, y, width, height):
    return dict(x=x, y=y, width=width, height=height)

def representation(output, name, w, h, landscape):
    if landscape:
        gh = min(h - 124, (w - 360) * 2/3, 400)
        gw = gh * 1.5
        screen = frame((w-gw)/2, 22, gw, gh)
        tracker = frame(w/2-56, 22+gh+8, 112, 36)
        dpad = frame(22, h/2-60, 120, 120)
        buttons = [('A', 'a', frame(w-90, h/2-62, 66, 66)),
                   ('B', 'b', frame(w-158, h/2+5, 66, 66)),
                   ('L', 'l', frame(22, 24, 100, 38)),
                   ('R', 'r', frame(w-122, 24, 100, 38))]
        bottom = h-48
    else:
        gw = w-24
        gh = gw*2/3
        screen = frame(12, 52, gw, gh)
        tracker = frame(w/2-56, 52+gh+10, 112, 40)
        controls = 52+gh+82
        dpad = frame(24, controls+55, 138, 138)
        buttons = [('A', 'a', frame(w-92, controls+38, 72, 72)),
                   ('B', 'b', frame(w-170, controls+105, 72, 72)),
                   ('L', 'l', frame(18, controls-10, 112, 38)),
                   ('R', 'r', frame(w-130, controls-10, 112, 38))]
        bottom = h-88
    buttons += [('MENU', 'menu', frame(18, bottom, 82, 34)),
                ('SELECT', 'select', frame(w/2-90, bottom, 80, 34)),
                ('START', 'start', frame(w/2+10, bottom, 80, 34))]
    items = [dict(inputs=['l', 'select'], frame=tracker),
             dict(inputs={k:k for k in ('up','down','left','right')}, frame=dpad)]
    items += [dict(inputs=[key], frame=f) for _,key,f in buttons]
    assets = {}
    for scale, size in ((1,'small'), (2,'medium'), (3,'large')):
        image = Image.new('RGBA', (w*scale,h*scale), '#e9edf2')
        draw = ImageDraw.Draw(image)
        def rect(f, fill, outline=None, radius=10):
            x,y,fw,fh = (f[k]*scale for k in ('x','y','width','height'))
            draw.rounded_rectangle((x,y,x+fw,y+fh), radius*scale,
                                   fill=fill, outline=outline, width=2*scale)
        def label(f, text, color, size=13):
            font = ImageFont.truetype(FONT, size*scale)
            draw.text(((f['x']+f['width']/2)*scale,(f['y']+f['height']/2)*scale),
                      text, font=font, fill=color, anchor='mm')
        draw.rectangle((0,0,w*scale,18*scale), fill='#b83c42')
        rect(frame(screen['x']-4,screen['y']-4,screen['width']+8,screen['height']+8), '#42576d')
        # DeltaCore renders GameView below the controller artwork. The game
        # viewport must be transparent, or the artwork hides the entire ROM.
        rect(screen, (0, 0, 0, 0), radius=0)
        rect(tracker, '#b83c42', '#7c2630')
        label(tracker, 'TRACKER', 'white')
        # A single D-pad input is divided by Delta into a 3x3 grid.
        third=dpad['width']/3
        rect(frame(dpad['x']+third,dpad['y'],third,dpad['height']), '#42576d')
        rect(frame(dpad['x'],dpad['y']+third,dpad['width'],third), '#42576d')
        for text,key,f in buttons:
            rect(f, '#b83c42' if key in ('a','b') else '#42576d')
            label(f,text,'white',23 if key in ('a','b') else 12)
        filename=f'{name}_{size}.png'
        image.save(output/filename)
        assets[size]=filename
    return dict(assets=assets, items=items, screens=[dict(outputFrame=screen)],
                mappingSize=dict(width=w,height=h), translucent=False)

def validate(info, output):
    allowed={'a','b','l','r','start','select','menu','up','down','left','right'}
    for device in info['representations'].values():
        for variants in device.values():
            for rep in variants.values():
                w,h=rep['mappingSize'].values()
                sf=rep['screens'][0]['outputFrame']
                assert abs(sf['width']/sf['height']-1.5)<0.001
                tracker=rep['items'][0]
                assert tracker['inputs']==['l','select']
                tf=tracker['frame']
                assert abs(tf['x']+tf['width']/2-w/2)<0.001
                assert 0<tf['y']-sf['y']-sf['height']<=10
                for item in rep['items']:
                    f=item['frame']
                    assert f['x']>=0 and f['y']>=0
                    assert f['x']+f['width']<=w and f['y']+f['height']<=h
                    inputs=item['inputs']
                    assert set(inputs.values() if isinstance(inputs,dict) else inputs)<=allowed
                    assert (f['x']+f['width']<=sf['x'] or f['x']>=sf['x']+sf['width']
                            or f['y']+f['height']<=sf['y'] or f['y']>=sf['y']+sf['height'])
                for i, item in enumerate(rep['items']):
                    f = item['frame']
                    for other in rep['items'][i+1:]:
                        g = other['frame']
                        assert (f['x']+f['width']<=g['x'] or g['x']+g['width']<=f['x']
                                or f['y']+f['height']<=g['y'] or g['y']+g['height']<=f['y'])
                for filename in rep['assets'].values():
                    with Image.open(output/filename) as im:
                        assert abs(im.width/im.height-w/h)<0.001
                        assert im.mode == 'RGBA'
                        sx = im.width/w
                        sy = im.height/h
                        center = ((sf['x']+sf['width']/2)*sx,
                                  (sf['y']+sf['height']/2)*sy)
                        assert im.getpixel(tuple(int(v) for v in center))[3] == 0
                        interior = (int(sf['x']*sx)+1, int(sf['y']*sy)+1,
                                    int((sf['x']+sf['width'])*sx)-1,
                                    int((sf['y']+sf['height'])*sy)-1)
                        assert im.getchannel('A').crop(interior).getbbox() is None

def main():
    parser=argparse.ArgumentParser()
    parser.add_argument('--output',type=Path,default=Path('/tmp/chaos-delta-skin'))
    args=parser.parse_args();out=args.output;out.mkdir(parents=True,exist_ok=True)
    info=dict(name='Pokemon Chaos Tracker',identifier='com.jdc611.chaos.gba.tracker',
              gameTypeIdentifier='com.rileytestut.delta.game.gba',debug=False,
              representations={})
    for device,variants in {'iphone':{'standard':(414,736),'edgeToEdge':(414,896)},
                            'ipad':{'standard':(768,1024)}}.items():
        info['representations'][device]={}
        for variant,(w,h) in variants.items():
            info['representations'][device][variant]={}
            for orientation in ('portrait','landscape'):
                landscape=orientation=='landscape'
                name=f'{device}_{variant}_{orientation}'
                info['representations'][device][variant][orientation]=representation(
                    out,name,h if landscape else w,w if landscape else h,landscape)
    validate(info,out)
    (out/'info.json').write_text(json.dumps(info,indent=2)+'\n')
    package=out/'Pokemon-Chaos-Tracker.deltaskin'
    with zipfile.ZipFile(package,'w',zipfile.ZIP_DEFLATED) as z:
        for path in [out/'info.json',*sorted(out.glob('*.png'))]:z.write(path,path.name)
    with zipfile.ZipFile(package) as z:assert z.testzip() is None
    print(f'PASS schema/geometry/assets/archive checks: {package}')
    print('Actual Delta import/touch-input verification remains pending.')

if __name__=='__main__':main()
