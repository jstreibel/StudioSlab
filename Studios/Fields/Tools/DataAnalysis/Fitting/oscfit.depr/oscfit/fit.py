from .c_analytic import oscfit
from numpy import linspace, asarray
from scipy.optimize import minimize

class FitParams(object):
    def __init__(self, XMin, XMax, TMin, TMax, V, n):
        super(FitParams, self).__init__()
        self.TMin = TMin
        self.TMax = TMax
        self.XMin = XMin
        self.XMax = XMax
        self.VGuess = V
        self.n = n

    def getExtent(self):
        return self.XMin, self.XMax, self.TMin, self.TMax

    def getTRange_n(self, n):
        return linspace(self.TMin, self.TMax, n)

    def getTRange(self):
        return self.getTRange_n(self.n)

    def getXLims(self, T):
        return self.XMin+(T-self.TMin)*self.VGuess, self.XMax+(T-self.TMin)*self.VGuess

    def getPlotPolygon(self):
        from matplotlib.patches import Polygon

        t0, t1 = self.TMin, self.TMax
        xMin0, xMax0 = self.getXLims(t0)
        xMin1, xMax1 = self.getXLims(t1)

        points = asarray([[xMin0, t0],
                          [xMin1, t1],
                          [xMax1, t1],
                          [xMax0, t0]])

        return Polygon(points, facecolor='none', edgecolor='k')



class OscillonFit(object):
    """Good! Good!"""
    def __init__(self, simData, fitParams):
        super(OscillonFit, self).__init__()

        self.simData = simData
        self.fp = fitParams

        oscfit.Init()

        self.result = None

        eps = 5e-2
        lMin, lMax = 1e-3, 1.5
        xMin = -30
        xMax = 30
        self.boundsDict = {
                    'v': (-1+eps,1-eps),
                    'V': (-1+eps, 1-eps),
                    'phase': (0,1),
                    'd': (xMin, xMax),
                    'l': (lMin, lMax)}

    def __del__(self):
        oscfit.Finish()

    def _fitFunc(self, params):
        v, V, phase, d, l = params

        err = 0
        TRange = self.fp.getTRange()
        for T in TRange:
            Xmin, Xmax = self.fp.getXLims(T)

            phi0 = self.simData.getInstant(T, Xmin, Xmax)
            oscfit.setMetaData(Xmin, Xmax)
            oscfit.setData(phi0)

            err += oscfit.getDif(T, v, V, phase, d, l, True)

        return err


    def fit(self):

        # v V phase d l
        initGuess = asarray([0.2, self.fp.VGuess, 0, 0, 0.1])

        bounds = []
        print('Fitting with bounds:')
        for key in ('v', 'V', 'phase', 'd', 'l'):
            val = self.boundsDict[key]
            bounds.append(val)
            print(key + ':', val)

        self.result = minimize(self._fitFunc, initGuess, bounds=bounds)

        return self.result

    def getAnalyticOscillon(self, T, v, V, phase, d, l, vertAxisMirror=True):
        osc = oscfit.doubleArray_frompointer(oscfit.getOsc(T, v, V, phase, d, l, True))
        N = oscfit.getN()
        oscl = [osc[i] for i in range(N)]
        return asarray(oscl)

    def getAnalyticData(self, nSteps):
        if self.result is None:
            self.fit()

        output = []
        v, V, phase, d, l = self.result.x
        TRange = self.fp.getTRange_n(nSteps)
        for T in TRange:
            xLims = self.fp.getXLims(T)
            osc = self.getAnalyticOscillon(T, v, -0.1, phase, d, l)
            output.append(osc)

        return asarray(output)

    def __str__(self):
        paramsStr = 'v', 'V', 'phase', 'd', 'l'
        retStr = ''
        for i in range(len(self.result.x)):
            retStr += paramsStr[i] + ' = ' + str(self.result.x[i]) + '\n'

        return retStr


class OscFitViewer(object):
    def __init__(self, simData, result):
        self.simData = simData
        self.result = result
