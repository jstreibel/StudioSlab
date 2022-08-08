

from os.path import isfile, getsize
from ast import literal_eval as lit_eval
import numpy as np

from numpy import asarray, double, linspace, gradient, amax, mean, heaviside, abs as np_abs, zeros, array

from Utils import utils


class SimData(object):

    @property
    def MetaData(self):
        return self._metaData

    @property
    def Data(self):
        return self._fieldData

    @property
    def Phi(self):
        return self._phi

    @property
    def dPhidt(self):
        return self._dphidt


    def getItems(self, keyIterable):
        return [self._metaData[a] for a in keyIterable]

    def __contains__(self, key):
        return key in self._metaData

    def __getitem__(self, item):
        if item == 'phi': return self.Data

        if item == 'T':
            print("Trying to access SimData metadata key 'T'. Assuming it is time.")
            item = 't'

        if item == 'initTime' and 'initTime' not in self._metaData:
            return 0.0

        return self._metaData[item]


    def __init__(self, filename):
        super(SimData, self).__init__()

        metaData, headerSizeInBytes = self._testFileAndRetrieveMetadata(filename)

        outresX = metaData['outresX']
        dataSize = 4 # bytes
        channels = metaData['data_channels']
        entriesPerInstant = outresX*channels

        entriesPerInstant += 1*metaData['lines_contain_timestamp']

        totalBytesPerInstant = dataSize*entriesPerInstant

        self._filename = filename
        self._headerSizeInBytes = headerSizeInBytes
        self._entriesPerInstant = entriesPerInstant
        self._metaData = metaData
        self._totalChannels = channels
        self._totalBytesPerInstant = totalBytesPerInstant

        self._buildFieldData()


    def _testFileAndRetrieveMetadata(self, filename):
        if not isfile(filename):
            print("File not found \'" + filename + "\'")
            raise FileNotFoundError

        with open(filename, mode='rb') as file:
            headerData = file.readline()

        try:
            header = headerData.decode('utf-8').strip('# ')
            metaData = lit_eval(header)
        except:
            raise Exception("Unsupported .oscb file format")

        if type(metaData) is not dict:
            raise Exception("Supported .oscb file by current SimData class is version 4 only. "
                            "For earlier versions using .osc, use class SimData_old.")

        if metaData['data_type'] != 'fp32':
            raise NotImplementedError

        if not metaData['lines_contain_timestamp']:
            raise Exception("Instants should contain timestamps")

        headerSizeInBytes = len(headerData)

        return metaData, headerSizeInBytes

    def _buildFieldData(self):
        md = self.MetaData
        outresX, outresT = md['outresX'], md['outresT']

        with open(self._filename, mode='rb') as file:
            file.seek(self._headerSizeInBytes)

            bufferSize = outresT * self._totalBytesPerInstant

            assert(getsize(self._filename) == (self._headerSizeInBytes+bufferSize))

            buffer = file.read(bufferSize)

        entriesPerInstant = self._entriesPerInstant

        numericData = np.frombuffer(buffer, np.float32)
        numericData.shape = (outresT, entriesPerInstant)

        timeStamps = numericData[:,0]

        # TODO esse 2 do outresX*2 eh por causa do numero de canais?? Obs.: N+1 because there's timestamps included in file.
        numericData = numericData[:,1:outresX*self._totalChannels+1]

        self._timeStamps = timeStamps
        self._fieldData = numericData

        outresX = self._metaData['outresX']

        self._phi = self._fieldData[:, :outresX]
        self._dphidt = self._fieldData[:,outresX:outresX*2]

        return numericData


    def _getTimeForLine(self, nLine):
        n, t_tot = self.getItems(('outresT', 't'))

        t = (nLine-1)*t_tot/n

        if t > t_tot: return t_tot

        return t

    def _getLineForTime(self, t):
        n, t_tot = self.getItems(('outresT', 't'))

        #maxLines = self.Phi.shape(1)

        return int(n*t/float(t_tot))

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

        lineSplit = self.Phi[nLine]
        if self['lines_contain_timestamp']: lineSplit = lineSplit[1:]

        return asarray([float(digit) for digit in lineSplit])

    def getXDiscrete(self):
        L_2 = float(self["L"])*.5 # L/2
        x_res = float(self["outresX"])
        return linspace(-L_2, L_2, x_res)

    def getAtTime(self, t):
        nLine = self._getLineForTime(t)
        return self.Phi[nLine]

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
        nLine = self._getLineForTime(t)

        return self.dPhidt[nLine]

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

        N, L, n, Ttot = self.getItems(('outresX', 'L', 'outresT', 't'))
        Nmin = int(N*(Xmin/L + 0.5))
        Nmax = int(N*(Xmax/L + 0.5))

        return asarray(phiT[Nmin:Nmax].flatten(), dtype=double)

    def getSpaceInterval(self, t, x0, x1):
        return self.getInstant(t, x0, x1)

    #def VPert(eta):
    #    UnitStep = lambda arg: heaviside(arg, 0.5)
    #    Hn = lambda arg: UnitStep(arg+2.0) - UnitStep(arg-2.0)

    #    V = zeros(eta.shape)
    #    for n in range(-2, 3):
    #        arg = eta - 4.0*float(n)
    #        #arg = np.full(phi.shape, 2.05) - 4.0*float(n)
    #        V = V + (np_abs(arg) - 0.5*arg**2)*Hn(arg)

    #    return V

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

        #V = SimData_depr.VPert(phi) if usePerturbedPotential else np_abs(phi)
        V = np_abs(phi)

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

    def getRegion(self, XMin, XMax, TMin, TMax):
        p = self._metaData
        n = ((TMax-TMin)/p['t'])*p['outresT']

        region = [self.getInstant(T, XMin, XMax) for T in linspace(TMin, TMax, n)]
        return asarray(region)