#!/usr/bin/env python
# -*- coding: utf-8 -*-


import glob
from multiprocessing import Pool
from numpy import sqrt, unique, asarray
import utils
from open_sim import OpenSimData
from typing import List

import argparse


PRINT_DENSITY_FIGURE_TO_TERMINAL = False

baseFormat = '{:.7f}'



def GET_ENERGY_CALC_ARGUMENTS(parser: argparse.ArgumentParser = None) -> argparse.ArgumentParser:
    if parser is None:
        parser = argparse.ArgumentParser()

    myGroup = parser.add_argument_group("Energy computing")

    myGroup.add_argument('--oscsFiles', '-f', help="Filename(s). Default is all .oscs files in folder. If a folder "
                                                   "is given, it's taken as the root of a tree directory structure.",
                         nargs='*', default='./*.oscs')
    myGroup.add_argument('--useTreeDirectoryStructure', '-tr', help="Flag to tell that files are organized in a tree "
                                                                    "directory structure.",
                         action='store_true')
    myGroup.add_argument('--variableParameters', '-v', help='The two variable parameters to create the map upon.',
                         nargs=2, default=('a1', 'a2'))
    myGroup.add_argument('--threads', '-t', help='Number of threads to use. Default is 1.', type=int, default=1)
    myGroup.add_argument('--nStride', '-n', help='\'n\' stride determines if we jump over files (and how much we jump) in '
                                                'the u (or V) direction.', type=int, default=1)
    myGroup.add_argument('--mStride', '-m', help='\'m\' stride determines if we jump over files (and how much we jump) in '
                                                'the \\alpha (or phase) direction.', type=int, default=1)

    return parser


class ReducedParameters:
    def __init__(self, a1: float, a2: float, loc: (int, int), shape: (int, int), u: float, fileName: str) -> None:
        self._a1 = a1
        self._a2 = a2
        self._loc = loc
        self._shape = shape
        self._u = u
        self._fileName = fileName

    @property
    def a1(self) -> float:
        return self._a1
    @property
    def a2(self) -> float:
        return self._a2
    @property
    def loc(self) -> (int,int):
        return self._loc
    @property
    def shape(self) -> (int,int):
        return self._shape
    @property
    def j(self) -> int:
        return self._loc[1]
    @property
    def i(self) -> int:
        return self._loc[0]
    @property
    def j(self) -> int:
        return self._loc[1]
    @property
    def u(self) -> float:
        return self._u
    @property
    def fileName(self) -> str:
        return self._fileName

    @property
    def analyticEnergyOfConfiguration(self):
        u = self._u

        return 1./(24.*sqrt(1.-u*u))

    @property
    def oscillonWidth(self):
        ''' Pode variar com a velocidade da borda, que nao esta contemplada aqui. '''
        u = self._u
        return 1.*sqrt(1.-u*u)

    def __getitem__(self, item: int):
        if item == 0: return self.a1
        elif item == 1: return self.a2
        elif item == 2: return self.i
        elif item == 3: return self.j

        raise IndexError()


class EnergyCompute:
    def __init__(self, params: ReducedParameters, energies: (float, float, float) = None) -> None:
        super().__init__()
        self._params = params

        if energies is not None:
            E_left, E_right, E_tot = energies
            self._E_left = E_left
            self._E_right = E_right
            self._E_tot = E_tot
        else:
            self._calcEnergy()

    def _calcEnergy(self):
        params = self._params

        fileName = params.fileName
        simData = OpenSimData(fileName)
        t = simData['t']
        u = params.u
        oscWidth = params.oscillonWidth

        slack = 0.5*oscWidth
        xMin = -t*u-slack
        xMax = xMin + oscWidth + 2.*slack

        E_left = simData.integrateEnergyInterval(xMin, xMax)
        E_right = simData.integrateEnergyInterval(-xMax, -xMin)
        E_tot = simData.getTotalEnergy()

        self._E_left = E_left
        self._E_right = E_right
        self._E_tot = E_tot

    @property
    def params(self) -> ReducedParameters:
        return self._params
    @property
    def E_left(self) -> float:
        return self._E_left
    @property
    def E_right(self) -> float:
        return self._E_right
    @property
    def E_tot(self) -> float:
        return self._E_tot


class FileNameParameterExtractor:
    def __init__(self, files, nStride = 1, mStride = 1, treeStructure: bool = True):
        ''' Por enquanto so funciona com alpha_1 e alpha_2. '''

        files = glob.glob(files)
        files.sort()

        if files[0][-5:] != '.oscs':
            raise Exception("SnapshotEnergyCompute currently supports only .oscs file type. "
                            "Filetype is " + files[0][-5])

        theMap = {}
        for fileName in files:
            if fileName[-5:] != '.oscs':
                print("Unknown file type", fileName)
                continue

            fileName = fileName[:-5]

            fileNameArgs = fileName.replace('./', '').replace('snapshot-gen-+-', '').replace('--', '-').split('-')

            for arg in fileNameArgs:
                key, val = arg.split('=')
                val = float(val)
                key = 'u' if key == 'V' else 'alpha' if key == 'ph' else key

                if key in theMap:
                    theMap[key].append(val)
                else:
                    theMap[key] = [val, ]
        for key in theMap:
            theMap[key] = unique(asarray(theMap[key]))

        self._theMap = theMap

        self._nStride = nStride
        self._mStride = mStride

        self._buildBaseFileFormat(treeStructure)
        self._buildTheEasyIterableList()

    def _buildBaseFileFormat(self, treeStructure: bool) -> None:
        fullParameters = self._theMap
        bf = baseFormat
        self._fileFormats = ("snapshot-gen-+-v1={v1}-v2={v2}-a1={a1}-a2={a2}-l=1.000000-V={V}--N={N}-t={t}.oscs",
                             "./v1=" + bf + "/v2=" + bf + "/a1=" + bf + "/a2=" + bf + "/V=" + bf + "/N={}.oscs")
        if treeStructure:
            fileFormat = self._fileFormats[1]
        else:
            fileFormat = self._fileFormats[0]

        v1, v2, l, V, N, t = fullParameters['v1'][0], fullParameters['v2'][0], fullParameters['l'][0], \
                             fullParameters['u'][0], fullParameters['N'][0], fullParameters['t'][0]

        fileFormat = fileFormat.replace('{v1}', utils.fmt(v1, 6)) \
            .replace('{v2}', utils.fmt(v2, 6)) \
            .replace('{l}', utils.fmt(l, 6)) \
            .replace('{V}', utils.fmt(V, 6)) \
            .replace('{N}', str(int(N))) \
            .replace('{t}', utils.fmt(t, 4))

        print(fileFormat.replace('{a1}', utils.Bblue('{a1}')).replace('{a2}', utils.Bblue('{a2}')))

        self._pMap = fullParameters
        self._fileFormat = fileFormat
        self._treeStructure = treeStructure

    def _formatFileName(self, a1: float, a2: float) -> str:
        return self._fileFormat.replace('{a1}', utils.fmt(a1, 6)).replace('{a2}', utils.fmt(a2, 6))

    def _buildTheEasyIterableList(self):
        fullParameters = self._theMap
        nStride = self._nStride
        mStride = self._mStride

        a1_v = fullParameters['a1']
        a2_v = fullParameters['a2']
        u = fullParameters['u'][0]
        t = fullParameters['t'][0]

        n, m = int(len(a1_v)/nStride), int(len(a2_v)/mStride)

        list = []
        for i in range(n):
            for j in range(m):
                a1 = a1_v[int(i * nStride)]
                a2 = a2_v[int(j * mStride)]
                list.append(ReducedParameters(a1, a2, (i,j), (n,m), u, self._formatFileName(a1, a2)))

        self._list = list

        self._shape = n, m

    def getShape(self) -> (int, int):
        return self._shape

    def __getitem__(self, item):
        if type(item) is int:
            return self._list[item]
        elif type(item) is str:
            return self._theMap[item]

        raise Exception("Unknown type" + str(type(item)))


def _doCase(reducedParams: ReducedParameters) -> EnergyCompute:
    energy = EnergyCompute(reducedParams)

    if PRINT_DENSITY_FIGURE_TO_TERMINAL:
        normRadiatedEnergy = 1. - (energy.E_left + energy.E_right) / energy.E_tot
        gsacsii = utils.asciiGrayScale(normRadiatedEnergy)

        sh_rows, sh_columns = utils.getTerminalSize()
        i, j = reducedParams.loc
        w, h = sh_columns-2, sh_rows-2
        n, m = reducedParams.shape

        utils.printThere(int(1 + (i+.5)*w/n), int(1 + (j+.5)*h/m), gsacsii, True)

    return energy


def _doAllCases(paramsList: FileNameParameterExtractor, nThreads: int) -> List[EnergyCompute]:
    if PRINT_DENSITY_FIGURE_TO_TERMINAL:
        utils.clearTerminal()

    if nThreads > 1:
        return Pool(nThreads).map(_doCase, paramsList)
    else:
        result = []
        for arguments in paramsList: result.append(_doCase(arguments))
        return result


def ComputeEnergyMap(parser: argparse.ArgumentParser = None) -> str:
    if parser is None:
        parser = GET_ENERGY_CALC_ARGUMENTS(parser)

    args = parser.parse_args()

    theParametersMap = FileNameParameterExtractor(args.oscsFiles, args.nStride, args.mStride,
                                                  args.useTreeDirectoryStructure)

    results = _doAllCases(theParametersMap, args.threads)
    u = theParametersMap['u'][0]
    a1_v = theParametersMap['a1']
    a2_v = theParametersMap['a2']

    fmt = utils.fmt
    outputFileName = 'rad-u=' + fmt(u, 5) + '.enmap'
    with open(outputFileName, 'w') as output:
        output.write('# (v1, v2, l, V, N, t) ; shape ; a1_v ; a2_v ; (E_left, E_right, E_tot, i, j) \n')

        v1, v2, l, V, N, t = theParametersMap['v1'][0], theParametersMap['v2'][0], theParametersMap['l'][0], \
                             theParametersMap['u'][0], theParametersMap['N'][0], theParametersMap['t'][0]
        output.write('(' + fmt(v1, 6) + "," + fmt(v2, 6) + ',' + fmt(l, 6) + ',' + fmt(V, 6) + ',' + str(int(N)) + ','
                         + fmt(t, 6) + ')\n')
        output.write(str(theParametersMap.getShape()) + '\n')

        output.write("(")
        for a1 in a1_v:
            output.write(fmt(a1,5) + ",")
        output.write(")\n")

        output.write("(")
        for a2 in a2_v:
            output.write(fmt(a2,5) + ",")
        output.write(")\n")

        output.write('[')
        for energyResult in results:
            E_left = energyResult.E_left
            E_right = energyResult.E_right
            E_tot = energyResult.E_tot
            i, j = energyResult.params.loc
            output.write("(" + str(E_left) + "," + str(E_right) + "," + str(E_tot) + "," + str(i) + "," + str(j) + "),")
        output.write(']')

    return outputFileName


