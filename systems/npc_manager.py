"""NPC Manager - Ambient alien characters for the park."""
from ursina import *
from math import sin, cos, radians, degrees, atan2, pi
import random

class NPC(Entity):
    """Simple ambient NPC character."""
    def __init__(self, start_pos, waypoints, color_scheme):
        super().__init__(model=None, position=start_pos)
        self.color_scheme = color_scheme
        self.waypoints = waypoints
        self.current_waypoint = 0
        self.speed = random.uniform(3, 5)
        self.walk_t = 0

        # Build simple alien model
        suit_color = color_scheme['suit']
        # Body
        self.torso = Entity(parent=self, model='cube', color=suit_color,
                           scale=(0.5, 0.6, 0.4))
        # Head
        self.head = Entity(parent=self, model='sphere', color=suit_color,
                          scale=0.4, position=(0, 0.5, 0))
        # Eyes
        Entity(parent=self.head, model='sphere', scale=0.08,
              position=(-0.1, 0.1, -0.2), color=color.black)
        Entity(parent=self.head, model='sphere', scale=0.08,
              position=(0.1, 0.1, -0.2), color=color.black)
        # Antenna
        Entity(parent=self.head, model='cube', scale=(0.04, 0.3, 0.04),
              position=(0, 0.25, 0), color=color_scheme.get('accent', color.yellow))
        Entity(parent=self.head, model='sphere', scale=0.08,
              position=(0, 0.35, 0), color=color_scheme.get('accent', color.yellow))

    def update(self):
        if not self.waypoints or len(self.waypoints) == 0:
            return

        dt = time.dt
        target = self.waypoints[self.current_waypoint]
        direction = (target - self.position)
        distance = direction.length()

        if distance < 1:
            self.current_waypoint = (self.current_waypoint + 1) % len(self.waypoints)
            return

        # Move towards waypoint
        direction = direction.normalized()
        self.position += direction * self.speed * dt

        # Face direction
        self.rotation_y = degrees(atan2(direction.x, direction.z))

        # Walking animation
        self.walk_t += dt * self.speed
        sw = sin(self.walk_t * 4) * 15
        # Simple walking - just keep moving, animation is subtle

class NPCManager:
    """Manages ambient NPC characters."""
    def __init__(self):
        self.npcs = []
        self._define_paths()
        self._spawn_npcs()

    def _define_paths(self):
        """Define waypoint paths for different areas."""
        self.paths = {
            'entry_plaza': [
                Vec3(-12, 1, 10),
                Vec3(0, 1, -15),
                Vec3(12, 1, 10),
                Vec3(0, 1, 20),
            ],
            'games_zone_a': [
                Vec3(-25, 1, 15),
                Vec3(-15, 1, 40),
                Vec3(-5, 1, 30),
                Vec3(-20, 1, 20),
            ],
            'games_zone_b': [
                Vec3(20, 1, 15),
                Vec3(25, 1, 35),
                Vec3(10, 1, 40),
                Vec3(15, 1, 20),
            ],
            'cosmic_path_a': [
                Vec3(-25, 1, 65),
                Vec3(-10, 1, 80),
                Vec3(10, 1, 75),
                Vec3(0, 1, 60),
            ],
            'cosmic_path_b': [
                Vec3(20, 1, 70),
                Vec3(30, 1, 85),
                Vec3(25, 1, 100),
                Vec3(15, 1, 90),
            ],
        }

        # Color schemes for alien NPCs
        self.color_schemes = [
            {'suit': color.Color(0.2, 0.7, 0.2, 1), 'accent': color.Color(0.8, 1, 0.2, 1)},
            {'suit': color.Color(0.2, 0.3, 0.8, 1), 'accent': color.Color(0.5, 0.9, 1, 1)},
            {'suit': color.Color(0.7, 0.2, 0.2, 1), 'accent': color.Color(1, 0.5, 0.2, 1)},
            {'suit': color.Color(0.7, 0.2, 0.7, 1), 'accent': color.Color(1, 0.5, 1, 1)},
            {'suit': color.Color(0.7, 0.7, 0.2, 1), 'accent': color.Color(1, 1, 0.5, 1)},
            {'suit': color.Color(0.2, 0.7, 0.7, 1), 'accent': color.Color(0.5, 1, 1, 1)},
        ]

    def _spawn_npcs(self):
        """Spawn NPCs at various locations."""
        spawn_configs = [
            ('entry_plaza', 2),
            ('games_zone_a', 1),
            ('games_zone_b', 1),
            ('cosmic_path_a', 2),
            ('cosmic_path_b', 2),
        ]

        for path_key, count in spawn_configs:
            path = self.paths[path_key]
            for _ in range(count):
                color_scheme = random.choice(self.color_schemes)
                start_pos = random.choice(path)
                npc = NPC(start_pos, path, color_scheme)
                self.npcs.append(npc)

    def update(self):
        """Update all NPCs."""
        for npc in self.npcs:
            npc.update()

    def destroy(self):
        """Destroy all NPCs."""
        for npc in self.npcs:
            try:
                destroy(npc)
            except:
                pass
        self.npcs.clear()
