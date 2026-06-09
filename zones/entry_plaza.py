from ursina import *
from math import sin, cos, radians, degrees, atan2, pi
import random
from systems.particle_system import create_fountain_effect

def _box(pos, scale, col, parent=scene):
    return Entity(parent=parent, model='cube', position=pos, scale=scale, color=col)

def _pillar(x, z, h=6, col=None):
    col = col or color.Color(0.31, 0.24, 0.47, 1)
    _box((x, h/2, z), (1.2,h,1.2), col)
    _box((x, h+0.4, z), (1.8,0.5,1.8), color.Color(0.47, 0.31, 0.71, 1))  # cap

class EntryPlaza:
    def __init__(self):
        self.entities = []
        self.emitters = []
        self._build()

    def _build(self):
        e = self.entities
        # ── Welcome Gate arch ───────────────────────────────────────────────
        _pillar(-6, -30, 9, color.Color(0.24, 0.16, 0.39, 1))
        _pillar( 6, -30, 9, color.Color(0.24, 0.16, 0.39, 1))
        # Arch beam
        e.append(_box((0, 9.8, -30), (13,1.2,1.2), color.Color(0.39, 0.24, 0.71, 1)))
        e.append(_box((0, 9.8, -30), (11,0.8,1.6), color.Color(0.55, 0.31, 0.86, 1)))
        # Sign
        sign = _box((0, 12, -30), (10,2.2,0.3), color.Color(0.08, 0.04, 0.2, 1))
        e.append(sign)
        e.append(Text('SPACE PARK FUN WORLD', position=(0,12,-30.3),
                      scale=3.5, origin=(0,0), color=color.Color(0.31, 0.86, 1, 1),
                      billboard=True))

        # ── Glowing fountain ────────────────────────────────────────────────
        e.append(_box((0,0.4,  -15), (4,0.8,4),   color.Color(0.24, 0.16, 0.39, 1)))   # base
        e.append(_box((0,1.0,  -15), (2.8,1.2,2.8), color.Color(0.31, 0.24, 0.55, 1))) # basin
        e.append(_box((0,1.6,  -15), (0.6,3,0.6), color.Color(0.39, 0.31, 0.63, 1)))  # pillar
        # Water jets (tall thin spheres)
        for ang in range(0,360,90):
            r = radians(ang)
            e.append(Entity(model='sphere', scale=(0.25,1.8,0.25),
                            position=(sin(r)*0.8, 3.2, -15+cos(r)*0.8),
                            color=color.Color(0.31, 0.78, 1, 1)))
        e.append(Entity(model='sphere', scale=0.7,
                        position=(0,5,-15), color=color.Color(0.39, 0.86, 1, 1)))

        # Fountain particle effects
        fountain_emitter = create_fountain_effect(Vec3(0, 2, -15), duration=999)
        self.emitters.append(fountain_emitter)

        # ── Path tiles ──────────────────────────────────────────────────────
        for z in range(-28, 10, 4):
            for x in [-1, 0, 1]:
                c = color.Color(0.27, 0.2, 0.39, 1) if (z+x)%2==0 else color.Color(0.22, 0.15, 0.31, 1)
                e.append(_box((x*2, 0.05, z), (1.9,0.1,3.8), c))

        # ── Alien NPC decorations ────────────────────────────────────────────
        npc_positions = [(-12,-10),(-15,5),(12,-8),(14,2),(-10,15),(10,12)]
        npc_colors    = [color.Color(0.39, 0.78, 0.39, 1), color.Color(0.78, 0.39, 0.39, 1),
                         color.Color(0.39, 0.39, 0.78, 1), color.Color(0.78, 0.78, 0.31, 1),
                         color.Color(0.78, 0.39, 0.78, 1), color.Color(0.31, 0.78, 0.78, 1)]
        for (nx,nz), nc in zip(npc_positions, npc_colors):
            # body
            e.append(_box((nx, 1.0, nz),    (0.7,1.0,0.5), nc))
            e.append(Entity(model='sphere', scale=0.55,
                            position=(nx,1.85,nz), color=nc))
            # eyes
            e.append(Entity(model='sphere', scale=0.13,
                            position=(nx-0.12,1.95,nz-0.26), color=color.black))
            e.append(Entity(model='sphere', scale=0.13,
                            position=(nx+0.12,1.95,nz-0.26), color=color.black))
            # antenna
            e.append(_box((nx, 2.55, nz),(0.06,0.5,0.06), nc))
            e.append(Entity(model='sphere', scale=0.12,
                            position=(nx,2.85,nz), color=color.Color(1, 1, 0.39, 1)))

        # ── Decorative banners / flags ───────────────────────────────────────
        for i, (bx, bz) in enumerate([(-8,-25),(8,-25),(-8,-5),(8,-5)]):
            bc = [color.Color(1, 0.31, 0.31, 1),color.Color(0.31, 0.31, 1, 1),
                  color.Color(1, 0.78, 0.2, 1),color.Color(0.31, 0.86, 0.31, 1)][i%4]
            _pillar(bx, bz, 8)
            e.append(_box((bx+1,7.5,bz), (2,1.5,0.1), bc))

        # ── Zone label ───────────────────────────────────────────────────────
        e.append(Text('* ENTRY PLAZA', position=(0,0.5,0),
                      scale=5, origin=(0,0), color=color.Color(1, 0.86, 0.31, 1),
                      billboard=True))

    def destroy(self):
        for e in self.entities:
            destroy(e)
        for emitter in self.emitters:
            if hasattr(emitter, 'destroy_all'):
                emitter.destroy_all()
        self.entities.clear()
        self.emitters.clear()

    def update(self):
        for emitter in self.emitters:
            if hasattr(emitter, 'update'):
                emitter.update()
