import matplotlib.pyplot as pyplot
import sympy as sp
import numpy as np

sp.init_printing()

a0=2.23
N = 20
xk = []
ak = [a0]

def Ak(k):
    val = 1.0
    for i in range(1, k): # eh ate k-1 mesmo
        product = 1.0
        for j in range(1, i+1):
            product *= xk[k-j-1]
        val += (-1.0)**i / product

    return val


def F(x, A_k=1):
    return 1 - x + 2 * A_k * sp.log(x)


def a(k, a0=1.0):
    val = a0
    for i in range(1, k+1):
        val *= xk[i-1]
    return val


x, tau = sp.symbols("x tau")

#print("xk: ", end='')
for i in range(1, N+1):
    A = Ak(i)
    xk_sols = sp.solve(F(x, A), x) # era pra ser um Lambert W aqui mas ok.
    xk.append(xk_sols[1])  # a primeira solucao eh sempre igual (x=1), usamos a segunda somente
    a_ = a(i, a0)
    ak.append(a_)
    #print(sp.N(xk_sols[1]), ", ", end='')


def xsi(tau, a_k):
    return sp.sqrt(tau**2-4*a_k)


def plotxkak():
    pyplot.figure(1)

    pyplot.subplot(211)
    pyplot.plot(xk, 'o')

    pyplot.subplot(212)
    pyplot.plot(ak, 'o')

    pyplot.show()


xsi_k = []
#for i in range(N):
#    xsi_k.append(xsi(tau, ak[i]))
if 0:
    sp.plot(xsi_k[0], xsi_k[1], xsi_k[2], xsi_k[3], xsi_k[4], xsi_k[5], xsi_k[6], xsi_k[7], xsi_k[8], xsi_k[9], 
            xsi_k[10], xsi_k[11], xsi_k[12], xsi_k[13], xsi_k[14], xsi_k[15], xsi_k[16], xsi_k[17], xsi_k[18], xsi_k[19], (tau, 0, 20))

print("\n\nak: ", ak)

def zk(k):
    if k == 0:
        return 2*ak[0]
    
    return 2*ak[k]-zk(k-1)
def W(z, k):
    if k == -1:
        return z + ak[0]
    
    a_k = ak[k]
    z_k = zk(k)
    return (-1)**k * (z + a_k + z_k*np.log(np.abs(z/a_k)))
            
#def phi(xi, tau):
#   z = 0.25*(xi*xi-tau*tau)
#    
#    return np.heaviside(-z) * 






# O HAI