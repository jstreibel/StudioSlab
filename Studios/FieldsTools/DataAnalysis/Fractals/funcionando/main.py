#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 11 19:34:03 2018

@author: joao
"""

from matplotlib import pyplot
from scipy.interpolate import interp1d
from numpy import arange, linspace, asarray, log, log10, gradient

import primitives
import open_sim

filename = "/home/joao/Documents/result/data/workdir/SYMMETRIC/V=0.9-scatter-fractal/sym-v=0.0000-V=0.9000-ph=0.2000-d=0.0000-(r=0.1000-N=1048576).osc"

sim = open_sim.SimData(filename)

N = sim["outresX"]

subplot = 420

x = sim.getXDiscrete()
phit = sim.getAtTime(4)
pyplot.subplot(subplot+1)
pyplot.plot(x, phit)

phiInterp = interp1d(x, phit)

y = x*x
yInterp = interp1d(x, y)
pyplot.subplot(subplot+2)
pyplot.plot(x, y)

xmin, xmax = -2., 2.
s0 = 2.
sf = N
ds = 10.
dx0 = (xmax-xmin)/s0

for curve in (phiInterp, yInterp):
    a0 = primitives.calculateAreaUnder(dx0*s0, curve(arange(xmin, xmax, dx0/s0)))
    s_arr = [s0/s0]
    a_arr = [a0/a0]
    
    for s in arange(s0+ds, sf, ds):
        x_interval = arange(xmin, xmax, dx0/s)
        phi = curve(x_interval)
           
        dx = dx0*s
        #l = primitives.calculateLength(dx, phi)
        a = primitives.calculateAreaUnder(dx, phi)
        
        #pyplot.plot(x_interval, phi)
        
        s_arr.append(s/s0)
        a_arr.append(a/a0)
    
    s_arr = asarray(s_arr)
    a_arr = asarray(a_arr)
    
    pyplot.subplot(subplot+3)
    pyplot.plot(s_arr, a_arr)
    pyplot.grid(True)
    pyplot.subplot(subplot+5)
    pyplot.loglog(s_arr, a_arr)
    pyplot.grid(True)
    pyplot.subplot(subplot+7)
    pyplot.plot(s_arr, gradient(gradient(a_arr, ds), ds))
    pyplot.grid(True)
    
    subplot += 1

pyplot.show()