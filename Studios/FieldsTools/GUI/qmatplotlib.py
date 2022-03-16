from PyQt5 import QtWidgets

from matplotlib.backends.backend_qt5agg import FigureCanvasQTAgg as FigureCanvas
from matplotlib.backends.backend_qt5agg import NavigationToolbar2QT as NavigationToolbar
from matplotlib.figure import Figure
from matplotlib.animation import FuncAnimation
from mpl_toolkits.mplot3d import Axes3D # yes it is used.
import matplotlib.image as mpimage
import numpy as np

cmaps = ['ocean', 'coolwarm', 'PuBu', 'copper', 'bone', 'jet', 'CMRmap',
         'Blues_r', 'magma', 'RdBu', 'jet', 'terrain']
plotColors = ['teal']

class QMatplotlib(QtWidgets.QWidget):
    '''Abstract common purpose class defining some basic matplotlib framework within Qt'''
    def __init__(self, parent=None):
        super(QMatplotlib, self).__init__(parent)

        self.figure = Figure()
        self.canvas = FigureCanvas(self.figure)

        self.layout = QtWidgets.QVBoxLayout()

        self.layout.addWidget(self.canvas,1)
        self.setLayout(self.layout)

        self.ax = self.figure.add_subplot(111)

    def setupToolbar(self):
        '''Shows the main matplotlib navigation toolbar or not. Not yet fully implemented'''
        self.toolbar = NavigationToolbar(self.canvas, self)
        self.layout.addWidget(self.toolbar,0)
        self.setLayout(self.layout)

    def plot(self):
        return NotImplemented

class QMatplotlibHistory(QMatplotlib):
    '''Child from QMatplotlib, this class handles the collision history and gives you utilities
       to reproduce the simulation evolution on the Minkowski diagram, such as functions for you
       to hook onto some slider'''
    def __init__(self, parent=None):
        super(QMatplotlibHistory, self).__init__(parent)
        self.plotColor = plotColors[0]
        self.slider = None

    def plot(self, sim, imgParams=None):
        self.ax.cla()
        if not sim.simExists():
            return

        self.phi = sim.getPhi()
        self.T   = sim.T
        self.nX  = sim._alles['outresX']
        self.nT  = sim._alles['outresT']
        self.animInterval = 1e-2
        self.isPlaying = False
        self.anim = None

        self.historyPlot, = self.ax.plot(self.phi[0], color=self.plotColor)
        self.ax.axes.set_ylim(self.phi.min(), self.phi.max())
        self.ax.axes.set_xlabel(r'$x$', fontsize=16)
        self.ax.axes.set_ylabel(r'$\phi$', fontsize=16)

        self.canvas.draw()

    def update(self, constant):
        #i = int(constant / self.T * self.nT)
        self.historyPlot.set_ydata(self.phi[constant])
        self.slider.setValue(constant)
        self.canvas.draw_idle()

    # Too ugly, breaks encapsulation
    def setSlider(self, sld):
        self.slider = sld

    # Still not working that well
    def setAnimInterval(self, value):
        self.animInterval = .1/value
        if not self.anim:
            self.anim.event_source.interval = self.animInterval

    def _animate(self):
        self.anim = FuncAnimation(self.figure, self.update, frames=int(self.nT),
                                  interval=self.animInterval)
        self.canvas.draw()

    def play(self):
        if not self.isPlaying:
            self.isPlaying = True
            self._animate()
        else:
            self.anim.event_source.stop()
            self.isPlaying = False


class QSimPlot(QMatplotlib):
    '''Child from QMatplotlib, this class is the main attraction of the whole framework. Plots
       existing simulations' Minkowski diagram for a given set of parameters or generate a new
       low-res collision if it doesn't exist, dropping a compact .png image for later quick
       consumption.

       If given simulation has said image in the data tree, load it instead (quite fast indeed!).'''
    def __init__(self, parent=None):
        super(QSimPlot, self).__init__(parent)
        self.setupToolbar()
        self.cmap = cmaps[2]

    def _log_abs(self, phi, epsilon):
        return np.log(np.abs(phi)/epsilon + 1)*np.sign(phi)

    def plot(self, sim, preferImage=True, saveFigure=True, regenImage=False, useLogScale=True, logEps=-6., imgWidth=1024):
        self.ax.cla()
        #self.figure.clf()
        if sim.imgExists() and preferImage and not regenImage:
            self._plotImg(sim.imgFileName())
        else:
            self._plotSim(sim, saveFigure=saveFigure, plotLogScale=useLogScale, epsilon=10.**logEps, imgWidth=imgWidth)

        self.canvas.draw()

    def _plotSim(self, sim, plotLogScale=True, showColorbar=False, saveFigure=True, epsilon=1.e-6, imgWidth=1024):
        phi, L, T = sim.getPhi(), sim.L, sim.T
        fig, ax = self.figure, self.ax

        field = phi
        if plotLogScale:
            field = self._log_abs(field, epsilon)

        extent = -L*0.5, L*0.5, 0, T
        im = ax.imshow(field, extent=extent, cmap=self.cmap, origin='lower', interpolation='quadric')
        ax.tick_params(labelsize=8.0)
        w, h = fig.get_figwidth(), fig.get_figheight()

        if showColorbar:
            self.figure.colorbar(im)

        if saveFigure:
            dpi = 100.13
            basesize = float(imgWidth)/dpi
            fig.set_figwidth(basesize)
            fig.set_figheight(L/T * basesize)
            fig.savefig(sim.imgFileName(), bbox_inches='tight', dpi=dpi, interpolation='quadric')
            fig.set_figwidth(w)
            fig.set_figheight(h)

        # Draw invisible x and t markers to be toggleable later on
        xLimits = self.ax.get_xlim()
        tLimits = self.ax.get_ylim()
        self.axConstXMarker = self.ax.plot(xLimits, [0,0], color='#00000000')
        self.axConstTMarker = self.ax.plot([0,0], tLimits, color='#00000000')

    def _plotImg(self, imgFileName):
        img = mpimage.imread(imgFileName)
        self.ax.set_axis_off()
        self.ax.imshow(img)

    def showHistoryMarker(self):
        try:
            self.axConstTMarker[0].set_color("red")
        except AttributeError:
            pass
        return NotImplemented

    def hideHistoryMarker(self):
        try:
            self.axConstTMarker[0].set_color("#00000000")
            self.axConstXMarker[0].set_color("#00000000")
        except AttributeError:
            pass

class QParamPlot(QMatplotlib):
    '''Child from QMatplotlib, this class aims to plot a messy dot param cloud, where each represents
       a simulation present in the data tree while highlighting the current in-screen simulation.'''
    def __init__(self, parent=None):
        super(QParamPlot, self).__init__(parent)

        ax = self.figure.add_subplot(1, 1, 1, projection='3d')
        ax.set_xlabel('v')
        ax.set_ylabel('V')
        ax.set_zlabel('$\phi$')
        self._ax3d = ax
        self._locationPlot = None

    def readDir(self, rootDir):
        from glob import glob
        from os.path import getsize
        import re

        dirs = glob(rootDir+'*/*/*/*.osc')
        pointsDict = []
        for d in dirs:
            params = [s for s in re.split(r'[\/\-]', d.rstrip('.osc')) if s.find('=') >= 0]

            point = {'size': getsize(d)}
            for p in params:
                key, val = p.split('=')
                point[key] = val

            pointsDict.append(point)

        self._v = []
        self._V = []
        self._ph = []
        self._c = []
        for p in pointsDict:
            self._v.append(float(p['v']))
            self._V.append(float(p['V']))
            self._ph.append(float(p['ph']))
            
            self._c.append(int(p['size']))

        self._v = np.asarray(self._v)
        self._V = np.asarray(self._V)
        self._ph = np.asarray(self._ph)

    def updateLocation(self, v, V, ph):
        if self._locationPlot is not None:
            self._locationPlot.remove()

        self._locationPlot = self._ax3d.scatter(v, V, ph, c='r', marker='v', alpha=1)
        self.canvas.draw()

    def plot(self):
        #self.ax.clear()
        #self.ax.imshow([[random.random() for i in range(20)]  for j in range(20)], cmap='terrain')
        #self.canvas.draw()

        v, V, ph = self._v, self._V, self._ph
        self._ax3d.scatter(v, V, ph, c='b', marker='.', alpha=0.1)

        self.canvas.draw()

        pass

    def addSim(self, v, V, ph):
        self._ax3d.scatter(v, V, ph, c='b', marker='.', alpha=0.1)
        self.canvas.draw()
