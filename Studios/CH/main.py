# Arquivo principal pra rodar a CH

import CHInterface as CH
import math

def InitVals(N):
    x = []
    # x[i] = 0.1 + 0.05*cos(3.14159*(sqrt(5.0)+1)*i);
    for i in range(N):
        #val = 0.1 + 0.01*math.cos(1.0*i) + 0.008*math.cos(math.pi*(math.sqrt(5.0) + 1.0)*i)
        val = 0.1 + 1.0e-2*math.cos(3.14159*(math.sqrt(5.0)+1)*i);
        x.append(val)
        
    return x

def Reset(resetPlot=True):
    CH.Init(InitVals(N), h, T, A1, A2, A3, B1, B2, B3, acc)
    CH.Plot(reset=resetPlot)

def Area():
    xComp = CH.GetX()
    area = 0.0
    for x in xComp:
        area += x
    
    area *= h
    return area
    
def Media(N):
    xComp = CH.GetX()
    total = 0.0
    for x in xComp:
        total += x
    
    media = total / N
    print(media)
    
def Step(nSteps, dt):
    A1 = Area()
    CH.Step(nSteps, dt)
    A2 = Area()
    print("Area =", A2)
    print("dArea =", A2-A1)
    

acc = 1
T = 900
#l = 400*10.0e-9 # largura
l=1.0
N = 1000 # numero de sitios
h = l/float(N) #largura do sitio

# Liu:
AL = 37100.0, 2869.0, 3251.0
BL = -5.194, 0, 0

# Palumbo:
AP = 44537.0, -7489.0, 4390.0
BP = -10.095, 4.161, 0

# Turchanin:
AT = 34600, -6410, 4390
BT = -4, 3.7, 0

A1, A2, A3 = AT
B1, B2, B3 = BT

CH.Init(InitVals(N), h, T, A1, A2, A3, B1, B2, B3, acc)
CH.Plot(reset = True)
Step(0, 1.0e-25)
