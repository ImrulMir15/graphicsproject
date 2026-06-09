from ursina import *
from math import sin, cos, radians, degrees, atan2, pi

class SpacePlayer(Entity):
    def __init__(self, gender='male', on_interact=None):
        super().__init__(
            model=None,
            collider='box',
            scale=(0.8, 1.8, 0.8),
            position=(5, 1, 5),
            visible=True,
        )
        self.on_interact = on_interact
        self.gender = gender

        # Suit colours
        suit  = color.Color(0.86, 0.86, 0.94, 1) if gender=='male' else color.Color(0.94, 0.82, 0.9, 1)
        visor = color.Color(0.12, 0.47, 0.86, 1)  if gender=='male' else color.Color(0.71, 0.24, 0.86, 1)
        acc   = color.Color(0.2, 0.63, 1, 1)  if gender=='male' else color.Color(0.82, 0.31, 0.9, 1)

        # Torso
        self.torso = Entity(parent=self, model='cube', color=suit,  scale=(0.75, 0.72, 0.52))
        Entity(parent=self.torso, model='cube', color=color.Color(0.71, 0.71, 0.78, 1), scale=(0.65,0.7,0.3),  position=(0,0,0.38))  # backpack
        Entity(parent=self.torso, model='cube', color=acc,           scale=(0.38,0.22,0.08), position=(0,0.12,-0.38)) # chest panel

        # Helmet
        self.head = Entity(parent=self, model='sphere', color=suit, scale=0.56, position=(0,0.65,0))
        Entity(parent=self.head, model='sphere', color=visor, scale=(0.72,0.52,0.52), position=(0,0,-0.28))

        # Arms
        self.larm = Entity(parent=self, model='cube', color=suit, scale=(0.26,0.58,0.26), position=(-0.5,0.04,0))
        self.rarm = Entity(parent=self, model='cube', color=suit, scale=(0.26,0.58,0.26), position=( 0.5,0.04,0))
        Entity(parent=self.larm, model='cube', color=acc, scale=(1.15,0.22,1.15), position=(0, 0.42,0))
        Entity(parent=self.rarm, model='cube', color=acc, scale=(1.15,0.22,1.15), position=(0, 0.42,0))

        # Legs
        self.lleg = Entity(parent=self, model='cube', color=suit, scale=(0.3,0.62,0.3), position=(-0.22,-0.72,0))
        self.rleg = Entity(parent=self, model='cube', color=suit, scale=(0.3,0.62,0.3), position=( 0.22,-0.72,0))
        Entity(parent=self.lleg, model='cube', color=color.Color(0.2, 0.2, 0.27, 1), scale=(1.1,0.2,1.35), position=(0,-0.55,0.05))
        Entity(parent=self.rleg, model='cube', color=color.Color(0.2, 0.2, 0.27, 1), scale=(1.1,0.2,1.35), position=(0,-0.55,0.05))

        # Physics
        self.vel_y      = 0.0
        self.on_ground  = False
        self.speed      = 8
        self.run_speed  = 15
        self.current_speed = 0  # Actual speed (with acceleration)
        self.jump_force = 12
        self.walk_t     = 0.0
        self.accel      = 25  # Acceleration rate
        self.friction   = 0.85  # Deceleration factor

        # Camera
        self.cam_mode   = 'third'    # 'third' | 'first'
        self.sensitivity= 80

        # Interaction
        self.near_kiosk    = None
        self.interact_hint = Text('', parent=camera.ui, origin=(0,0), position=(0,-0.38),
                                  color=color.Color(1, 1, 0.39, 1), scale=1.4)

        # HUD
        self.cam_label = Text('[C] 3rd Person', parent=camera.ui,
                              origin=(-0.5,-0.5), position=(-0.85,-0.47),
                              color=color.Color(0.78, 0.78, 1, 0.71), scale=0.9)

        camera.fov = 80

    # ─────────────────────────────────────────────────────────────────────────
    def switch_camera(self):
        if self.cam_mode == 'third':
            self.cam_mode = 'first'
            mouse.visible = True
            self.torso.visible = False
            self.head.visible  = False
            self.larm.visible  = False
            self.rarm.visible  = False
            self.lleg.visible  = False
            self.rleg.visible  = False
            for c in self.torso.children + self.head.children:
                c.visible = False
            self.cam_label.text = '[C] 1st Person'
        else:
            self.cam_mode = 'third'
            mouse.visible = True
            for e in [self.torso, self.head, self.larm, self.rarm, self.lleg, self.rleg]:
                e.visible = True
                for c in e.children:
                    c.visible = True
            self.cam_label.text = '[C] 3rd Person'

    # ─────────────────────────────────────────────────────────────────────────
    def update(self):
        dt = time.dt

        # Gravity
        self.vel_y -= 30 * dt
        if self.vel_y < -50:  # Terminal velocity
            self.vel_y = -50

        # Calculate target speed based on input
        target_speed = self.run_speed if held_keys['shift'] else self.speed

        # Direction vectors from player's y-rotation
        ry  = radians(self.rotation_y)
        fwd = Vec3(sin(ry), 0, cos(ry))
        rgt = Vec3(cos(ry), 0,-sin(ry))

        move = Vec3(0, 0, 0)
        if held_keys['w']: move += fwd
        if held_keys['s']: move -= fwd
        if held_keys['a']: move -= rgt
        if held_keys['d']: move += rgt

        moving = move.length() > 0.01

        if moving:
            move = move.normalized()
            # Smooth acceleration towards target speed
            self.current_speed = lerp(self.current_speed, target_speed, dt * self.accel)

            # Animate walking/running
            self.walk_t += dt * self.current_speed * 1.2
            sw = sin(self.walk_t * 4) * 28
            self.larm.rotation_x =  sw
            self.rarm.rotation_x = -sw
            self.lleg.rotation_x = -sw
            self.rleg.rotation_x =  sw

            # Head bob during movement
            head_bob = sin(self.walk_t * 4) * 0.15
            self.head.y = 0.65 + head_bob

            # Face movement direction (3rd person) with smooth rotation
            if self.cam_mode == 'third':
                tgt = degrees(atan2(move.x, move.z))
                self.rotation_y = lerp(self.rotation_y, tgt, dt * 10)
        else:
            # Decelerate when not moving
            self.current_speed *= self.friction
            if self.current_speed < 0.1:
                self.current_speed = 0

            # Return to idle pose
            for part in [self.larm, self.rarm, self.lleg, self.rleg]:
                part.rotation_x = lerp(part.rotation_x, 0, dt * 8)

            # Idle head bob (breathing)
            idle_bob = sin(time.time() * 2) * 0.05
            self.head.y = lerp(self.head.y, 0.65 + idle_bob, dt * 3)

        # Apply horizontal movement
        self.position += move * self.current_speed * dt
        # Apply vertical
        self.position += Vec3(0, self.vel_y * dt, 0)

        # Ground raycast
        ray = raycast(self.position + Vec3(0, 0.1, 0), Vec3(0, -1, 0), distance=1.3, ignore=[self])
        if ray.hit and self.vel_y <= 0:
            # Landing
            if not self.on_ground and self.vel_y < -20:
                # Landing animation - slight crouch
                for part in [self.lleg, self.rleg]:
                    part.scale_y = 0.5
                    animate(part, scale_y=0.62, duration=0.2)

            self.vel_y = 0
            self.y     = ray.world_point.y + 0.9
            self.on_ground = True
        else:
            self.on_ground = False

        # Clamp to world bounds
        self.x = clamp(self.x, -90, 90)
        self.z = clamp(self.z, -90, 90)

        # Camera
        if self.cam_mode == 'first':
            self._cam_first(dt)
        else:
            self._cam_third(dt)

    # ─────────────────────────────────────────────────────────────────────────
    def _cam_first(self, dt):
        camera.position = self.position + Vec3(0, 0.55, 0)
        # Use arrow keys for looking in first person
        if held_keys['right arrow']: self.rotation_y += self.sensitivity * dt
        if held_keys['left arrow']:  self.rotation_y -= self.sensitivity * dt
        camera.rotation_y = self.rotation_y
        if held_keys['up arrow']:    camera.rotation_x -= self.sensitivity * dt * 0.6
        if held_keys['down arrow']:  camera.rotation_x += self.sensitivity * dt * 0.6
        camera.rotation_x = clamp(camera.rotation_x, -80, 80)

    def _cam_third(self, dt):
        ry   = radians(self.rotation_y)
        back = Vec3(sin(ry), 0, cos(ry)) * -8
        tgt  = self.position + Vec3(0, 4.5, 0) + back
        camera.position = lerp(camera.position, tgt, dt * 6)
        camera.look_at(self.position + Vec3(0, 1.5, 0))

    # ─────────────────────────────────────────────────────────────────────────
    def input(self, key):
        if key == 'space' and self.on_ground:
            self.vel_y     = self.jump_force
            self.on_ground = False
        if key == 'c':
            self.switch_camera()
        if key == 'e' and self.near_kiosk and self.on_interact:
            self.on_interact(self.near_kiosk)

    def set_near_kiosk(self, kiosk_name):
        self.near_kiosk = kiosk_name
        self.interact_hint.text = f'[E] Play {kiosk_name}' if kiosk_name else ''

    def hide_ui(self):
        self.interact_hint.enabled = False
        self.cam_label.enabled     = False

    def show_ui(self):
        self.interact_hint.enabled = True
        self.cam_label.enabled     = True
