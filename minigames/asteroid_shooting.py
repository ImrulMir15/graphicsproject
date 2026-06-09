"""Asteroid Shooting Mini-Game"""
from ursina import *
import random

class AsteroidShooting:
    def __init__(self, player_name='Explorer', on_complete=None):
        self.player_name=player_name; self.on_complete=on_complete
        self.score=0; self.timer=60; self.misses=3; self.streak=0
        self.asteroids=[]; self.ents=[]
        mouse.visible=True
        # Background
        bg=Entity(parent=camera.ui,model='quad',scale=(2,2),color=color.Color(0.01,0,0.05,0.97),z=1); self.ents.append(bg)
        # Title + stats
        self.title_t=Text('ASTEROID SHOOTING',parent=camera.ui,scale=2.5,position=(0,0.44),origin=(0,0),color=color.Color(0.3,0.86,1,1)); self.ents.append(self.title_t)
        self.score_t=Text('Score: 0',parent=camera.ui,scale=1.8,position=(-0.6,0.38),origin=(-0.5,0),color=color.Color(1,0.86,0.2,1)); self.ents.append(self.score_t)
        self.timer_t=Text('Time: 60',parent=camera.ui,scale=1.8,position=(0,0.38),origin=(0,0),color=color.Color(0.3,1,0.5,1)); self.ents.append(self.timer_t)
        self.miss_t=Text('Misses: 0/3',parent=camera.ui,scale=1.5,position=(0.45,0.38),origin=(0,0),color=color.Color(1,0.4,0.4,1)); self.ents.append(self.miss_t)
        self.streak_t=Text('',parent=camera.ui,scale=1.4,position=(0,0.30),origin=(0,0),color=color.Color(1,0.6,0.2,1)); self.ents.append(self.streak_t)
        self.info=Text('Click asteroids to shoot!  [ESC] Exit',parent=camera.ui,scale=1,position=(0,-0.44),origin=(0,0),color=color.Color(0.6,0.6,0.8,0.7)); self.ents.append(self.info)
        # Spawn asteroids
        self._spawn_batch()
        self._update_task=Entity(update=self._update); self.ents.append(self._update_task)

    def _spawn_batch(self):
        for _ in range(6): self._spawn_one()

    def _spawn_one(self):
        sizes=[(0.06,'small',10),(0.10,'medium',25),(0.16,'large',50)]
        sc,kind,pts=random.choice(sizes)
        x=random.uniform(-0.75,0.75); y=random.uniform(-0.35,0.32)
        cols=[color.Color(0.6,0.5,0.4,1),color.Color(0.7,0.6,0.3,1),color.Color(0.5,0.4,0.5,1)]
        col=random.choice(cols)
        ast=Entity(parent=camera.ui,model='sphere',scale=sc,position=(x,y,0),color=col)
        ast.kind=kind; ast.pts=pts
        ast.vx=random.uniform(-0.1,0.1); ast.vy=random.uniform(-0.08,0.08)
        ast.rot_spd=random.uniform(-90,90)
        self.asteroids.append(ast); self.ents.append(ast)

    def _update(self):
        dt=time.dt
        self.timer-=dt
        self.timer_t.text=f'Time: {max(0,int(self.timer))}'
        if self.timer<=0: self._end(); return
        for ast in list(self.asteroids):
            ast.x+=ast.vx*dt; ast.y+=ast.vy*dt; ast.rotation_z+=ast.rot_spd*dt
            if abs(ast.x)>0.9 or abs(ast.y)>0.5: ast.vx*=-1; ast.vy*=-1
            if mouse.left and self._hovered(ast):
                mult=2 if self.streak>=5 else 1
                self.score+=ast.pts*mult; self.streak+=1
                self.score_t.text=f'Score: {self.score}'
                self.streak_t.text=f'STREAK x{self.streak}! x2 BONUS!' if self.streak>=5 else f'Streak: {self.streak}'
                self.asteroids.remove(ast); destroy(ast); self._spawn_one()

    def _hovered(self,ast):
        mp=mouse.position; ap=Vec2(ast.x,ast.y)
        return (mp-ap).length()<ast.scale[0]*0.7

    def _end(self):
        for e in self.ents:
            try: destroy(e)
            except: pass
        if self.on_complete: self.on_complete('asteroid_shooting',self.score)

    def input(self,key):
        if key=='escape': self._end()
