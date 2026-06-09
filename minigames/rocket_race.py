"""Rocket Race Mini-Game"""
from ursina import *
import random

class RocketRace:
    def __init__(self, player_name='Explorer', on_complete=None):
        self.player_name=player_name; self.on_complete=on_complete
        self.score=0; self.timer=0; self.ents=[]; self.rings_hit=0
        self.total_rings=20; self.speed=0; self.rx=0; self.ry=0
        self.ring_idx=0; self.done=False
        mouse.visible=True
        bg=Entity(parent=camera.ui,model='quad',scale=(2,2),color=color.Color(0.0,0,0.04,0.97),z=1); self.ents.append(bg)
        self.title=Text('ROCKET RACE',parent=camera.ui,scale=3,position=(0,0.44),origin=(0,0),color=color.Color(1,0.5,0.1,1)); self.ents.append(self.title)
        self.timer_t=Text('Time: 0.0',parent=camera.ui,scale=1.8,position=(0,0.36),origin=(0,0),color=color.Color(0.3,1,0.5,1)); self.ents.append(self.timer_t)
        self.ring_t=Text(f'Rings: 0/{self.total_rings}',parent=camera.ui,scale=1.5,position=(-0.55,0.36),origin=(-0.5,0),color=color.Color(1,0.86,0.2,1)); self.ents.append(self.ring_t)
        self.spd_t=Text('Speed: 0',parent=camera.ui,scale=1.3,position=(0.55,0.36),origin=(0.5,0),color=color.Color(0.5,0.8,1,1)); self.ents.append(self.spd_t)
        info=Text('W=Thrust  S=Brake  A/D=Steer  Pass all rings!',parent=camera.ui,scale=1.1,position=(0,-0.43),origin=(0,0),color=color.Color(0.6,0.6,0.8,0.7)); self.ents.append(info)
        # Rocket (player icon)
        self.rocket=Entity(parent=camera.ui,model='quad',scale=(0.04,0.08),
                           position=(0,0,0),color=color.Color(0.9,0.9,1,1)); self.ents.append(self.rocket)
        # Generate ring positions
        self.ring_positions=[]
        for i in range(self.total_rings):
            x=random.uniform(-0.55,0.55); y=random.uniform(-0.35,0.35)
            self.ring_positions.append((x,y))
        self._draw_rings()
        self._task=Entity(update=self._update); self.ents.append(self._task)
        self.active=True

    def _draw_rings(self):
        self.ring_ents=[]
        for i,(rx,ry) in enumerate(self.ring_positions):
            col=color.Color(1,0.8,0.1,1) if i==self.ring_idx else color.Color(0.3,0.3,0.5,0.6)
            e=Entity(parent=camera.ui,model='circle',scale=0.11,position=(rx,ry,0),color=col)
            inner=Entity(parent=camera.ui,model='circle',scale=0.07,position=(rx,ry,0.01),color=color.Color(0,0,0,0.7))
            num=Text(str(i+1),parent=camera.ui,scale=1,position=(rx,ry,0.02),origin=(0,0),color=color.white)
            self.ents+=[e,inner,num]; self.ring_ents.append(e)

    def _update(self):
        if not self.active: return
        dt=time.dt; self.timer+=dt
        self.timer_t.text=f'Time: {self.timer:.1f}s'
        # Movement
        if held_keys['w']: self.speed=min(self.speed+dt*0.8,1.0)
        elif held_keys['s']: self.speed=max(self.speed-dt*0.5,0)
        else: self.speed=max(self.speed-dt*0.2,0)
        if held_keys['a']: self.rx-=dt*0.6*self.speed
        if held_keys['d']: self.rx+=dt*0.6*self.speed
        if held_keys['up arrow']: self.ry+=dt*0.6*self.speed
        if held_keys['down arrow']: self.ry-=dt*0.6*self.speed
        self.rx=clamp(self.rx,-0.75,0.75); self.ry=clamp(self.ry,-0.45,0.45)
        self.rocket.x=self.rx; self.rocket.y=self.ry
        self.spd_t.text=f'Speed: {int(self.speed*100)}'
        # Check ring collision
        if self.ring_idx<self.total_rings:
            tx,ty=self.ring_positions[self.ring_idx]
            dist=((self.rx-tx)**2+(self.ry-ty)**2)**0.5
            if dist<0.055:
                bonus=50 if dist<0.025 else 20
                self.score+=bonus; self.rings_hit+=1
                self.ring_t.text=f'Rings: {self.rings_hit}/{self.total_rings}'
                # Update ring colors
                if self.ring_idx<len(self.ring_ents):
                    self.ring_ents[self.ring_idx].color=color.Color(0.1,0.7,0.2,1)
                self.ring_idx+=1
                if self.ring_idx<len(self.ring_ents):
                    self.ring_ents[self.ring_idx].color=color.Color(1,0.8,0.1,1)
                if self.ring_idx>=self.total_rings:
                    self._end()

    def _end(self):
        self.active=False
        time_bonus=max(0,int(1000-self.timer*10))
        self.score+=time_bonus
        for e in self.ents:
            try: destroy(e)
            except: pass
        if self.on_complete: self.on_complete('rocket_race',self.score)

    def input(self,key):
        if key=='escape': self._end()
