"""Particle Effects System for visual polish."""
from ursina import *
from math import sin, cos, pi, sqrt
import random

class Particle(Entity):
    """A single particle with velocity, lifetime, and size."""
    def __init__(self, position, velocity, lifetime, color, size=0.1):
        super().__init__(
            model='sphere',
            scale=size,
            position=position,
            color=color
        )
        self.velocity = velocity
        self.lifetime = lifetime
        self.max_lifetime = lifetime
        self.size_start = size
        self.opacity_start = color.a if hasattr(color, 'a') else 1

    def update(self):
        """Update particle position, fade and size."""
        dt = time.dt
        self.lifetime -= dt

        if self.lifetime <= 0:
            destroy(self)
            return

        # Move particle
        self.position += self.velocity * dt

        # Apply gravity to velocity
        self.velocity.y -= 9.8 * dt

        # Fade out towards end of life
        fade = self.lifetime / self.max_lifetime
        current_color = self.color
        self.color = color.Color(
            current_color.r,
            current_color.g,
            current_color.b,
            self.opacity_start * fade
        )

        # Shrink towards end of life
        self.scale = self.size_start * fade

class ParticleEmitter(Entity):
    """Emits particles over time."""
    def __init__(self, position, particle_color, emission_rate=10, lifetime=1,
                 velocity_range=(-2, 2), size=0.1):
        super().__init__(position=position)
        self.particle_color = particle_color
        self.emission_rate = emission_rate
        self.lifetime = lifetime
        self.velocity_range = velocity_range
        self.size = size
        self.particles = []
        self.emission_timer = 0
        self.active = True

    def update(self):
        """Emit new particles and update existing ones."""
        if not self.active:
            return

        dt = time.dt
        self.emission_timer += dt

        # Emit particles
        while self.emission_timer >= (1 / self.emission_rate):
            self.emission_timer -= (1 / self.emission_rate)

            # Random velocity
            vx = random.uniform(self.velocity_range[0], self.velocity_range[1])
            vy = random.uniform(self.velocity_range[0], self.velocity_range[1])
            vz = random.uniform(self.velocity_range[0], self.velocity_range[1])
            velocity = Vec3(vx, vy, vz)

            p = Particle(
                self.position,
                velocity,
                random.uniform(self.lifetime * 0.8, self.lifetime * 1.2),
                self.particle_color,
                self.size
            )
            self.particles.append(p)

        # Update all particles
        for p in list(self.particles):
            try:
                if p and p.lifetime > 0:
                    p.update()
                else:
                    try:
                        destroy(p)
                    except:
                        pass
                    if p in self.particles:
                        self.particles.remove(p)
            except:
                if p in self.particles:
                    self.particles.remove(p)

    def stop(self):
        """Stop emitting new particles."""
        self.active = False

    def destroy_all(self):
        """Destroy all particles and emitter."""
        for p in self.particles:
            try:
                destroy(p)
            except:
                pass
        self.particles.clear()
        try:
            destroy(self)
        except:
            pass

# ═══════════════════════════════════════════════════════════════════════════════
# PRESET EFFECTS
# ═══════════════════════════════════════════════════════════════════════════════

def create_fountain_effect(pos, duration=10):
    """Create a water fountain spray effect."""
    emitter = ParticleEmitter(
        position=pos,
        particle_color=color.Color(0.31, 0.78, 1, 0.6),
        emission_rate=15,
        lifetime=2,
        velocity_range=(-3, 3),
        size=0.15
    )
    return emitter

def create_flame_effect(pos, duration=2):
    """Create a flame/fire effect."""
    emitter = ParticleEmitter(
        position=pos,
        particle_color=color.Color(1, 0.5, 0.1, 0.7),
        emission_rate=20,
        lifetime=1.5,
        velocity_range=(-1, 1),
        size=0.2
    )
    return emitter

def create_sparkle_effect(pos, duration=1.5, color_=color.Color(1, 1, 0.2, 0.8)):
    """Create a sparkle/shine effect."""
    emitter = ParticleEmitter(
        position=pos,
        particle_color=color_,
        emission_rate=30,
        lifetime=1.5,
        velocity_range=(-2, 2),
        size=0.08
    )
    return emitter

def create_glow_aura(pos, color_=color.Color(0.5, 1, 0.5, 0.5), radius=5):
    """Create a glowing aura around an object."""
    # Create glowing sphere entity
    aura = Entity(
        model='sphere',
        position=pos,
        scale=radius,
        color=color_,
        unlit=False
    )
    return aura

def create_explosion_effect(pos, intensity=1):
    """Create an explosion particle effect."""
    emitter = ParticleEmitter(
        position=pos,
        particle_color=color.Color(1, 0.6, 0.2, 0.8),
        emission_rate=50 * intensity,
        lifetime=1,
        velocity_range=(-8, 8),
        size=0.2 * intensity
    )
    return emitter
