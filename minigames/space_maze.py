"""Space Maze Mini-Game"""
from ursina import *
import random

class SpaceMaze:
    def __init__(self, player_name='Explorer', on_complete=None):
        self.player_name=player_name; self.on_complete=on_complete
        self.score=0; self.timer=90; self.stars_collected=0; self.ents=[]
        mouse.visible=True
        # UI overlay
        bg=Entity(parent=camera.ui,model='quad',scale=(2,2),color=color.Color(0.01,0,0.06,0.97),z=1); self.ents.append(bg)
        self.title=Text('SPACE MAZE',parent=camera.ui,scale=3,position=(0,0.44),origin=(0,0),color=color.Color(0.3,0.7,1,1)); self.ents.append(self.title)
        self.timer_t=Text('Time: 90',parent=camera.ui,scale=1.8,position=(0,0.36),origin=(0,0),color=color.Color(0.3,1,0.5,1)); self.ents.append(self.timer_t)
        self.star_t=Text('Stars: 0',parent=camera.ui,scale=1.5,position=(-0.55,0.36),origin=(-0.5,0),color=color.Color(1,0.86,0.2,1)); self.ents.append(self.star_t)
        info=Text('W/A/S/D to navigate  Reach the GREEN EXIT!',parent=camera.ui,scale=1.1,position=(0,-0.43),origin=(0,0),color=color.Color(0.6,0.6,0.8,0.7)); self.ents.append(info)
        # Maze grid (0=path,1=wall,2=star,3=exit)
        self.CELL=0.072; self.maze=self._make_maze()
        self.rows=len(self.maze); self.cols=len(self.maze[0])
        self.px=1; self.py=1  # player grid pos (start)
        self._draw_maze()
        self._draw_player()
        self._task=Entity(update=self._update); self.ents.append(self._task)
        self.move_cool=0

    def _make_maze(self):
        return [
            [1,1,1,1,1,1,1,1,1,1,1,1,1],
            [1,0,0,0,1,0,0,0,0,0,1,0,1],
            [1,0,1,0,1,0,1,1,1,0,1,0,1],
            [1,0,1,0,0,0,0,0,1,0,0,0,1],
            [1,0,1,1,1,1,1,0,1,1,1,0,1],
            [1,0,0,0,0,0,1,0,0,0,0,0,1],
            [1,1,1,0,1,0,1,1,1,0,1,0,1],
            [1,0,0,0,1,0,0,0,1,0,1,0,1],
            [1,0,1,1,1,1,1,0,1,0,0,0,1],
            [1,0,0,0,0,0,0,0,0,0,1,2,1],
            [1,1,1,0,1,1,1,1,1,1,1,2,1],
            [1,2,0,0,0,0,0,0,0,0,0,3,1],
            [1,1,1,1,1,1,1,1,1,1,1,1,1],
        ]

    def _cell_pos(self,r,c):
        ox=-self.cols*self.CELL/2; oy=self.rows*self.CELL/2-0.08
        return ox+c*self.CELL, oy-r*self.CELL

    def _draw_maze(self):
        for r,row in enumerate(self.maze):
            for c,cell in enumerate(row):
                x,y=self._cell_pos(r,c)
                if cell==1:
                    e=Entity(parent=camera.ui,model='quad',scale=self.CELL*0.98,
                             position=(x,y,0),color=color.Color(0.2,0.1,0.5,1)); self.ents.append(e)
                elif cell==2:
                    e=Entity(parent=camera.ui,model='quad',scale=self.CELL*0.4,
                             position=(x,y,0),color=color.Color(1,0.9,0.2,1)); self.ents.append(e)
                    e.star_r=r; e.star_c=c; e.is_star=True
                elif cell==3:
                    e=Entity(parent=camera.ui,model='quad',scale=self.CELL*0.9,
                             position=(x,y,0),color=color.Color(0.1,0.9,0.3,1)); self.ents.append(e)
                    lbl=Text('EXIT',parent=camera.ui,scale=0.9,position=(x,y,0),origin=(0,0),color=color.white); self.ents.append(lbl)

    def _draw_player(self):
        if hasattr(self,'player_e') and self.player_e:
            try: destroy(self.player_e)
            except: pass
        x,y=self._cell_pos(self.py,self.px)
        self.player_e=Entity(parent=camera.ui,model='circle',scale=self.CELL*0.6,
                              position=(x,y,-0.1),color=color.Color(0.2,0.8,1,1))
        self.ents.append(self.player_e)

    def _update(self):
        dt=time.dt; self.timer-=dt; self.move_cool-=dt
        self.timer_t.text=f'Time: {max(0,int(self.timer))}'
        if self.timer<=0: self._end(False); return
        if self.move_cool>0: return
        nr,nc=self.py,self.px
        if held_keys['w'] or held_keys['up arrow']: nr-=1
        elif held_keys['s'] or held_keys['down arrow']: nr+=1
        elif held_keys['a'] or held_keys['left arrow']: nc-=1
        elif held_keys['d'] or held_keys['right arrow']: nc+=1
        else: return
        self.move_cool=0.15
        if 0<=nr<self.rows and 0<=nc<self.cols and self.maze[nr][nc]!=1:
            self.py,self.px=nr,nc; self._draw_player()
            cell=self.maze[nr][nc]
            if cell==2:
                self.maze[nr][nc]=0; self.stars_collected+=1
                self.score+=20; self.star_t.text=f'Stars: {self.stars_collected}'
                # Remove star entity
                for e in self.ents:
                    if hasattr(e,'star_r') and e.star_r==nr and hasattr(e,'star_c') and e.star_c==nc:
                        destroy(e); break
            elif cell==3:
                self._end(True)

    def _end(self,won):
        base=500+int(self.timer)*10 if won else 0
        self.score+=base
        for e in self.ents:
            try: destroy(e)
            except: pass
        if self.on_complete: self.on_complete('space_maze',self.score)

    def input(self,key):
        if key=='escape': self._end(False)
