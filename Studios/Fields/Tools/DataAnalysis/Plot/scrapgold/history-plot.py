# -*- coding: utf-8 -*-
#!/usr/bin/env python2

import sys, os
import matplotlib.pyplot as plt
#import matplotlib.transforms as trans
from matplotlib.widgets import Slider #, Button, RadioButtons

try:
    filename = sys.argv[1]
except IndexError:
    print("Usage: " + sys.argv[0] + "<wave-eq-1d-RK4 simulation>")
    sys.exit(1)


with open(filename,'r') as simulation:
    results = simulation.readlines()
    phi = [[float(num) for num in line.split()] for line in results[4:]]
    
phi_len = len(phi)
# Find the minimum/maximum values of phi
allTimeMin, allTimeMax = 0,0
for timeStep in phi:
    currentTimeMin = min(timeStep)
    currentTimeMax = max(timeStep)
    if currentTimeMax > allTimeMax: allTimeMax = currentTimeMax
    if currentTimeMin < allTimeMin: allTimeMin = currentTimeMin
    

# Regarding the plot
detailColor = 'teal'

fig, ax = plt.subplots(1,2)
fig.suptitle(os.path.basename(filename), fontsize=16)
plt.subplots_adjust(left=0.25, bottom=0.25)

generalPicture = ax[0]
timeSlice = ax[1]

generalPicture.imshow(phi, cmap='PuBu_r', origin='lower', \
                           interpolation='nearest')
generalPicture.axes.set_xticks([], [])
generalPicture.axes.set_yticks([], [])
marker = generalPicture.plot([0,phi_len],[0,0], color='red')

collision, = timeSlice.plot(phi[0], color=detailColor)
collision.axes.set_ylim([allTimeMin, allTimeMax])
collision.axes.set_xlabel(r'$x$', fontsize=16)
collision.axes.set_ylabel(r'$\phi$', fontsize=16)

time = plt.axes([0.20, 0.15, 0.65, 0.03])

timeSlider = Slider(time, r'$t$', 0, phi_len, valinit=0, valstep=1, \
                    valfmt=u'%d', color=detailColor, alpha=0.2 )

def onUpdate(val):
    collision.set_ydata(phi[int(val)])
    marker[0].set_ydata(val)
    fig.canvas.draw_idle()
timeSlider.on_changed(onUpdate)

# ... and show it
plt.show()