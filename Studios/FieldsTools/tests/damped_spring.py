import numpy as np
from matplotlib.pyplot import plot

t = np.arange(0, 1, 0.01)
lam = 10 # (qto maior mais rápido amortece)
B = 0.1

for B in np.arange(0, 0.1, 0.005):
    plot(t, np.exp(-lam*t)*(B+t))