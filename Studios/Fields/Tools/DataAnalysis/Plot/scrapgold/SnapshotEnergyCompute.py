#!/usr/bin/env python
# -*- coding: utf-8 -*-


from multiprocessing import Pool
from numpy import sqrt, unique, asarray, linspace
from Utils import utils
from SimData import OpenSimData, SnapshotData
from typing import List, Dict, Tuple
from ast import literal_eval

from glob import glob

import argparse


PRINT_DENSITY_FIGURE_TO_TERMINAL = True

baseFormat = '{:.7f}'

generateDebugInfo = False


def GET_ENERGY_CALC_ARGUMENTS(parser: argparse.ArgumentParser = None) -> argparse.ArgumentParser:
    if parser is None:
        parser = argparse.ArgumentParser()

    myGroup = parser.add_argument_group("Energy computing")

    myGroup.add_argument('--snapshotsDBRootFolder', '-r', help="The snapshots database root folder.",
                         default='./')
    myGroup.add_argument('--variableParameters', '-v', help='The two variable parameters to create the map upon.',
                         nargs=2, default=('a1', 'a2'))
    myGroup.add_argument('--threads', '-t', help='Number of threads to use. Default is 1.', type=int, default=1)
    myGroup.add_argument('--nStride', '-n', help='\'n\' stride determines if we jump over files (and how much we jump) in '
                                                'the u (or V) direction.', type=int, default=1)
    myGroup.add_argument('--mStride', '-m', help='\'m\' stride determines if we jump over files (and how much we jump) in '
                                                'the \\alpha (or phase) direction.', type=int, default=1)

    myGroup.add_argument('--plotDebugMap', '-d', help="Plot debug map.", action='store_true')

    return parser


class MetaData:
    def __init__(self, theMap: dict, index: int, shape: (int, int)) -> None:
        myMap = {}
        for arg in MetaData._GeneralArgs:
            myMap[arg] = theMap[arg][index]
        self._myMap = myMap
        self._shape = shape

    _GeneralArgs = {"v1": baseFormat, "v2": baseFormat,
                    "a1": baseFormat, "a2": baseFormat,
                    "V": baseFormat, "N": '{}',
                    "fileName": '{}', "loc": '{} {}'}

    @staticmethod
    def getFormat(key: str):
        return MetaData._GeneralArgs[key]

    @property
    def v1(self) -> float:
        return self._myMap['v1']
    @property
    def v2(self) -> float:
        return self._myMap['v2']
    @property
    def a1(self) -> float:
        return self._myMap['a1']
    @property
    def a2(self) -> float:
        return self._myMap['a2']
    @property
    def u(self) -> float:
        return self._myMap['V']
    @property
    def N(self) -> int:
        return self._myMap['N']
    @property
    def fileName(self) -> str:
        return self._myMap['fileName']
    @property
    def shape(self) -> (int, int):
        return self._shape
    @property
    def loc(self) -> str:
        return self._myMap['loc']


class EnergyCompute:
    def __init__(self, metaData: MetaData, keepFullSimData: bool = False) -> None:
        super().__init__()
        self._metaData = metaData

        self._fullSimData = None

        self._t = None

        self._calcEnergy(keepFullSimData)

    def getLeftXLims(self) -> (float, float):
        if self._t is None: raise Exception("Variable 't' is not defined in EnergyCompute.")

        metaData = self._metaData
        t = self._t
        u = metaData.u

        oscWidth = sqrt(1.-u*u) # = 1/gamma(u)
        slack = 0.5*oscWidth
        xMin = -t*u-slack
        xMax = xMin + oscWidth + 2.*slack

        return xMin, xMax

    def _calcEnergy(self, keepFullSimData: bool = False):
        metaData = self._metaData

        fileName = metaData.fileName
        simData = OpenSimData(fileName)
        self._t = simData['t']

        xMin, xMax = self.getLeftXLims()

        E_left = simData.integrateEnergyInterval(xMin, xMax)
        E_right = simData.integrateEnergyInterval(-xMax, -xMin)
        E_tot = simData.getTotalEnergy()
        P_left = simData.integrateLinMomentumInterval(xMin, xMax)
        P_right = simData.integrateLinMomentumInterval(-xMax, -xMin)

        self._E_left = E_left
        self._E_right = E_right
        self._E_tot = E_tot
        self._P_left = P_left
        self._P_right = P_right

        if keepFullSimData:
            self._fullSimData = simData

    @property
    def metaData(self) -> MetaData:
        return self._metaData
    @property
    def E_left(self) -> float:
        return self._E_left
    @property
    def E_right(self) -> float:
        return self._E_right
    @property
    def E_tot(self) -> float:
        return self._E_tot
    @property
    def P_left(self) -> float:
        return self._P_left
    @property
    def P_right(self) -> float:
        return self._P_right
    @property
    def fullSimData(self) -> SnapshotData:
        return self._fullSimData


class SnapshotMetaDataContainer:
    def __init__(self, vParams: (str, str), beVerbose = False) -> None:
        genArgs = MetaData._GeneralArgs
        theMap = {}
        for key in genArgs:
            theMap[key] = []

        self._vParams = vParams
        self._theMap = theMap
        self._shape = ()
        self._beVerbose = beVerbose

    def add(self, key, value) -> None:
        self._theMap[key].append(value)

        if self._beVerbose:
            if key in self._vParams:
                print(" -- " + key + ":", value, end='')

    def finish(self) -> None:
        theMap = self._theMap
        for key in theMap:
            self._theMap[key] = asarray(theMap[key])

        vParams = self._vParams

        width = unique(self._theMap[vParams[0]]).shape[0]
        height = unique(self._theMap[vParams[1]]).shape[0]

        self._shape = (width, height)

    def getUniqueParameters(self) -> dict:
        tinyDict = {}

        theMap = self._theMap
        for key in theMap:
            uniqued = unique(theMap[key])
            if uniqued.shape == (1,) and not key == 'loc':
                tinyDict[key] = uniqued[0]

        return tinyDict

    @property
    def shape(self):
        return self._shape
    @property
    def vParams(self) -> (str, str):
        return self._vParams

    def __getitem__(self, index: int) -> MetaData:
        return MetaData(self._theMap, index, self._shape)

    @property
    def theMap(self):
        return self._theMap


def _computeCase(metaData: MetaData) -> EnergyCompute:
    global generateDebugInfo
    energy = EnergyCompute(metaData, keepFullSimData=generateDebugInfo)

    if PRINT_DENSITY_FIGURE_TO_TERMINAL:
        normRadiatedEnergy = 1. - (energy.E_left + energy.E_right) / energy.E_tot
        gsacsii = utils.asciiGrayScale(normRadiatedEnergy)

        sh_rows, sh_columns = utils.getTerminalSize()
        i, j = metaData.loc
        w, h = sh_columns-2, sh_rows-2
        n, m = metaData.shape

        utils.printThere(int(1 + (i+.5)*w/n), int(1 + (j+.5)*h/m), gsacsii, True)
    else:
        print('.', end='', flush=True)
        pass

    return energy


def _computeAllCases(metaDataContainer: SnapshotMetaDataContainer, nThreads: int) -> List[EnergyCompute]:
    if PRINT_DENSITY_FIGURE_TO_TERMINAL:
        utils.clearTerminal()

    result = []
    if nThreads > 1:
        result = Pool(nThreads).map(_computeCase, metaDataContainer)
    else:
        for metaData in metaDataContainer: result.append(_computeCase(metaData))

    if PRINT_DENSITY_FIGURE_TO_TERMINAL:
        utils.clearTerminal()

    return result

#class DirNameFormatter:
#    def __init__(self, dataBaseRootDir: str, vParams: (str, str)):
#        self._getFirstEntryOfEachArgument(dataBaseRootDir)
#        #for arg, fmt in _GeneralArgs:
#        #    baseFormat += '/' + arg + '=' + fmt
#        pass
#
#    def _getFirstEntryOfEachArgument(self, dbRootDir: str):
#        firstEntryOfEachArg = []
#
#        _GeneralArgs = (("v1", baseFormat), ("v2", baseFormat),
#                        ("a1", baseFormat), ("a2", baseFormat),
#                        ("V", baseFormat), ("N", '{}'))
#
#        dirFormat = dbRootDir
#        dirList = dbRootDir
#        dirList = sorted(glob(dirList + '*'))[0]
#        dirList = sorted(glob(dirList + '/*'))[0]
#        dirList = sorted(glob(dirList + '/*'))[0]
#        dirList = sorted(glob(dirList + '/*'))[0]
#        dirList = sorted(glob(dirList + '/*'))[0]
#
#        print(dirList)
#
#        exit(0)
#
#        for arg, fmt in _GeneralArgs:
#            pass
#
#        raise NotImplemented()
#def buildDataStructura(dataBaseRootDir: str, vParams: (str, str),
#                       stride: (int, int) = (1, 1)) -> SnapshotMetaDataContainer:
#    dirFormat = DirNameFormatter(dataBaseRootDir, vParams)
#    raise NotImplemented


def crawlExistingData(dataBaseRootDir: str, vParams: (str, str), stride: (int, int) = (1, 1)) -> SnapshotMetaDataContainer:
    recurseLevels = {"v1": 1, "v2": 2,
                     "a1": 3, "a2": 4, "V": 5}
    dataRecurseLevel = 6 # nesse nivel de recursao eh para existir dados.

    vParam1RecurseLevel = recurseLevels[vParams[0]]
    vParam2RecurseLevel = recurseLevels[vParams[1]]
    if vParam1RecurseLevel > vParam2RecurseLevel:
        if 1: raise(utils.FAIL + "Change the order of parameters.")

        vParams = vParams[1], vParams[0]
        vParam1RecurseLevel = recurseLevels[vParams[0]]
        vParam2RecurseLevel = recurseLevels[vParams[1]]

    metaDataContainer = SnapshotMetaDataContainer(vParams, beVerbose=False)

    nStride, mStride = stride

    def crawl(folder, i=0, j=0, recurseLevel=0, counter=0) -> (int, (int, int)):
        """ AQUI EH O DEMONIO TERRA PRA HOMEM NUNHUM """

        if recurseLevel == dataRecurseLevel: # o mesmo que if folder[-5:] == '.oscs'
            splitFolder = folder[:-5].split('/')[1:]
            for split in splitFolder:
                key, valRaw = split.split('=')
                val = literal_eval(valRaw)
                metaDataContainer.add(key, val)

            metaDataContainer.add('fileName', folder)
            metaDataContainer.add('loc', (int(i/nStride)-1, int(j/mStride)-1))
            #print((int(i/nStride), int(j/mStride), "   ---   ", ))

            counter += 1
            print('\rFound ' + str(counter), "snapshot files.", end='', flush=True)

        dirList = sorted(glob(folder + '/*'))
        for folder in dirList:
            if recurseLevel == vParam1RecurseLevel-1: i+=1
            elif recurseLevel == vParam2RecurseLevel-1: j+=1

            if i%nStride != 0 or j%mStride != 0: continue

            counter, (i, j) = crawl(folder, i, j, recurseLevel+1, counter)

        if recurseLevel == vParam1RecurseLevel-1: i=0
        elif recurseLevel == vParam2RecurseLevel-1: j=0
        return counter, (i, j)

    crawl(dataBaseRootDir)

    print()

    metaDataContainer.finish()

    print(metaDataContainer.shape)

    return metaDataContainer


def outputResults(results: List[EnergyCompute], metaDataContainer: SnapshotMetaDataContainer) -> None:
    uniqueParams = metaDataContainer.getUniqueParameters()
    vParams = metaDataContainer.vParams

    outputFileName = 'rad'
    for key in uniqueParams:
        val = uniqueParams[key]
        outputFileName += '-' + key + '=' + MetaData.getFormat(key).format(val)
    outputFileName += '.enmap'
    print("\nShape =", metaDataContainer.shape)
    print("Will save to " + outputFileName)


    with open(outputFileName, 'w') as output:
        output.write('{')
        for key in uniqueParams:
            val = uniqueParams[key]
            output.write("\"" + key + "\": " + MetaData.getFormat(key).format(val) + ', ')
        output.write("\"variableParams\": " + str(vParams))

        p1 = metaDataContainer.theMap[vParams[0]]
        p2 = metaDataContainer.theMap[vParams[1]]
        output.write(", \"shape\": " + str(metaDataContainer.shape) + ", "
                     + "\"" + vParams[0] + "\": " + str(tuple(p1)) + ", "
                     + "\"" + vParams[1] + "\": " + str(tuple(p2)))

        output.write(", \"energyParams\": (\"E_left\", \"E_right\", \"E_tot\", \"P_left\", \"P_right\", \"i\", \"j\")")
        output.write("}\n(")
        for energyResult in results:
            E_left = energyResult.E_left
            E_right = energyResult.E_right
            E_tot = energyResult.E_tot
            P_left = energyResult.P_left
            P_right = energyResult.P_right
            i, j = energyResult.metaData.loc
            output.write("(" + str(E_left) + "," + str(E_right) + "," + str(E_tot) + ","
                             + str(P_left) + "," + str(P_right) + "," + str(i) + "," + str(j) + "), ")
        output.write(')')

    print("Done outputting calculations.")

    global generateDebugInfo
    if generateDebugInfo:
        allSims = []
        xMins = []
        xMaxs = []
        n=0
        for energyResult in results:
            allSims.append(energyResult.fullSimData['phi'])

            xMin, xMax = energyResult.getLeftXLims()
            xMins.append(xMin)
            xMaxs.append(xMax)

            n+=1

        allSims = asarray(allSims)
        xMins = asarray(xMins)
        xMaxs = asarray(xMaxs)

        from matplotlib import pyplot
        from plot_single import log_abs #, log_abs_inv

        func = lambda x: log_abs(x, 1.e-4)
        #invFunc = lambda x: log_abs_inv(x, 1.e-4)
        allSims = func(allSims)

        L = results[0].fullSimData['L']
        pyplot.imshow(allSims, origin='lower', extent=(-L/2, L/2, 0, n))
        pyplot.plot(xMins, linspace(0, n, n), color='b', linestyle='--')
        pyplot.plot(xMaxs, linspace(0, n, n), color='b', linestyle='--')
        pyplot.plot(-xMins, linspace(0, n, n), color='g', linestyle='--')
        pyplot.plot(-xMaxs, linspace(0, n, n), color='g', linestyle='--')

        ax = pyplot.axes()
        ax.set_aspect(45/n)

        ax.set_xlim(-3*L/2, 3*L/2)

        pyplot.plot(())
        pyplot.show()


def ComputeEnergyMap(parser: argparse.ArgumentParser = None):

    if parser is None:
        parser = GET_ENERGY_CALC_ARGUMENTS(parser)

    args = parser.parse_args()
    vParams = args.variableParameters

    global generateDebugInfo
    generateDebugInfo = args.plotDebugMap

    #DirNameFormatter(args.snapshotsDBRootFolder, vParams)

    ### BEGIN PIPELINE
    metaDataContainer = crawlExistingData(args.snapshotsDBRootFolder, vParams,
                                          (args.nStride, args.mStride))
    results = _computeAllCases(metaDataContainer, args.threads)
    outputResults(results, metaDataContainer)
    ### END PIPELINE


