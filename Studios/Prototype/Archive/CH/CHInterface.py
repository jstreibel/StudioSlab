# Interface pro arquivo gerado pelo Swig.

import CH
import matplotlib.pyplot as plot

N = 0
xVals = []
def Init(x, h, T, A1, A2, A3, B1, B2, B3, acc):
    global N
    global xVals
    N = len(x)
    xVals = []
    
    CHData = CH.doubleArray(N)
    
    for i in range(N):
        CHData[i] = x[i]
        xVals.append(i*h)        
    
    CH.InitCH(CHData, N, h, T, A1, A2, A3, B1, B2, B3, acc)

def GetX():
    darr = CH.doubleArray_frompointer(CH.X())

    global N
    x = []
    for i in range(N):
        x.append(darr[i])
        
    return x

def Step(nSteps, timeStep):
    CH.stepCH(nSteps, timeStep)
    Plot()
    

def SetT(T):
    CH.setT(T)
    
def Plot(reset = False):
    ylim = plot.ylim()
    xlim = plot.xlim()
    plot.cla()
    global xVals
    plot.grid(True)
    plot.plot(xVals, GetX())
    
    if not reset:
        plot.ylim(ylim)
        plot.xlim(xlim)
    else:
        plot.ylim(-0.1, 1.1)
        
plot.interactive(True)
plot.ylim(-0.1, 1.1)
