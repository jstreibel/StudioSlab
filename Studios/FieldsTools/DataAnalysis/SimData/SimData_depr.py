from linecache import getline, clearcache

from numpy import asarray, double, linspace, gradient, amax, mean, heaviside, abs as np_abs, zeros, array
from ast import literal_eval as lit_eval
from os.path import isfile

from DataUtils_depr import _extract_timestamps, _open_sim_data, _open_old

from Utils import utils

class SimData_depr(object):
    """Raw access to oscillon simulation data. Note that full simulation
    data is not loaded until requested."""
    def __init__(self, filename, resolution_x='full'):
        if not isfile(filename):
            print("File not found \'" + filename + "\'")
            raise FileNotFoundError

        super(SimData_depr, self).__init__()
        self._filename = filename

        line1 = getline(filename, 1)
        try:
            fullSimParams = lit_eval(line1.strip('# '))
        except:
            fullSimParams = {"Ver": 1, "lines_contain_timestamp": False }

        ver = fullSimParams['Ver']
        if ver in [1, 2]:
            keysLine = getline(filename, ver) # casualmente, isso funciona.
            valsLine = getline(filename, ver+1)

            param_keys = [val for val in keysLine.replace(' ', '').strip('#').strip().split('|')]
            param_vals= [float(val) if utils.isfloat(val) else val for val in valsLine.split()]
            fullSimParams.update(dict(zip(param_keys, param_vals)))
        elif ver not in [1, 2, 3, 4]:
            raise Exception("Unknown oscfile version.")

        fullSimParams['filename'] = filename

        # Correcao para alguns arquivos:
        if 'ph' in fullSimParams and 'phase' in fullSimParams:
            fullSimParams['phase'] = fullSimParams['ph']
            fullSimParams['alpha'] = fullSimParams['ph']
        if 'l' not in fullSimParams:
            fullSimParams['l'] = 1.
        if 'initTime' not in fullSimParams:
            fullSimParams['initTime'] = 0.
        if 'T' not in fullSimParams and 't' in fullSimParams:
            fullSimParams['T'] = fullSimParams['t']

        self._simParams = fullSimParams

        self._hasTimeStamps = fullSimParams['lines_contain_timestamp']

        # _dict refere-se ao dicionario que vc tem quando usa _open_sim
        self._dict = None

        self._resolution_x = resolution_x

        if self._simParams['Ver'] == 1: self._headerSize = 2
        elif self._simParams['Ver'] == 2: self._headerSize = 3
        elif self._simParams['Ver'] in [3, 4]: self._headerSize = 1
        else: raise Exception("Oscillon file version unknown.")

    def __del__(self):
        self._clearCache()

    def getPhi(self):
        if 'phi' not in self._simParams:
            with open(self._filename) as file:
                file.readline()
                file.readline()
                if self._simParams['Ver'] == 2: file.readline()
                if self._resolution_x == 'full':
                    phi = asarray([[float(digit) for digit in line.split()] for line in file])
                else:
                    ''' TODO: TERMINAR DE IMPLEMENTAR ISSO AQUI. EH UMA VERSAO MAIS
                         LIGHT DA ABERTURA DO ARQUIVO, EM QUE NAO PEGAMOS TUDO E PULAMOS
                        ALGUMAS LINHAS. '''

                    #resx = int(self._resolution_x) # ja testa pra ver se eh mesmo numero
                    #step = int(self['outresX'])/resx
                    raise NotImplementedError()

            if self._hasTimeStamps:
                timestamps, phi = _extract_timestamps(phi)
                dtArray = gradient(timestamps)
                max_dt = amax(dtArray)
                mean_dt = mean(dtArray)

                self._simParams.update({'max_dt': max_dt, 'mean_dt': mean_dt})
            else:
                timestamps = None

            self._simParams.update({'phi': phi, 'timestamps': timestamps})

        return self['phi']

    def getFullDictionary(self):
        if self._dict is None:
            self._dict = _open_sim_data(self._filename)

        return self._dict

    def __contains__(self, key):
        return key in self._simParams

    def __setitem__(self, key, item):
        self._simParams[key] = item

    def __getitem__(self, key):
        # Phi nao fica carregado desde sempre, porque pode se tratar de muitos dados, e muitas vezes precisamos apenas de uma parcela destes dados.
        if key == 'phi' and key not in self._simParams:
            return self.getPhi()

        return self._simParams[key]

    def getItems(self, keyIterable):
        return [self._simParams[a] for a in keyIterable]

    def _getTimeForLine(self, nLine):
        n, t_tot = self.getItems(('outresT', 'T'))

        t = (nLine-self._headerSize-1)*t_tot/n

        if t > t_tot: return t_tot

        return t

    def _getLineForTime(self, t):
        n, t_tot = self.getItems(('outresT', 'T'))

        nLine = int(n*t/float(t_tot)) + self._headerSize + 1
        linesInFile = self._headerSize + int(self['outresT'])
        if nLine > linesInFile:
            # print("No data for time t=" + str(t) + " (corresponds to line " + str(nLine) + "). File contains " + str(linesInFile) + " lines. Using last line.")

            return linesInFile - 1  # gambiarra?

        return nLine

    def _get_dtEffective(self):
        t_res, t_tot = self["outresT"], self["T"]
        return t_tot / float(t_res)

    def _getNextTime(self, t):
        t_res, t_tot = self["outresT"], self["T"]
        dt = t_tot/float(t_res)

        return t + dt*1.1

    _haveIWarned = False
    def _getAtLine(self, nLine):
        if self['Ver'] == 1 and not self._haveIWarned:
            self._haveIWarned = True
            print(utils.WARNING + "# WARNING: This .osc file is version 1 and returns wrong times for arbitrary lookup.")

        lineSplit = getline(self._filename, nLine).split()
        if self['lines_contain_timestamp']: lineSplit = lineSplit[1:]

        return asarray([float(digit) for digit in lineSplit])

    def getXDiscrete(self):
        L_2 = float(self["L"])*.5 # L/2
        x_res = float(self["outresX"])
        return linspace(-L_2, L_2, x_res)

    def getAtTime(self, t):
        nLine = self._getLineForTime(t)
        return self._getAtLine(nLine)

    def getFullHistoryAtPosition(self, x):
        moi = self

        xMin = moi['xLeft']
        L = moi['L']
        N = moi['outresX']
        phi = moi['phi']

        if x<xMin or x>(xMin+L):
            raise Exception("Ooopsie x is outside range of simulation.")

        i = int(N*(x-xMin)/L)

        return phi[:,i]


    def getTimeDerivative(self, t):
        nLine0 = self._getLineForTime(t)
        nLine1 = nLine0 + 1

        phi0 = self._getAtLine(nLine0)
        phi1 = self._getAtLine(nLine1)

        t_res, t_tot = self["outresT"], self["T"]
        dt = t_tot/float(t_res)

        return (phi1-phi0)/dt

    def getTimeDerivativeInterval(self, t, x0, x1):
        dphidt = self.getTimeDerivative(t)

        N, L, = self.getItems(('outresX', 'L'))
        Nmin = int(N*(x0/L + 0.5))
        Nmax = int(N*(x1/L + 0.5))

        return asarray(dphidt[Nmin:Nmax].flatten(), dtype=double)

    def getSpaceDerivative(self, t):
        phi = self.getAtTime(t)
        dx = self['h']
        return gradient(phi, dx)

    def getSpaceDerivativeInterval(self, t, x0, x1):
        dphidx = self.getSpaceDerivative(t)

        N, L, = self.getItems(('outresX', 'L'))
        Nmin = int(N*(x0/L + 0.5))
        Nmax = int(N*(x1/L + 0.5))

        return asarray(dphidx[Nmin:Nmax].flatten(), dtype=double)

    def getNForWidth(self, width):
        N, L = self.getItems(('outresX', 'L'))
        return int(N*width/L)

    def getNLims(self, x0, x1):
        N, L = self['outresX'], self['L']

        Nmin = int(N*(x0/L + 0.5))
        Nmax = int(N*(x1/L + 0.5))

        return Nmin, Nmax

    def getInstant(self, t, Xmin, Xmax):
        phiT = self.getAtTime(t)

        N, L, n, Ttot = self.getItems(('outresX', 'L', 'outresT', 'T'))
        Nmin = int(N*(Xmin/L + 0.5))
        Nmax = int(N*(Xmax/L + 0.5))

        return asarray(phiT[Nmin:Nmax].flatten(), dtype=double)

    def getSpaceInterval(self, t, x0, x1):
        return self.getInstant(t, x0, x1)

    def VPert(eta):
        UnitStep = lambda arg: heaviside(arg, 0.5)
        Hn = lambda arg: UnitStep(arg+2.0) - UnitStep(arg-2.0)

        V = zeros(eta.shape)
        for n in range(-2, 3):
            arg = eta - 4.0*float(n)
            #arg = np.full(phi.shape, 2.05) - 4.0*float(n)
            V = V + (np_abs(arg) - 0.5*arg**2)*Hn(arg)

        return V

    def getKineticEnergyDensityInInterval(self, t, x0, x1):
        dphidt = self.getTimeDerivativeInterval(t, x0, x1)

        return .5 * dphidt ** 2

    def getGradientEnergyDensityInInterval(self, t, x0, x1):
        phi = self.getInstant(t, x0, x1)
        dx = self['L'] / self['outresX']

        dphidx = gradient(phi, dx)

        return .5 *  dphidx ** 2

    def getPotentialEnergyInInterval(self, t, x0, x1):
        phi = self.getInstant(t, x0, x1)

        return np_abs(phi)

    def getKineticAndGradientEnergyDensityInInterval(self, t, x0, x1):
        phi = self.getInstant(t, x0, x1)
        dx = self['L'] / self['outresX']

        dphidx = gradient(phi, dx)
        dphidt = self.getTimeDerivativeInterval(t, x0, x1)

        return .5 * (dphidt ** 2 + dphidx ** 2)

    def getKineticAndGradientEnergyDensity(self, t):
        phi = self.getAtTime(t)
        dx = self['L'] / self['outresX']

        dphidx = gradient(phi, dx)
        dphidt = self.getTimeDerivative(t)

        return .5 * (dphidt ** 2 + dphidx ** 2)


    def getKineticEnergyDensity(self, t):
        dphidt = self.getTimeDerivative(t)

        return .5 * dphidt ** 2


    def getEnergyDensity(self, t, usePerturbedPotential=False):
        phi = self.getAtTime(t)
        dx = self.get_dxEffective()

        dphidx = gradient(phi, dx)
        dphidt = self.getTimeDerivative(t)

        V = SimData_depr.VPert(phi) if usePerturbedPotential else np_abs(phi)

        return .5*dphidt**2 + .5*dphidx**2 + V

    def get_dxEffective(self):
        return self['L']/self['outresX']

    def getEnergyDensityIntervalIndexedTime(self, tIndex, x0, x1):
        t = self._getTimeForLine(tIndex)

        return self.getEnergyDensityInterval(t, x0, x1)

    def getEnergyDensityInterval(self, t, x0, x1):
        E = self.getEnergyDensity(t)

        Nmin, Nmax = self.getNLims(x0, x1)

        return E[Nmin:Nmax]

    def getTotalEnergy(self, t):
        E = self.getEnergyDensity(t)
        dx = self.get_dxEffective()

        return E.sum()*dx

    def getEnergyIndefiniteIntegral(self, t):
        E = self.getEnergyDensity(t)
        dx = self.get_dxEffective()

        return E.cumsum()*dx

    def integrateEnergyInterval(self, t, x0, x1):
        E = self.getEnergyDensityInterval(t, x0, x1)
        dx = self.get_dxEffective()

        return E.sum()*dx

    def getLinMomentumDensity(self, t: float) -> array:
        phi = self.getAtTime(t)
        dx = self.get_dxEffective()

        dphidx = gradient(phi, dx)
        dphidt = self.getTimeDerivative(t)

        return dphidx*dphidt

    def getLinMomentumDensityInterval(self, t: float, x0: float, x1: float) -> array:
        linMomentumDensity = self.getLinMomentumDensity(t)
        Nmin, Nmax = self.getNLims(x0, x1)

        return linMomentumDensity[Nmin:Nmax]

    def integrateLinMomentumInterval(self, t:float, x0: float, x1: float) -> float:
        linMomentumDensity = self.getLinMomentumDensityInterval(t, x0, x1)
        dx = self.get_dxEffective()

        return linMomentumDensity.sum()*dx

    @staticmethod
    def _clearCache():
        ''' Limpa o cache do linecache. '''
        clearcache()

    def getRegion(self, XMin, XMax, TMin, TMax):
        p = self._simParams
        n = ((TMax-TMin)/p['T'])*p['outresT']

        region = []
        for T in linspace(TMin, TMax, n):
            region.append(self.getInstant(T, XMin, XMax))

        return asarray(region)
