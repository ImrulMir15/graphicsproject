from ursina import *

class PauseMenu:
    def __init__(self, on_resume, on_quit):
        self.entities = []
        self.on_resume = on_resume
        self.on_quit   = on_quit

        bg = Entity(parent=camera.ui, model='quad', scale=(0.7,0.7),
                    color=color.Color(0.02, 0, 0.1, 0.82), z=0)
        title = Text('PAUSED', parent=camera.ui, scale=4,
                     position=(0,0.22), origin=(0,0),
                     color=color.Color(0.31, 0.86, 1, 1))
        btn_r = Button(text='Resume',  parent=camera.ui,
                       position=(0,0.06), scale=(0.28,0.08),
                       color=color.Color(0.08, 0.63, 0.31, 1), on_click=self.resume)
        btn_q = Button(text='Quit',    parent=camera.ui,
                       position=(0,-0.08), scale=(0.28,0.08),
                       color=color.Color(0.71, 0.12, 0.12, 1), on_click=self.quit)
        self.entities = [bg, title, btn_r, btn_q]

    def resume(self):
        self.on_resume()
        self.destroy()

    def quit(self):
        self.on_quit()

    def destroy(self):
        for e in self.entities:
            destroy(e)
        self.entities.clear()
