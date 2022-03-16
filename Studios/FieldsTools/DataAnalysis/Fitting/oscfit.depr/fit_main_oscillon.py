#!/usr/bin/env python
# -*- coding: utf-8 -*-

from oscfit import oscfit
from numpy import sqrt, asarray, arange, linspace
import scipy.optimize as optimize
from open_sim import SimData

fmt = '{:.3f}'.format

def getUsedParams( ):
    # Isso fica hard-coded mesmo, por causa da funcao _fitFunc que nao eh muito maleavel.
    return 'v', 'V', 'phase'

class FitSymmetricSim(object):
    """Symmetric oscillon collision simulation fitting."""
    def __init__(self, simData, params, bounds):
        super(FitSymmetricSim, self).__init__()

        if not type(simData) is SimData:
            raise "Sim data expected"

        oscfit.Init()
        self._simData = simData
        self._params = params
        self._bounds = bounds

    def _fitFunc(self, paramVals):
        T=self.T

        v, V, phase = paramVals
        d, l = self._simData.getItems(('d', 'l'))

        return oscfit.getDif(T, v, V, phase, d, l, True)

    def _fitFunc_multiT(self, paramVals):
        err = 0
        for T in self.Tlist:
            self.T = T
            suppMin, suppMax = self.getMainOscillon_xSupport(T)

            phi0 = self._simData.getInstant(T, suppMin, suppMax)
            oscfit.setMetaData(suppMin, suppMax)
            oscfit.setData(phi0)

            err += self._fitFunc(paramVals)

        return err

    def getMainOscillon_xSupport(self, T):
        V, d = self._simData['V'], self._simData['d']

        invGamma = sqrt(1-V*V)
        oscWidth = invGamma
        # 0.5*oscWidth eh pra ficar um pouquinho mais largo.
        xmax = -0.5*d + 0.5*oscWidth + T*V
        xmin = xmax - oscWidth - 1.0*oscWidth

        return xmin, xmax

    def getFitParams(self):
        return self._params 

    def fitMainOscillon(self, T, multiT=True, multiT_param={'n': 3, 'ndT': 10}):
        fitFunc = None

        if multiT:
            n = 10
            dT = self._simData['T'] / self._simData['outresT']
            if 'n' in multiT_param:
                n = multiT_param['n']
            if 'ndT' in multiT_param:
                dT*=multiT_param['ndT']

            self.Tlist = arange(T, T + n*dT, dT)

            fitFunc = self._fitFunc_multiT

        else:
            self.T = T
            Lmin, Lmax = self.getMainOscillon_xSupport(T)

            phi0 = self._simData.getInstant(T, Lmin, Lmax)
            oscfit.setMetaData(Lmin, Lmax)
            oscfit.setData(phi0)

            fitFunc = self._fitFunc

        initGuess = asarray(self._simData.getItems(self._params))
        result = optimize.minimize(fitFunc, initGuess, bounds=self._bounds)

        return result


    def getAnalyticOscillo(self, T, v, V, phase, d, l, invertAxisMirror):
        osc = oscfit.doubleArray_frompointer(oscfit.getOsc(T, v, V, phase, d, l, invertAxisMirror))
        N = oscfit.getN()
        oscl = [osc[i] for i in range(N)]
        return asarray(oscl)


def FitAndOutputMainOscillonParams(args):
    from glob import glob
    files = glob(args.files)
    files.sort()
    T = float(args.T)

    with open('fitOutput.fit', 'w') as output:
        params = getUsedParams() 
        #pbe -> v, V, phase
        pbe = (1.0, 0.05, 0.3) # params_bounds_eps ==> o quanto os parametros podem variar pra cima ou pra baixo

        output.write('T'+' ')
        for p in params:
            output.write(p + '_ ')
        for p in params:
            output.write(p + ' ')

        for filename in files:
            print('Now doing', filename)

            sim = SimData(filename)
            bounds = []
            for i in range(len(params)):
                val = sim[params[i]]
                bounds.append((val-pbe[i], val+pbe[i]))
            fit = FitSymmetricSim(sim, params, bounds)
            suppMin, suppMax = fit.getMainOscillon_xSupport(T)

            res = fit.fitMainOscillon(T, multiT=False)

            print('   Result is', str(dict(zip(params,res.x))).replace(':', ' =').replace(', ', '   '))

            output.write('\n')
            output.write(str(T) + ' ')
            for val in res.x:
                output.write(str(val) + ' ')

            for key in params:
                output.write(str(sim[key]) + ' ')

            if args.showFit:
                N = oscfit.getN()
                xi = linspace(suppMin, suppMax, N)
                x = linspace(-sim['L']*.5, sim['L']*.5, int(sim['outresX'])) 
                v_, V_, phase_ = res.x
                v, V, phase, d = sim.getItems(('v', 'V', 'phase', 'd'))
                l = 1.0
                d_ = d
                l_ = l
                oscFitt =  fit.getAnalyticOscillon(T, v_, V_, phase_, d_, l_, True)
                oscFree = fit.getAnalyticOscillon(T, v,  V, phase, d, l, True)

                from matplotlib import pyplot
                pyplot.xlim((suppMin, suppMax))
                pyplot.plot(x, sim.getAtTime(T), linewidth=2.0, label='scattered')
                pyplot.plot(xi, oscFitt, linewidth=0.8, linestyle='--', label='fit')
                pyplot.plot(xi, oscFree, 'r', linewidth=0.8, linestyle='-.', label='free')
                pyplot.legend()
                pyplot.show()


def View(args):
    from sys import exit
    from glob import glob
    filename = glob(args.files)

    if len(filename) != 1:
        print("One file, please. " + str(len(filename)) + " were given.")
        exit(1)
    if filename[0][-4:] != '.fit':
        print("Fit file, please. " + filename[0] + " not recognized.")
        exit(1)

    with open(filename[0]) as file:
        param_keys = [val for val in file.readline().rstrip().split(' ')]
        fits = [[float(v) for v in line.rstrip().split(' ')] for line in file]

    dict = {}
    for key in param_keys:
        index = param_keys.index(key)
        dict[key] = [v[index] for v in fits]

    xParam = args.x
    yParam = args.y.replace(' ', '').split(',')

    from matplotlib import pyplot
    pyplot.xlabel(xParam)
    for yp in yParam:
        if not yp in dict:
            print(yp, 'not in param list.')
            continue
        pyplot.scatter(dict[xParam], dict[yp], marker='x', label='$'+yp.replace('_', ' (out)')+'$')
    pyplot.legend()
    pyplot.show()


if __name__ == '__main__':
    import argparse
    parser = argparse.ArgumentParser()
    parser.add_argument('--files', help="Filename(s). Default is all files in folder.", \
                        type=str, default='./*.osc')
    parser.add_argument('--T', help="Time of sim to fit.", type=float, default=2)
    parser.add_argument('--showFit', '-s', help='Show fit result and comparissons.', \
                        action='store_true')

    parser.add_argument('--view', '-v', help='View oscillon fit result.', \
                        action='store_true')
    parser.add_argument('--y', '-y', help='y-axis param.', type=str, default='phase_')
    parser.add_argument('--x', '-x', help='x-axis param.', type=str, default='phase')

    args = parser.parse_args()

    if args.view is True:
        View(args)
    else:
        FitAndOutputMainOscillonParams(args)
