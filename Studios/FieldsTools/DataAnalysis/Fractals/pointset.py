from abc import ABC, abstractmethod
from math import log

from numpy import asarray, linspace, meshgrid



class PointSet(ABC):
    def __init__(self, args):
        pass

    @abstractmethod
    def getPoints(self):
        pass
    @abstractmethod
    def getExpectedPhaseTransitions(self):
        pass
    @abstractmethod
    def getAnalyticDimension(self):
        pass
    @abstractmethod
    def getAnalyticDimensionStr(self):
        pass
    @abstractmethod
    def getName(self):
        pass
    @abstractmethod
    def getDescription(self):
        pass


def _makeCantor(x1, x2, level, flist=[]):
    raise Exception("Not implemented.")

    xd = (x2 - x1)
    yd = (y2 - y1)
    xa = x1 + xd / 3.0
    ya = y1 + yd / 3.0
    xb = x1 + xd * 2.0 / 3.0
    yb = y1 + yd * 2.0 / 3.0
    xm = (x1 + x2) / 2.0 - 0.866 * yd / 3.0
    ym = (y1 + y2) / 2.0 + 0.866 * xd / 3.0
    flist.append((xa,ya,0))
    flist.append((xm,ym,0))
    flist.append((xb,yb,0))

    if (level > 0):
       _makeKoch(x1, y1, xa, ya, level-1, flist)
       _makeKoch(xa, ya, xm, ym, level-1, flist)
       _makeKoch(xm, ym, xb, yb, level-1, flist)
       _makeKoch(xb, yb, x2, y2, level-1, flist)

    return flist

class CantorPointSet(PointSet):
    def __init__(self, order):
        self._pointset = asarray(_makeCantor(0, 1, order))
        self._order = order

    def getPoints(self):
        return self._pointset
    def getExpectedPhaseTransitions(self):
        return (3.**-self._order, )
    def getAnalyticDimension(self):
        return log(4)/log(3)
    def getAnalyticDimensionStr(self):
        return "log(4)/log(3)"
    def getName(self):
        return "Koch"
    def getDescription(self):
        return "Calculado para conjunto Koch gerado com " + str(self._order) + " iteracoes."


def _makeKoch(x1, y1, x2, y2, level, flist=[]):
    xd = (x2 - x1)
    yd = (y2 - y1)
    xa = x1 + xd / 3.0
    ya = y1 + yd / 3.0
    xb = x1 + xd * 2.0 / 3.0
    yb = y1 + yd * 2.0 / 3.0
    xm = (x1 + x2) / 2.0 - 0.866 * yd / 3.0
    ym = (y1 + y2) / 2.0 + 0.866 * xd / 3.0
    flist.append((xa,ya,0))
    flist.append((xm,ym,0))
    flist.append((xb,yb,0))

    if (level > 0):
       _makeKoch(x1, y1, xa, ya, level-1, flist)
       _makeKoch(xa, ya, xm, ym, level-1, flist)
       _makeKoch(xm, ym, xb, yb, level-1, flist)
       _makeKoch(xb, yb, x2, y2, level-1, flist)

    return flist

class KochPointSet(PointSet):
    def __init__(self, koch_order):
        self._pointset = asarray(_makeKoch(0, 0, 1, 0, koch_order))
        self._order = koch_order

    def getPoints(self):
        return self._pointset
    def getExpectedPhaseTransitions(self):
        return (3.**-self._order, )
    def getAnalyticDimension(self):
        return log(4)/log(3)
    def getAnalyticDimensionStr(self):
        return "log(4)/log(3)"
    def getName(self):
        return "Koch"
    def getDescription(self):
        return "Calculado para conjunto Koch gerado com " + str(self._order) + " iteracoes."



class TwoParamPointSet(PointSet):
    def __init__(self, n=100, m=100, func=lambda u, v: (u, v, 0.1)):
        points = []
        for u in linspace(0., 1., n):
            for v in linspace(0., 1., m):
                point = asarray(func(u, v))
                points.append(point)

        self._pointset = asarray(points)
        self._n = n
        self._m = m

        self._phaseTransitions = [1./n, 1./m]

    def getPoints(self):
        return self._pointset
    def getExpectedPhaseTransitions(self):
        return self._phaseTransitions
    def getAnalyticDimension(self):
        return 2
    def getAnalyticDimensionStr(self):
        return "2"
    def getName(self):
        return "Plano"
    def getDescription(self):
        n = str(self._n)
        m = str(self._m)
        return "Calculado para plano gerado com " + n + "x" + m  + " pontos entre [0,1]."

    def addPhaseTransition(self, eps):
        self._phaseTransitions.append(eps)


class OneParamPointSet(PointSet):
    def __init__(self, n=100, func=lambda t: (t, 0, 0)):
        points = []
        for t in linspace(0., 1., n):
            point = asarray(func(t))
            points.append(point)

        self._pointset = asarray(points)
        self._n = n

        self._phaseTransitions = [1./n, ]

    def getPoints(self):
        return self._pointset
    def getExpectedPhaseTransitions(self):
        return self._phaseTransitions
    def getAnalyticDimension(self):
        return 1
    def getAnalyticDimensionStr(self):
        return "1"
    def getName(self):
        return "Linha"
    def getDescription(self):
        n = str(self._n)
        return "Calculado para linha gerada com " + n + " pontos entre [0,1]."

    def addPhaseTransition(self, eps):
        self._phaseTransitions.append(eps)
