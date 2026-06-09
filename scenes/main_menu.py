from ursina import *

class MainMenu:
    def __init__(self, on_play):
        self.on_play = on_play
        self.gender  = 'male'
        self.entities = []

        # Dark background panel - covers full screen
        bg = Entity(parent=camera.ui, model='quad', scale=(3, 2),
                    color=color.Color(0.02, 0.0, 0.08, 0.95), z=1)
        self.entities.append(bg)

        # Title
        t1 = Text('SPACE PARK', parent=camera.ui, scale=4.5,
                  position=(0, 0.38), origin=(0,0),
                  color=color.Color(0.3, 0.86, 1.0, 1))
        t2 = Text('FUN WORLD', parent=camera.ui, scale=3.2,
                  position=(0, 0.27), origin=(0,0),
                  color=color.Color(1, 0.7, 0.2, 1))
        t3 = Text('A Disney-style space theme park experience',
                  parent=camera.ui, scale=1.1, position=(0, 0.18),
                  origin=(0,0), color=color.Color(0.7, 0.7, 0.86, 0.7))
        self.entities += [t1, t2, t3]

        # Character select label
        lbl = Text('Choose your astronaut:', parent=camera.ui,
                   position=(0, 0.07), origin=(0,0),
                   color=color.Color(0.8, 0.8, 1.0, 1), scale=1.3)
        self.entities.append(lbl)

        # Gender buttons
        self.btn_male = Button(text='Male', parent=camera.ui,
                               position=(-0.18, -0.04), scale=(0.2, 0.07),
                               color=color.Color(0.12, 0.4, 0.8, 1),
                               highlight_color=color.Color(0.24, 0.6, 1.0, 1),
                               text_color=color.white,
                               on_click=lambda: self._select('male'))
        self.btn_female = Button(text='Female', parent=camera.ui,
                                 position=(0.18, -0.04), scale=(0.2, 0.07),
                                 color=color.Color(0.5, 0.16, 0.7, 1),
                                 highlight_color=color.Color(0.78, 0.3, 1.0, 1),
                                 text_color=color.white,
                                 on_click=lambda: self._select('female'))
        self.entities += [self.btn_male, self.btn_female]

        # Name label + input
        nlbl = Text('Your Name:', parent=camera.ui, position=(-0.24, -0.15),
                    origin=(0,0), color=color.Color(0.8, 0.8, 1.0, 1), scale=1.2)
        self.name_field = InputField(parent=camera.ui, position=(0.1, -0.15),
                                     scale=(0.35, 0.06))
        self.name_field.text = 'Explorer'
        self.entities += [nlbl, self.name_field]

        # Play button
        play = Button(text='ENTER THE PARK', parent=camera.ui,
                      position=(0, -0.28), scale=(0.38, 0.09),
                      color=color.Color(0.08, 0.7, 0.3, 1),
                      highlight_color=color.Color(0.16, 0.86, 0.4, 1),
                      text_color=color.white,
                      on_click=self._play)
        self.entities.append(play)

        # Floating stars decoration
        self.stars = []
        import random
        for _ in range(40):
            s = Entity(parent=camera.ui, model='quad',
                       scale=random.uniform(0.003, 0.008),
                       position=(random.uniform(-0.9, 0.9),
                                 random.uniform(-0.5, 0.5), 0.5),
                       color=color.Color(1, 1, 1, random.uniform(0.4, 0.85)))
            self.stars.append(s)
        self.entities += self.stars

        self._select('male')

    def _select(self, g):
        self.gender = g
        self.btn_male.color   = color.Color(0.24, 0.6, 1.0, 1) if g == 'male' else color.Color(0.12, 0.3, 0.6, 1)
        self.btn_female.color = color.Color(0.78, 0.3, 1.0, 1) if g == 'female' else color.Color(0.4, 0.12, 0.6, 1)

    def _play(self):
        name = self.name_field.text.strip() or 'Explorer'
        self.destroy_menu()
        self.on_play(name, self.gender)

    def destroy_menu(self):
        for e in self.entities:
            destroy(e)
        self.entities.clear()
