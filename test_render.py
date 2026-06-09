from ursina import *

app = Ursina(development_mode=True)
window.color = color.Color(0.04, 0.02, 0.10, 1)

# Simple light
from ursina.lights import DirectionalLight
dl = DirectionalLight(y=10, rotation=(45, -45, 0))

# Test objects
Entity(model='cube', color=color.red, position=(0, 1, 5), scale=2)
Entity(model='cube', color=color.blue, position=(3, 1, 5), scale=2)
Entity(model='sphere', color=color.green, position=(-3, 2, 5), scale=2)
Entity(model='plane', scale=20, color=color.Color(0.2, 0.15, 0.3, 1))

EditorCamera()

app.run()
