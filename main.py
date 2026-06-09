"""Space Park Fun World - Main Entry Point"""
from ursina import *
from ursina.lights import DirectionalLight, AmbientLight
from player.player import SpacePlayer
from zones.entry_plaza import EntryPlaza
from zones.galaxy_games import GalaxyGamesZone
from zones.cosmic_rides import CosmicRidesZone
from systems.day_night import DayNightSystem
from systems.score_manager import ScoreManager
from systems.npc_manager import NPCManager
from math import sin, cos, radians
import random, importlib, os, sys

sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))

app = Ursina(title='Space Park Fun World', vsync=True, development_mode=False)
camera.fov = 80

# ═══════════════════════════════════════════════════════════════════════════════
# GLOBAL STATE
# ═══════════════════════════════════════════════════════════════════════════════

class GameState:
    def __init__(self):
        self.player_name = 'Explorer'
        self.player_gender = 'male'
        self.game_mode = 'menu'  # menu | park | game
        self.player = None
        self.zones = {}
        self.day_night = None
        self.npc_manager = None
        self.score_manager = ScoreManager()
        self.menu_entities = []
        self.park_entities = []
        self.sky_entity = None
        self.lights = {'sun': None, 'sun2': None, 'amb': None}

G = GameState()

# ═══════════════════════════════════════════════════════════════════════════════
# UTILITIES
# ═══════════════════════════════════════════════════════════════════════════════

def C(r, g, b, a=1):
    """Convert 0-255 RGB to normalized Ursina color."""
    return color.Color(r/255, g/255, b/255, a)

# ═══════════════════════════════════════════════════════════════════════════════
# MENU SYSTEM
# ═══════════════════════════════════════════════════════════════════════════════

def create_main_menu():
    """Display main menu with character selection."""
    me = G.menu_entities

    # Background - high z-order to stay on top
    me.append(Entity(parent=camera.ui, model='quad', scale=(3, 2),
                     color=color.Color(0.02, 0, 0.08, 0.99), z=100))

    # Title
    me.append(Text('SPACE PARK', parent=camera.ui, scale=3.5,
                   position=(0, 0.35), origin=(0, 0),
                   color=C(75, 225, 255)))
    me.append(Text('FUN WORLD', parent=camera.ui, scale=2.8,
                   position=(0, 0.20), origin=(0, 0),
                   color=C(255, 215, 50)))
    me.append(Text('Disney-style space theme park', parent=camera.ui,
                   scale=0.9, position=(0, 0.08), origin=(0, 0),
                   color=color.Color(0.8, 0.8, 0.95, 0.8)))

    # Character selection
    me.append(Text('Choose your astronaut:', parent=camera.ui,
                   position=(0, -0.02), origin=(0, 0),
                   color=color.Color(0.9, 0.9, 1, 1), scale=1.0))

    def select_male():
        G.player_gender = 'male'
        btn_male.color = C(75, 165, 255)
        btn_female.color = C(100, 25, 145)

    def select_female():
        G.player_gender = 'female'
        btn_male.color = C(30, 80, 180)
        btn_female.color = C(200, 60, 255)

    btn_male = Button(text='Male', parent=camera.ui, position=(-0.18, -0.10),
                      scale=(0.20, 0.06), color=C(75, 165, 255),
                      text_color=color.white, on_click=select_male)
    btn_female = Button(text='Female', parent=camera.ui, position=(0.18, -0.10),
                        scale=(0.20, 0.06), color=C(100, 25, 145),
                        text_color=color.white, on_click=select_female)
    me.extend([btn_male, btn_female])

    # Name input
    me.append(Text('Name:', parent=camera.ui, position=(-0.23, -0.22),
                   origin=(0, 0), color=color.Color(0.9, 0.9, 1, 1), scale=1.0))

    name_field = InputField(parent=camera.ui, position=(0.08, -0.22),
                            scale=(0.35, 0.05))
    name_field.text = 'Explorer'
    me.append(name_field)

    # Animated stars background
    for _ in range(50):
        me.append(Entity(parent=camera.ui, model='quad',
                        scale=random.uniform(0.003, 0.01),
                        position=(random.uniform(-0.9, 0.9),
                                 random.uniform(-0.5, 0.5), 0.5),
                        color=color.Color(1, 1, 1, random.uniform(0.3, 0.9))))

    def start_game():
        G.player_name = name_field.text.strip() or 'Explorer'
        for e in G.menu_entities:
            try:
                destroy(e)
            except:
                pass
        G.menu_entities.clear()
        enter_park()

    # Buttons
    me.append(Button(text='ENTER THE PARK', parent=camera.ui,
                     position=(0, -0.35), scale=(0.40, 0.08),
                     color=C(20, 185, 80), text_color=color.white,
                     on_click=start_game))
    me.append(Button(text='QUIT', parent=camera.ui,
                     position=(0, -0.45), scale=(0.20, 0.06),
                     color=C(160, 30, 30), text_color=color.white,
                     on_click=application.quit))

# ═══════════════════════════════════════════════════════════════════════════════
# PARK BUILDING & ZONES
# ═══════════════════════════════════════════════════════════════════════════════

def build_park():
    """Initialize park zones, lighting, and player."""
    window.color = C(85, 55, 160)

    # Setup lighting
    G.lights['sun'] = DirectionalLight(y=25, rotation=(40, -55, 0))
    G.lights['sun'].color = color.Color(1.0, 0.95, 0.82, 1)

    G.lights['sun2'] = DirectionalLight(y=12, rotation=(-25, 125, 0))
    G.lights['sun2'].color = color.Color(0.48, 0.45, 0.60, 1)

    G.lights['amb'] = AmbientLight()
    G.lights['amb'].color = color.Color(0.62, 0.58, 0.72, 1)

    # Create sky entity (for day/night transitions) - parent to camera so it follows
    G.sky_entity = Entity(model='sphere', scale=300, position=(0, 0, 0),
                         color=C(85, 55, 160), unlit=True, parent=camera)

    # Ground base
    ground = Entity(model='cube', position=(0, 0, 65), scale=(300, 0.5, 300),
                   color=C(55, 40, 85))
    G.park_entities.append(ground)

    # Ground grid tiles
    for xi in range(-20, 21, 4):
        for zi in range(0, 38, 4):
            tc = C(62, 46, 98) if (xi + zi) % 2 == 0 else C(50, 36, 80)
            tile = Entity(model='cube', position=(xi*3.5, 0.27, zi*3.5+2),
                         scale=(3.3, 0.04, 3.3), color=tc)
            G.park_entities.append(tile)

    # Create zone instances
    G.zones['entry'] = EntryPlaza()
    G.zones['games'] = GalaxyGamesZone(on_enter_game=launch_game)
    G.zones['rides'] = CosmicRidesZone()

    # Create day/night system
    G.day_night = DayNightSystem(G.sky_entity)
    G.day_night.lights = G.lights  # Pass light references for smooth transitions

    # Create NPC manager
    G.npc_manager = NPCManager()

def enter_park():
    """Initialize park and create player."""
    G.game_mode = 'park'
    build_park()

    # Create player
    G.player = SpacePlayer(gender=G.player_gender, on_interact=None)
    G.player.position = (0, 2, 0)

    # Setup HUD
    Text('WASD:Move  Mouse:Look  E:Interact  N:Day/Night  C:Camera  Space:Jump',
         parent=camera.ui, origin=(0, -0.5), position=(0, -0.47),
         color=color.Color(0.7, 0.7, 0.85, 0.55), scale=0.82)

def cleanup_park():
    """Destroy all park entities and systems."""
    # Destroy zone entities
    for zone_obj in G.zones.values():
        if hasattr(zone_obj, 'destroy'):
            zone_obj.destroy()
    G.zones.clear()

    # Destroy NPCs
    if G.npc_manager:
        G.npc_manager.destroy()
        G.npc_manager = None

    # Destroy player
    if G.player:
        try:
            destroy(G.player)
        except:
            pass
        G.player = None

    # Destroy day/night system
    if G.day_night:
        G.day_night.destroy()
        G.day_night = None

    # Destroy sky entity
    if G.sky_entity:
        try:
            destroy(G.sky_entity)
        except:
            pass
        G.sky_entity = None

    # Destroy ground and other park entities
    for e in G.park_entities:
        try:
            destroy(e)
        except:
            pass
    G.park_entities.clear()

# ═══════════════════════════════════════════════════════════════════════════════
# MINIGAME SYSTEM
# ═══════════════════════════════════════════════════════════════════════════════

def launch_game(game_key):
    """Dynamically load and launch a minigame."""
    if G.game_mode != 'park':
        return

    G.game_mode = 'game'
    G.player.enabled = False
    mouse.locked = False
    mouse.visible = True
    G.player.hide_ui()

    # Dynamic import
    mod_name = {
        'asteroid_shooting': 'minigames.asteroid_shooting',
        'whack_a_mole': 'minigames.whack_a_mole',
        'space_maze': 'minigames.space_maze',
        'rocket_race': 'minigames.rocket_race',
    }[game_key]

    class_name = {
        'asteroid_shooting': 'AsteroidShooting',
        'whack_a_mole': 'WhackAMole',
        'space_maze': 'SpaceMaze',
        'rocket_race': 'RocketRace',
    }[game_key]

    mod = importlib.import_module(mod_name)
    game_class = getattr(mod, class_name)
    game_class(player_name=G.player_name, on_complete=on_game_complete)

def on_game_complete(game_key, score):
    """Handle minigame completion and score display."""
    G.game_mode = 'park'
    top_scores = G.score_manager.add_score(game_key, G.player_name, int(score))

    G.player.enabled = True
    mouse.locked = False
    mouse.visible = True
    G.player.show_ui()

    # Score popup
    popup = []

    bg = Entity(parent=camera.ui, model='quad', scale=(0.78, 0.68),
               color=color.Color(0.02, 0, 0.1, 0.94), z=0)
    popup.append(bg)

    def add_text(txt, y, sc, col=color.white):
        t = Text(txt, parent=camera.ui, scale=sc, position=(0, y),
                origin=(0, 0), color=col)
        popup.append(t)

    add_text('GAME OVER!', 0.25, 3.5, C(75, 225, 255))
    add_text(f'Score: {score}', 0.14, 2.5, C(255, 215, 50))
    add_text('--- TOP 5 ---', 0.04, 1.3, color.Color(0.7, 0.7, 0.9, 0.8))

    for i, entry in enumerate(top_scores):
        add_text(f"#{i+1} {entry['name']} - {entry['score']}", -0.05-i*0.07, 1.2,
                C(75, 225, 120))

    def close_popup():
        for x in popup:
            try:
                destroy(x)
            except:
                pass

    popup.append(Button(text='Return to Park', parent=camera.ui,
                       position=(0, -0.30), scale=(0.35, 0.08),
                       color=C(20, 165, 75), text_color=color.white,
                       on_click=close_popup))

# ═══════════════════════════════════════════════════════════════════════════════
# MAIN LOOP
# ═══════════════════════════════════════════════════════════════════════════════

def update():
    """Main game loop update."""
    if G.game_mode == 'park' and G.player:
        # Update day/night system
        if G.day_night:
            G.day_night.update()

        # Update NPCs
        if G.npc_manager:
            G.npc_manager.update()

        # Update zone animations and particle effects
        for zone_obj in G.zones.values():
            if hasattr(zone_obj, 'update'):
                zone_obj.update()

        # Check kiosk proximity
        if hasattr(G.zones.get('games'), 'check_proximity'):
            G.zones['games'].check_proximity(G.player.position, G.player)

def input(key):
    """Main input handler."""
    if key == 'n' and G.game_mode == 'park' and G.day_night:
        G.day_night.toggle()
    elif key == 'e' and G.game_mode == 'park' and G.player and G.player.near_kiosk:
        game_key = G.zones['games'].get_game_key(G.player.near_kiosk)
        if game_key:
            launch_game(game_key)
    elif key == 'escape':
        application.quit()

# ═══════════════════════════════════════════════════════════════════════════════
# ENTRY POINT
# ═══════════════════════════════════════════════════════════════════════════════

create_main_menu()
app.run()
