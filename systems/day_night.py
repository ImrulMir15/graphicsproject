from ursina import *
from math import sin

class DayNightSystem:
    def __init__(self, sky_entity):
        self.sky = sky_entity
        self.is_day = True
        self.t = 1.0
        self.transitioning = False
        self.duration = 2.0

        self.DAY_SKY   = color.Color(0.33, 0.16, 0.66, 1)
        self.NIGHT_SKY = color.Color(0.01, 0.01, 0.06, 1)

        # Day lighting setup
        self.DAY_COLORS = {
            'sun': color.Color(1.0, 0.95, 0.82, 1),
            'sun2': color.Color(0.48, 0.45, 0.60, 1),
            'amb': color.Color(0.62, 0.58, 0.72, 1),
        }

        # Night lighting setup (more atmospheric)
        self.NIGHT_COLORS = {
            'sun': color.Color(0.2, 0.18, 0.35, 1),
            'sun2': color.Color(0.15, 0.12, 0.25, 1),
            'amb': color.Color(0.15, 0.12, 0.22, 1),
        }

        # Get light references from global state (will be set by main.py)
        self.lights = None

        # Stars (visible at night)
        self.stars = []
        import random
        for _ in range(300):
            s = Entity(
                model='sphere',
                scale=random.uniform(0.1, 0.4),
                position=(random.uniform(-150, 150),
                          random.uniform(30, 100),
                          random.uniform(-150, 150)),
                color=color.white,
                unlit=True,
            )
            self.stars.append(s)
        self._set_star_alpha(0)

        # Toggle button
        self.btn = Button(
            text='[N] Day',
            parent=camera.ui,
            position=(0.62, 0.46),
            scale=(0.16, 0.05),
            color=color.Color(1, 0.78, 0.2, 0.8),
            text_color=color.black,
            on_click=self.toggle,
            z=-1,
        )

    def _set_star_alpha(self, a):
        for s in self.stars:
            s.color = color.Color(1, 1, 1, a)

    def toggle(self):
        if not self.transitioning:
            self.is_day = not self.is_day
            self.transitioning = True
            self.t = 0.0
            self.btn.text = '[N] Night' if not self.is_day else '[N] Day'

    def update(self):
        if self.transitioning:
            self.t += time.dt / self.duration
            if self.t >= 1.0:
                self.t = 1.0
                self.transitioning = False

            if self.is_day:
                # Transitioning to day
                self.sky.color = lerp(self.NIGHT_SKY, self.DAY_SKY, self.t)
                self._set_star_alpha(1 - self.t)
                if self.lights:
                    for key in self.lights:
                        self.lights[key].color = lerp(
                            self.NIGHT_COLORS[key],
                            self.DAY_COLORS[key],
                            self.t
                        )
            else:
                # Transitioning to night
                self.sky.color = lerp(self.DAY_SKY, self.NIGHT_SKY, self.t)
                self._set_star_alpha(self.t)
                if self.lights:
                    for key in self.lights:
                        self.lights[key].color = lerp(
                            self.DAY_COLORS[key],
                            self.NIGHT_COLORS[key],
                            self.t
                        )

    def destroy(self):
        for s in self.stars:
            destroy(s)
        destroy(self.btn)
