"""Build a working Fire/Water neon Delta skin from project artwork.

Artwork is generated separately; UI geometry and inputs are deterministic.
Run with Pillow and NumPy. Real iOS import/touch testing remains necessary.
"""
import argparse
import json
from pathlib import Path
import zipfile
import numpy as np
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageOps
from build_skin import representation, validate, FONT

ART = Path(__file__).parent / 'assets/fire-water-dragons.png'

def neon(canvas, f, scale, label='', kind='normal'):
    x,y,w,h = [round(f[k]*scale) for k in ('x','y','width','height')]
    radius = 10*scale if kind=='bezel' else min(w,h)//2 if kind in ('a','b','circle') or label in ('L','R','TRACKER') else min(w,h)//4
    mask = Image.new('L',canvas.size); d = ImageDraw.Draw(mask)
    d.rounded_rectangle((x,y,x+w,y+h),radius,fill=255)
    a = np.zeros((canvas.height,canvas.width,4),dtype=np.uint8)
    yy,xx = np.mgrid[0:h+1,0:w+1]
    if kind in ('a','b'):
        brightness=np.clip(1-np.sqrt(((xx/w-.47)*1.4)**2+((yy/h-.35)*1.4)**2),0,1)
        dark=np.array([65,2,10] if kind=='a' else [3,8,65])
        bright=np.array([255,28,42] if kind=='a' else [24,123,255])
        rgb=dark+brightness[:,:,None]*(bright-dark)
    else:
        brightness=np.clip(1-yy/h,0,1)
        rgb=np.array([4,5,10])+brightness[:,:,None]*np.array([24,22,28])
        rgb=np.broadcast_to(rgb,(h+1,w+1,3))
    a[y:y+h+1,x:x+w+1,:3]=rgb.astype(np.uint8)
    a[:,:,3]=np.array(mask)
    canvas.alpha_composite(Image.fromarray(a))
    ring=Image.new('L',canvas.size);d=ImageDraw.Draw(ring)
    d.rounded_rectangle((x,y,x+w,y+h),radius,outline=255,width=max(1,3*scale))
    colors=np.array([[255,130,0],[255,27,18],[203,26,255],[0,236,255],[22,97,255]])
    cols=np.linspace(0,4,w+1);lo=np.minimum(cols.astype(int),3);mix=(cols-lo)[:,None]
    rgb=colors[lo]*(1-mix)+colors[lo+1]*mix
    rim=np.zeros_like(a)
    positions=np.clip((np.arange(canvas.width)-x)/w*4,0,4)
    lo=np.minimum(positions.astype(int),3);mix=(positions-lo)[:,None]
    fullrgb=colors[lo]*(1-mix)+colors[lo+1]*mix
    rim[:,:,:3]=fullrgb.astype(np.uint8)[None,:,:]
    rim[:,:,3]=np.array(ring)
    glow=Image.fromarray(rim)
    # Blur alpha rather than RGB: otherwise black outside the stroke dims
    # the colored halo a second time when alpha compositing.
    for blur,strength in ((10,3),(4,2)):
        halo=glow.copy()
        halo.putalpha(ring.filter(ImageFilter.GaussianBlur(blur*scale)).point(lambda a:min(255,a*strength)))
        canvas.alpha_composite(halo)
    canvas.alpha_composite(glow)
    d=ImageDraw.Draw(canvas)
    inset=4*scale
    d.rounded_rectangle((x+inset,y+inset,x+w-inset,y+h-inset),max(1,radius-inset),outline=(238,245,255,225),width=scale)
    if label:
        fs=round((33 if kind in ('a','b') else 24 if label in ('▲','▼','◀','▶') else 9 if kind=='circle' else 14)*scale)
        font=ImageFont.truetype(FONT,fs)
        d.text((x+w/2,y+h/2),label,font=font,anchor='mm',fill='#17121d' if kind in ('a','b') else '#f1f2f7',
               stroke_width=max(1,scale),stroke_fill='#f57783' if kind=='a' else '#51b9ff' if kind=='b' else '#090b12')

def render(art, rep, scale):
    w,h=rep['mappingSize'].values()
    canvas=ImageOps.fit(art,(w*scale,h*scale),centering=(.5,.65)).convert('RGBA')
    screen=rep['screens'][0]['outputFrame']
    # Black glass bezel with the same fire-to-water luminous border.
    border={**screen,'x':screen['x']-10,'y':screen['y']-10,
            'width':screen['width']+20,'height':screen['height']+20}
    neon(canvas,border,scale,kind='bezel')
    labels={'up':'▲','down':'▼','left':'◀','right':'▶','a':'A','b':'B',
            'l':'L','r':'R','menu':'MENU','select':'SELECT','start':'START'}
    for item in rep['items']:
        inputs=item['inputs'];key=inputs[0]
        portrait=w<h
        bottom=portrait and w<600 and key in ('menu','start','select')
        neon(canvas,item['frame'],scale,'TRACKER' if len(inputs)==2 else labels[key],
             key if key in ('a','b') else 'circle' if bottom else 'normal')
    # Strict transparent rectangle, including its edges. Delta renders the
    # running game beneath controller artwork; title art must not cover it.
    d=ImageDraw.Draw(canvas)
    x,y,sw,sh=(screen[k]*scale for k in ('x','y','width','height'))
    d.rectangle((int(x),int(y),round(x+sw),round(y+sh)),fill=(0,0,0,0))
    return canvas

def main():
    parser=argparse.ArgumentParser();parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args();out=args.output;out.mkdir(parents=True,exist_ok=True)
    info={'name':'Pokemon Chaos Fire Water Neon','identifier':'com.jdc611.chaos.gba.firewater.neon',
          'gameTypeIdentifier':'com.rileytestut.delta.game.gba','debug':False,'representations':{}}
    with Image.open(ART) as source:art=source.convert('RGB')
    for device,variants in {'iphone':{'standard':(414,736),'edgeToEdge':(414,896)},'ipad':{'standard':(768,1024)}}.items():
        info['representations'][device]={}
        for variant,(w,h) in variants.items():
            info['representations'][device][variant]={}
            for orientation in ('portrait','landscape'):
                landscape=orientation=='landscape';name=f'{device}_{variant}_{orientation}'
                rep=representation(out,name,h if landscape else w,w if landscape else h,landscape)
                sf=rep['screens'][0]['outputFrame']
                sf['x']+=8;sf['width']-=16;sf['height']=sf['width']/1.5
                rep['items'][0]['frame']['y']=sf['y']+sf['height']+8
                if not landscape and w<600:
                    for item in rep['items']:
                        key=item['inputs'][0]
                        if len(item['inputs'])==1 and key in ('menu','select','start'):
                            center=w*({'select':.28,'menu':.5,'start':.72}[key])
                            item['frame']={'x':center-21,'y':h-66,'width':42,'height':42}
                for scale,size in ((1,'small'),(2,'medium'),(3,'large')):
                    render(art,rep,scale).save(out/rep['assets'][size])
                info['representations'][device][variant][orientation]=rep
    validate(info,out)
    (out/'info.json').write_text(json.dumps(info,indent=2)+'\n')
    package=out/'Pokemon-Chaos-Fire-Water-Neon.deltaskin'
    with zipfile.ZipFile(package,'w',zipfile.ZIP_DEFLATED) as archive:
        for path in [out/'info.json',*sorted(out.glob('*.png'))]:archive.write(path,path.name)
    with zipfile.ZipFile(package) as archive:assert archive.testzip() is None
    print('PASS all six layouts, alpha, inputs, touch geometry and archive:',package)

if __name__=='__main__':main()
