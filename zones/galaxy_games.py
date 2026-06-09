from ursina import *
from meshes import cylinder, cone
from systems.particle_system import create_sparkle_effect

KIOSKS = [
    {'name': 'Asteroid Shooting', 'game': 'asteroid_shooting',
     'pos': (-20, 0, 20), 'color': color.Color(1, 0.31, 0.2, 1)},
    {'name': 'Alien Whack-a-Mole', 'game': 'whack_a_mole',
     'pos': (20, 0, 20),  'color': color.Color(0.31, 0.86, 0.31, 1)},
    {'name': 'Space Maze',         'game': 'space_maze',
     'pos': (-20, 0, 45), 'color': color.Color(0.31, 0.63, 1, 1)},
    {'name': 'Rocket Race',        'game': 'rocket_race',
     'pos': (20, 0, 45),  'color': color.Color(1, 0.78, 0.2, 1)},
]

class GalaxyGamesZone:
    def __init__(self, on_enter_game):
        self.on_enter_game = on_enter_game
        self.entities = []
        self.colliders = []
        self.emitters = []
        self._build()

    def _build(self):
        # Zone sign
        e = Entity(model='cube', scale=(40,0.1,40), position=(0,0.05,33),
               color=color.Color(0.12, 0.08, 0.22, 1))
        self.entities.append(e)
        t = Text('[ GALAXY GAMES ZONE ]', position=(0,0.5,33),
             scale=5, origin=(0,0), color=color.Color(0.78, 0.39, 1, 1), billboard=True)
        self.entities.append(t)

        for k in KIOSKS:
            px,py,pz = k['pos']
            kc = k['color']

            # Platform
            plat = Entity(model=cylinder(), scale=(5,0.4,5),
                          position=(px, 0.2, pz), color=kc)
            self.entities.append(plat)
            # Glow ring
            ring = Entity(model=cylinder(), scale=(5.5,0.15,5.5),
                          position=(px, 0.4, pz),
                          color=color.rgba(kc.r*255, kc.g*255, kc.b*255, 120))
            self.entities.append(ring)
            # Booth back wall
            wall = Entity(model='cube', scale=(4,4.5,0.4),
                   position=(px, 2.6, pz+2.2), color=color.Color(0.12, 0.08, 0.22, 1))
            self.entities.append(wall)
            # Booth roof
            roof = Entity(model='cube', scale=(5,0.3,5),
                   position=(px, 5, pz), color=kc)
            self.entities.append(roof)
            # Sign
            sign = Entity(model='cube', scale=(3.8,1.2,0.2),
                   position=(px, 4.2, pz+2.1), color=kc)
            self.entities.append(sign)
            sig_text = Text(k['name'], position=(px, 4.2, pz+1.9),
                 scale=2.8, origin=(0,0), color=color.white, billboard=True)
            self.entities.append(sig_text)
            # Arcade screen (fake)
            screen = Entity(model='cube', scale=(2.5,2,0.15),
                   position=(px, 2.5, pz+2.0), color=color.Color(0.04, 0.04, 0.12, 1))
            self.entities.append(screen)
            display = Entity(model='cube', scale=(2.2,1.7,0.1),
                   position=(px, 2.5, pz+1.94), color=color.Color(0.08, 0.31, 0.16, 1))
            self.entities.append(display)

            # Invisible trigger collider
            trigger = Entity(model='cube', scale=(6,4,6),
                             position=(px, 2, pz),
                             collider='box',
                             color=color.Color(0, 0, 0, 0),
                             visible=False,
                             game_key=k['game'],
                             game_name=k['name'])
            self.colliders.append(trigger)
            self.entities.append(trigger)

            # Add sparkle effect above kiosk
            sparkle_emitter = create_sparkle_effect(Vec3(px, 9, pz), color_=kc)
            self.emitters.append(sparkle_emitter)

    def check_proximity(self, player_pos, player):
        """Call each frame from main update."""
        nearest = None
        nearest_dist = 7.0
        for t in self.colliders:
            d = (Vec3(t.x, 0, t.z) - Vec3(player_pos.x, 0, player_pos.z)).length()
            if d < nearest_dist:
                nearest_dist = d
                nearest = t.game_name
        player.set_near_kiosk(nearest)

    def get_game_key(self, game_name):
        for k in KIOSKS:
            if k['name'] == game_name:
                return k['game']
        return None

    def destroy(self):
        for e in self.entities:
            try:
                destroy(e)
            except:
                pass
        for emitter in self.emitters:
            if hasattr(emitter, 'destroy_all'):
                emitter.destroy_all()

    def update(self):
        for emitter in self.emitters:
            if hasattr(emitter, 'update'):
                emitter.update()

