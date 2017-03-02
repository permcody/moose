#!/usr/bin/env python
import chigger

box0 = chigger.geometric.CubeSource(center=[0.5,0.5,0.5], lengths=[1,1,1], color=[0.5,0.5,0.5], opacity=0.5, edges=True, edge_color=[1,1,1])
box1 = chigger.geometric.CubeSource(center=[0.25,0.25,0.25], lengths=[3,2,1], color=[1,0.5,0.5])

cubes = chigger.base.ChiggerResult(box0, box1)

window = chigger.RenderWindow(cubes, size=[300,300], test=True)
window.write('cube_source.png')
window.start()
