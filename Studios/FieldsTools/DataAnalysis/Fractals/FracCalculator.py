from Fractals import fraccalc

from numpy import floor, log

class CFracCalculator(object):
    def __init__(self, pointSet):
        self._pointSet = pointSet

        points = pointSet.getPoints()
        n = len(points)
        x, y, z = points[:,0], points[:,1], points[:,2]
        
        print(x)
        fraccalc.Init(n)
        fraccalc.set_xData(x)
        fraccalc.set_yData(y)
        fraccalc.set_zData(z)

    def N(self, eps):
        return fraccalc.getN(eps)

    def dim(self, eps):
        N = self.N(eps)
        if N==0: return -1.0
        d = log(N) / log(1/eps)

        return d

class PyFracCalculator(object):
    def __init__(self, pointSet):
        self._pointSet = pointSet

    def _getBoxes(self, eps):
        points = self._pointSet.getPoints()

        eps = float(eps)
        pointdict = {}
        for (x, y, z) in points:
            i = int(floor(x/eps))
            j = int(floor(y/eps))
            k = int(floor(z/eps))
            index = (i, j, k)

            if index not in pointdict: pointdict[index] = 1

        return pointdict

    def N(self, eps):
        return len(self._getBoxes(eps).keys())

    def dim(self, eps):
        N = self.N(eps)
        if N == 0: return -1.0
        d = log(N) / log(1/eps)

        return d
