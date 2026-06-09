"""Alien Whack-a-Mole Mini-Game"""
from ursina import *
import random

class WhackAMole:
    def __init__(self, player_name='Explorer', on_complete=None):
        self.player_name=player_name; self.on_complete=on_complete
        self.score=0; self.timer=45; self.ents=[]; self.holes=[]; self.aliens={}
        mouse.visible=True
        bg=Entity(parent=camera.ui,model='quad',scale=(2,2),color=color.Color(0.01,0,0.06,0.97),z=1); self.ents.append(bg)
        self.title=Text('ALIEN WHACK-A-MOLE',parent=camera.ui,scale=2.5,position=(0,0.44),origin=(0,0),color=color.Color(1,0.3,0.8,1)); self.ents.append(self.title)
        self.score_t=Text('Score: 0',parent=camera.ui,scale=1.8,position=(-0.6,0.37),origin=(-0.5,0),color=color.Color(1,0.86,0.2,1)); self.ents.append(self.score_t)
        self.timer_t=Text('Time: 45',parent=camera.ui,scale=1.8,position=(0.2,0.37),origin=(0,0),color=color.Color(0.3,1,0.5,1)); self.ents.append(self.timer_t)
        legend=Text('Green=10  Blue=25  Gold=100  Purple=-15',parent=camera.ui,scale=1,position=(0,0.30),origin=(0,0),color=color.Color(0.7,0.7,0.9,0.8)); self.ents.append(legend)
        self.info=Text('Click aliens before they hide!',parent=camera.ui,scale=1.1,position=(0,-0.44),origin=(0,0),color=color.Color(0.6,0.6,0.8,0.7)); self.ents.append(self.info)
        # Create hole grid (3x3)
        positions=[(x*0.38, y*0.22) for y in [1,0,-1] for x in [-1,0,1]]
        for i,(hx,hy) in enumerate(positions):
            hole=Entity(parent=camera.ui,model='circle',scale=0.13,position=(hx,hy,0),color=color.Color(0.05,0.02,0.12,1)); self.ents.append(hole)
            self.holes.append((hx,hy))
            self.aliens[i]=None
        self.show_t=0; self.show_dur=1.8; self.elapsed=0
        self._task=Entity(update=self._update); self.ents.append(self._task)

    def _update(self):
        dt=time.dt; self.timer-=dt; self.elapsed+=dt
        self.timer_t.text=f'Time: {max(0,int(self.timer))}'
        if self.timer<=0: self._end(); return
        spd_factor=1+self.elapsed/45
        self.show_t+=dt
        interval=max(0.5,1.3/spd_factor)
        if self.show_t>interval:
            self.show_t=0; self._pop_alien()
        for i,al in list(self.aliens.items()):
            if al and hasattr(al,'life'):
                al.life-=dt
                sc=al.scale[0]
                if al.life>0.3: al.scale=min(0.12,sc+dt*0.3)
                else: al.scale=max(0,sc-dt*0.4)
                if al.life<=0:
                    destroy(al); self.aliens[i]=None
                elif mouse.left and self._hit(al):
                    self.score+=al.pts; self.score_t.text=f'Score: {self.score}'
                    destroy(al); self.aliens[i]=None

    def _pop_alien(self):
        empty=[i for i,a in self.aliens.items() if not a]
        if not empty: return
        i=random.choice(empty); hx,hy=self.holes[i]
        kind=random.choices(['green','blue','gold','purple'],weights=[50,30,10,10])[0]
        cols={'green':color.Color(0.1,0.85,0.3,1),'blue':color.Color(0.2,0.5,1,1),
              'gold':color.Color(1,0.8,0.1,1),'purple':color.Color(0.7,0.2,0.9,1)}
        pts={'green':10,'blue':25,'gold':100,'purple':-15}
        al=Entity(parent=camera.ui,model='sphere',scale=0.001,position=(hx,hy,0),color=cols[kind])
        al.pts=pts[kind]; al.life=1.5; al.kind=kind
        self.aliens[i]=al; self.ents.append(al)

    def _hit(self,al):
        mp=mouse.position; ap=Vec2(al.x,al.y)
        return (mp-ap).length()<al.scale[0]*0.8

    def _end(self):
        for e in self.ents:
            try: destroy(e)
            except: pass
        if self.on_complete: self.on_complete('whack_a_mole',self.score)

    def input(self,key):
        if key=='escape': self._end()
