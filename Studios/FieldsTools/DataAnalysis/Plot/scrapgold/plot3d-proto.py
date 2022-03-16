import open_sim

from matplotlib import pyplot
from mpl_toolkits.mplot3d import Axes3D

from numpy import linspace, full, outer, ones


data = open_sim.SimData("pert--eps=1.0000-l=1.0000-(r=0.1000-N=8192).osc")

fig = pyplot.figure()
ax = pyplot.axes(projection='3d')

N = int(data['outresX'])
tmax = data['T']
xmax = data['L']

if 0:
    x = linspace(0., xmax, N)
    for t in linspace(0., tmax, 50, endpoint=False):
        cphi = data.getAtTime(t)
        t_ = full(N, t)

        ax.plot(x,t_,cphi,color='k', linewidth=0.5)

else:
    n = int(data['outresT'])
    x = outer(linspace(0, xmax, N), ones(n)).T
    t = outer(linspace(0, tmax, n), ones(N))
    phi = data.getPhi()

    #ax.plot_wireframe(x, t, phi, rstride=100, cstride=100, linewidth=0.4, color='k')
    stride = 50
    ax.plot_surface(x, t, phi, cmap='viridis', edgecolor='k', linewidth=0.4, rstride=stride, cstride=stride)
    
    r = 0.50
    ax.set_zlim(-r*.5,r*.5)

    ax.set_xlabel("$x$")
    ax.set_ylabel("$t$")
    ax.set_zlabel("$\\varphi$")

pyplot.show()
