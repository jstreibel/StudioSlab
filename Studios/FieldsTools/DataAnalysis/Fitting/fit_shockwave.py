import numpy as np
import matplotlib.pyplot as pyplot
from scipy.ndimage import convolve1d
from mpl_toolkits.mplot3d.axes3d import Axes3D # yes, it is used.
from Utils import utils

a_k_ = 3.51286241725234, 6.92082643286832, 11.0289351677089, 15.7328608401943, 20.9646870297799, 26.6755819971145, 32.8282239007621, 39.3928746473699, 46.3451095048736, 53.6643990106944, 61.3331707944679, 69.3361605324387, 77.6599465623376, 86.2926062475429, 95.2234559390378, 104.442850059323, 113.942023063953, 123.712963182046, 133.748310159998, 144.041271439634
a_k_ = np.asarray(a_k_)

def get_ak(n_curves=len(a_k_)):
    return np.asarray(a_k_[0:n_curves])

def tau_k(xsi, ak):
    return np.sqrt(xsi**2 + 4.0*ak)

class FunctionToMinimize(object):
    def __init__(self, a_k, phi, T, alongT0=False):
        xsi_zero = phi[:,int(phi.shape[1]/2)]
        self.a_k = a_k

        self.T = T

        T_range=np.arange(0, T, T/len(xsi_zero))
        zeros_index = np.where(convolve1d(xsi_zero<0, weights=[-1, 1])==True)[0]
        temp = T_range[zeros_index]
        # If time-distance between consecutive zeroes is smaller than cutoff, discard zero.
        cutoff = 5.e-3
        self.zeros = (temp[abs(temp - np.roll(temp, 1))>cutoff])

        self.T0_v = []
        self.a0_v = []
        self.val_v = []
        self.colors_v = []

        self.n_iter = 0

    def _check_val(self, T_k, zeros):
        ntk = len(T_k)
        nzero = len(zeros)

        evals = []
        for i in range(nzero):
            myeval = np.sum(np.abs(T_k - np.roll(zeros, i)[:ntk]))
            evals.append(myeval)

        argmin = np.argmin(evals)
        #'offset', 'eval'
        return argmin, evals[argmin]

    def initialGuess(self):
        T0 = 0.5
        a0i = (0.5*(self.zeros[0]-T0))**2 / self.a_k[0]

        return T0, a0i

    def __call__(self, params):
        T0, a0 = params
        a_k = self.a_k
        zeros = self.zeros
        T_k = T0 + 2.0*np.sqrt(a0*a_k)

        res = self._check_val(T_k, zeros)[1]

        self.T0_v.append(T0)
        self.a0_v.append(a0)
        self.val_v.append(res)
        if len(self.colors_v) == 0:
            self.colors_v.append('b')
        else:
            self.colors_v.append('r')

        self.n_iter += 1

        return res


class MyOut(object):
    def __init__(self):
        pass

    def write(self, val):
        utils.write(utils.WARNING)
        print(val)
        utils.write(utils.ENDC)

def fit(phi, T, n_curves=len(a_k_), initGuess=None, show_debug_info=False):

    import scipy.optimize as optimize

    func = FunctionToMinimize(a_k=get_ak(n_curves), phi=phi, T=T)
    if initGuess is None:
        initGuess = func.initialGuess()

    T0, a0 = initGuess
    bounds = ((0, T), (0.0001, 20.0))
    method = ('Nelder-Mead', 'Powell', 'CG', 'BFGS', 'Newton-CG', 'L-BFGS-B', 'TNC', \
        'COBYLA', 'SLSQP', 'trust-constr', 'dogleg', 'trust-ncg', 'trust-exact', \
        'trust-krylov')[5]

    import sys
    sys.stderr = MyOut()

    try:
        result = optimize.minimize(func, initGuess, bounds=bounds, method=method)
    except Exception as e:
        utils.colorBold()
        utils.colorFail()
        print("Fitting error. Initial guess (T0,a0) was", T0, a0, "\nMessage: "+utils.ENDC+utils.WARNING, end="")
        print(e)
        utils.colorEndc()
        exit()

    if 1 or not result.success:
        utils.colorBold()
        utils.colorFail()
        print("Fitting error. Initial guess (T0,a0) was", T0, a0, ";"+utils.ENDC+utils.WARNING)
        print(result)
        utils.colorEndc()

    if show_debug_info:
        fig = pyplot.figure(figsize=pyplot.figaspect(1.0))
        ax3d = fig.add_subplot(1, 1, 1, projection='3d')

        ax3d.set_xlabel('$T_0$')
        ax3d.set_ylabel('$a_0$')

        #ax3d.plot_surface(a0_a, T0_a, func(vals))
        ax3d.scatter(func.T0_v, func.a0_v, np.log(func.val_v), c=func.colors_v, marker='^')

        fig.show()
        pyplot.show()

    T0, a0 = result.x

    utils.write(utils.OKBLUE)
    print('Fit result from method', method, 'with', func.n_iter, 'iterations:\n\ta0 =', a0, '\n\tT0 =', T0, utils.ENDC)

    return {'T0': T0, 'a0': a0, 'n_curves': n_curves}
