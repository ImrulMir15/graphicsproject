"""Procedural mesh generators for Ursina (cylinder, cone)."""
from math import cos, sin, pi
from ursina import Mesh

_CYLINDER = None
_CONE = None

def cylinder(segments=16):
    global _CYLINDER
    if _CYLINDER is not None:
        return _CYLINDER
    verts, tris = [], []
    for i in range(segments):
        a0 = i/segments * 2*pi
        a1 = (i+1)/segments * 2*pi
        x0,z0 = cos(a0)*.5, sin(a0)*.5
        x1,z1 = cos(a1)*.5, sin(a1)*.5
        # Side quad
        b = len(verts)
        verts += [(x0,-.5,z0),(x0,.5,z0),(x1,.5,z1),(x1,-.5,z1)]
        tris  += [b,b+1,b+2, b,b+2,b+3]
        # Top cap
        b2 = len(verts)
        verts += [(0,.5,0),(x0,.5,z0),(x1,.5,z1)]
        tris  += [b2,b2+1,b2+2]
        # Bottom cap
        b3 = len(verts)
        verts += [(0,-.5,0),(x1,-.5,z1),(x0,-.5,z0)]
        tris  += [b3,b3+1,b3+2]
    _CYLINDER = Mesh(vertices=verts, triangles=tris, mode='triangle')
    return _CYLINDER

def cone(segments=16):
    global _CONE
    if _CONE is not None:
        return _CONE
    verts, tris = [], []
    for i in range(segments):
        a0 = i/segments * 2*pi
        a1 = (i+1)/segments * 2*pi
        x0,z0 = cos(a0)*.5, sin(a0)*.5
        x1,z1 = cos(a1)*.5, sin(a1)*.5
        # Side triangle
        b = len(verts)
        verts += [(x0,-.5,z0),(x1,-.5,z1),(0,.5,0)]
        tris  += [b,b+1,b+2]
        # Bottom cap
        b2 = len(verts)
        verts += [(0,-.5,0),(x1,-.5,z1),(x0,-.5,z0)]
        tris  += [b2,b2+1,b2+2]
    _CONE = Mesh(vertices=verts, triangles=tris, mode='triangle')
    return _CONE
