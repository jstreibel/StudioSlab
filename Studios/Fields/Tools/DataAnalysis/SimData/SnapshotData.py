

from os.path import isfile
from numpy import asarray, gradient, abs as np_abs, array

from ast import literal_eval as lit_eval

#from SimData_old import SimData_old


class SnapshotData(object):

    def __init__(self, fileName) -> None:
        super().__init__()

        if not isfile(fileName):
            print("File not found \'" + fileName + "\'")
            raise FileNotFoundError

        self._fileName = fileName

        with open(fileName) as file:
            line = file.readline()
            self._allParams = lit_eval(line.strip('# '))

            file.readline() # descarta uma linha

        for key in self._allParams:
            param = self._allParams[key]
            if type(param) is list or type(param) is tuple:
                self._allParams[key] = asarray(param)

    def __getitem__(self, item):
        return self._allParams[item]

    def getEnergyDensity(self) -> array:
        phi = self['phi']
        dx = self['L']/self['N']

        dphidx = gradient(phi, dx)
        dphidt = self['dphidt']

        #V = SimData_old.VPert(phi) if self['pert'] else np_abs(phi)
        V = np_abs(phi)
        return .5*dphidt**2 + .5*dphidx**2 + V

    def getNLims(self, x0, x1):
        N, L = self['N'], self['L']

        Nmin = int(N*(x0/L + 0.5))
        Nmax = int(N*(x1/L + 0.5))

        Nmin = max(Nmin, 0)
        Nmax = min(Nmax, N-1)

        return Nmin, Nmax

    def get_dxEffective(self):
        return self['L']/self['N']

    def getEnergyDensityInterval(self, x0, x1):
        E = self.getEnergyDensity()

        Nmin, Nmax = self.getNLims(x0, x1)

        return E[Nmin:Nmax]

    def integrateEnergyInterval(self, x0, x1):
        E = self.getEnergyDensityInterval(x0, x1)
        dx = self.get_dxEffective()

        return E.sum()*dx

    def getTotalEnergy(self) -> float:
        E = self.getEnergyDensity()
        dx = self.get_dxEffective()

        return E.sum()*dx

    def getLinMomentumDensity(self) -> array:
        phi = self['phi']
        dx = self['L']/self['N']

        dphidx = gradient(phi, dx)
        dphidt = self['dphidt']

        return dphidx*dphidt

    def getLinMomentumDensityInterval(self, x0:float, x1: float) -> array:
        linMomentumDensity = self.getLinMomentumDensity()
        Nmin, Nmax = self.getNLims(x0, x1)

        return linMomentumDensity[Nmin:Nmax]

    def integrateLinMomentumInterval(self, x0: float, x1: float) -> float:
        linMomentumDensity = self.getLinMomentumDensityInterval(x0, x1)
        dx = self.get_dxEffective()

        return linMomentumDensity.sum()*dx
