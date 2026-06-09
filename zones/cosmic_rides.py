from ursina import *
from meshes import cylinder, cone
from math import sin, cos, radians, degrees, pi, sqrt
import random
from systems.particle_system import create_flame_effect, create_sparkle_effect

class CosmicRidesZone:
    def __init__(self):
        self.entities = []
        self.emitters = []
        self._build()

    def _build(self):
        e = self.entities
        # ── Zone base ───────────────────────────────────────────────────────
        base = Entity(model='cube', scale=(80,0.1,50), position=(0,0.05,75),
               color=color.Color(0.1, 0.06, 0.18, 1))
        e.append(base)
        zone_text = Text('[ COSMIC RIDES ZONE ]', position=(0,0.5,60),
             scale=5, origin=(0,0), color=color.Color(1, 0.47, 0.2, 1), billboard=True)
        e.append(zone_text)

        # ── Roller coaster support towers ───────────────────────────────────
        coaster_cols = [color.Color(0.86, 0.24, 0.24, 1), color.Color(0.24, 0.71, 0.86, 1)]
        for i, (cx,cz) in enumerate([(-25,65),(-10,72),(5,68),(20,72),(30,65)]):
            h = 8 + (i%3)*3
            tower = Entity(model='cube', scale=(1,h,1),
                   position=(cx, h/2, cz), color=color.Color(0.31, 0.24, 0.47, 1))
            e.append(tower)
            # Track beam
            if i < 4:
                nx,nz = [(-10,72),(5,68),(20,72),(30,65)][i]
                beam = Entity(model='cube',
                       scale=((Vec3(nx,0,nz)-Vec3(cx,0,cz)).length(), 0.6, 0.6),
                       position=((cx+nx)/2, h, (cz+nz)/2),
                       color=coaster_cols[i%2])
                e.append(beam)

        # ── Coaster car (animated position) - decorative ─────────────────────
        self.coaster_t = 0
        self.car = Entity(model='cube', scale=(2,1,1.5),
                          position=(-25, 9, 65), color=color.Color(1, 0.31, 0.31, 1))
        e.append(self.car)
        car_rider = Entity(parent=self.car, model='sphere', scale=0.6,
               position=(0,0.8,0), color=color.Color(1, 0.59, 0.2, 1))
        e.append(car_rider)

        # ── Spinning rocket ride ─────────────────────────────────────────────
        # Central tower
        tower = Entity(model=cylinder(), scale=(1.5,12,1.5),
               position=(25,6,85), color=color.Color(0.24, 0.16, 0.39, 1))
        e.append(tower)
        top_sphere = Entity(model='sphere', scale=2, position=(25,12.5,85),
               color=color.Color(0.39, 0.24, 0.71, 1))
        e.append(top_sphere)
        # Arms + pods
        self.pods = []
        self.pod_t = 0
        for ang in range(0, 360, 90):
            r = radians(ang)
            arm = Entity(model='cube', scale=(6,0.4,0.4),
                         position=(25+sin(r)*3, 11, 85+cos(r)*3),
                         color=color.Color(0.31, 0.24, 0.47, 1))
            e.append(arm)
            pod = Entity(model='cube', scale=(1.5,1,1.5),
                         position=(25+sin(r)*6, 10, 85+cos(r)*6),
                         color=[color.Color(1, 0.24, 0.24, 1),color.Color(0.24, 0.78, 1, 1),
                                color.Color(1, 0.78, 0.2, 1),color.Color(0.39, 1, 0.39, 1)][ang//90])
            e.append(pod)
            rider = Entity(parent=pod, model='sphere', scale=0.55,
                   position=(0,0.7,0), color=color.Color(0.86, 0.86, 0.94, 1))
            e.append(rider)
            self.pods.append((arm, pod, ang))

        # ── Observation Deck ─────────────────────────────────────────────────
        # Support pillars
        for (ox,oz) in [(-10,90),(10,90),(-10,100),(10,100)]:
            pillar = Entity(model='cube', scale=(1,14,1),
                   position=(ox, 7, oz), color=color.Color(0.24, 0.16, 0.39, 1))
            e.append(pillar)
        # Deck floor
        deck = Entity(model='cube', scale=(22,0.8,12),
               position=(0,14,95), color=color.Color(0.2, 0.14, 0.31, 1))
        e.append(deck)
        # Railing
        for rx in range(-10,12,2):
            rail1 = Entity(model='cube', scale=(0.2,1.5,0.2),
                   position=(rx,15,89), color=color.Color(0.31, 0.78, 1, 1))
            e.append(rail1)
            rail2 = Entity(model='cube', scale=(0.2,1.5,0.2),
                   position=(rx,15,101), color=color.Color(0.31, 0.78, 1, 1))
            e.append(rail2)
        rail3 = Entity(model='cube', scale=(22,0.2,0.2), position=(0,16,89),
               color=color.Color(0.31, 0.78, 1, 1))
        e.append(rail3)
        rail4 = Entity(model='cube', scale=(22,0.2,0.2), position=(0,16,101),
               color=color.Color(0.31, 0.78, 1, 1))
        e.append(rail4)
        # Stairs to deck
        for s in range(7):
            stair = Entity(model='cube', scale=(4,0.5,1.5),
                   position=(-12, 1+s*2, 89+s*1.5), color=color.Color(0.24, 0.16, 0.35, 1))
            e.append(stair)
        deck_text = Text('Observation Deck', position=(0,15.5,95),
             scale=3, origin=(0,0), color=color.Color(0.31, 0.78, 1, 1), billboard=True)
        e.append(deck_text)

        # ── Rocket Launchpad ─────────────────────────────────────────────────
        launch_base = Entity(model=cylinder(), scale=(10,0.8,10),
               position=(-30, 0.4, 85), color=color.Color(0.16, 0.12, 0.27, 1))
        e.append(launch_base)
        # Rocket body
        rocket_body = Entity(model=cylinder(), scale=(2.5,12,2.5),
               position=(-30, 7, 85), color=color.Color(0.86, 0.86, 0.94, 1))
        e.append(rocket_body)
        # Nose cone
        nose = Entity(model=cone(), scale=(2.5,4,2.5),
               position=(-30,14,85), color=color.Color(1, 0.31, 0.31, 1))
        e.append(nose)
        # Fins
        for ang in range(0,360,120):
            r = radians(ang)
            fin = Entity(model='cube', scale=(1,3,3),
                   position=(-30+sin(r)*2, 2, 85+cos(r)*2),
                   rotation=(0,ang,0), color=color.Color(0.78, 0.24, 0.24, 1))
            e.append(fin)
        # Launch flame (decorative)
        self.flame_t = 0
        self.flames = []
        for _ in range(3):
            f = Entity(model=cone(), scale=(1.5,2,1.5),
                       position=(-30, -1, 85), color=color.Color(1, 0.59, 0.2, 1),
                       rotation=(180,0,0))
            self.flames.append(f)
            e.append(f)

        # Rocket launch flame particle effects
        flame_emitter = create_flame_effect(Vec3(-30, 0, 85), duration=999)
        self.emitters.append(flame_emitter)

    def update(self):
        dt = time.dt
        # Coaster car animation along waypoints
        self.coaster_t += dt * 0.4
        t_loop = self.coaster_t % (2*3.14159)
        self.car.x = -25 + 55 * (0.5 - 0.5*cos(t_loop))
        self.car.z = 65 + 7 * sin(t_loop * 2)
        self.car.y = 9 + 3 * sin(t_loop * 3)

        # Spinning pods
        self.pod_t += dt * 60
        for arm, pod, base_ang in self.pods:
            r = radians(base_ang + self.pod_t)
            arm.x = 25 + sin(r)*3; arm.z = 85 + cos(r)*3
            pod.x = 25 + sin(r)*6; pod.z = 85 + cos(r)*6
            arm.rotation_y = base_ang + self.pod_t
            pod.rotation_y = base_ang + self.pod_t

        # Flickering launch flame
        self.flame_t += dt
        import math
        fs = 0.8 + 0.4*abs(math.sin(self.flame_t * 8)) + random.uniform(0,0.2)
        for fl in self.flames:
            fl.scale_y = fs * 2

        # Update particle emitters
        for emitter in self.emitters:
            if hasattr(emitter, 'update'):
                emitter.update()

    def destroy(self):
        for e in self.entities:
            try:
                destroy(e)
            except:
                pass
        for emitter in self.emitters:
            if hasattr(emitter, 'destroy_all'):
                emitter.destroy_all()
