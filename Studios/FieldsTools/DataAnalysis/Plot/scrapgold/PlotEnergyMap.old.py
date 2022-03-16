#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
import glob
from numpy import asarray, zeros, sqrt
from ast import literal_eval as lit_eval
import plot_single
from typing import List
from matplotlib import pyplot

from open_sim import OpenSimData
import utils

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


def GET_PLOTTING_ARGUMENTS(parser: argparse.ArgumentParser = None) -> argparse.ArgumentParser:
    if parser is None:
        parser = argparse.ArgumentParser()

    myGroup = parser.add_argument_group("Plotting")

    myGroup.add_argument('--file', help="Input file of extension .enmap.", type=str, default='./*.enmap')

    myGroup.add_argument('--ext', help="Output plot file extension. Defaults to .pdf. Can be anything supported by "
                                      "matplotlib.", type=str, default='pdf')
    myGroup.add_argument('--logEps', '-e', help="When using log scale, formula is sign(phi)*log(abs(phi)/eps + 1), "
                                                "where epsilon determines the amount of detail near 0 (zero) to reveal. "
                                                "Smaller epsilon reveals more detail. Default is 1.e-6.", type=float,
                                                default=1.e0)
    myGroup.add_argument('--showEndResult', '-se', help="Flag to tell program to show each plot in the end.",
                         action='store_true')

    return parser


def buildGridToPlot(result):
    shape = result[0].params.shape

    ELeftGrid = zeros(shape)
    ERightGrid = zeros(shape)
    ETotGrid = zeros(shape)

    for energyOutput in result:
        i, j = energyOutput.params.loc

        ELeftGrid[i][j] = energyOutput.E_left
        ERightGrid[i][j] = energyOutput.E_right
        ETotGrid[i][j] = energyOutput.E_tot

    return ELeftGrid, ERightGrid, ETotGrid


def _extractExtent(results: List[EnergyCompute]) -> (float, float, float, float):
    a1_v = []
    a2_v = []
    for r in results:
        a1_v.append(r.params.a1)
        a2_v.append(r.params.a2)
    a1_v = asarray(a1_v)
    a2_v = asarray(a2_v)

    return a1_v.min(), a1_v.max(), a2_v.min(), a2_v.max()


def PlotEnergyMap(results: List[EnergyCompute], outputFileName: str, logEps: float = 1.e-5, showEndResult: bool = False) -> None:
    shape = results[0].params.shape

    ELeftGrid, ERightGrid, ETotGrid = buildGridToPlot(results)

    theGrid = 1.-(ELeftGrid+ERightGrid)/ETotGrid
    #theGrid = 1-.5*ELeftGrid/ETotGrid
    #theGrid = 1.-ERightGrid/(ETotGrid/2.)

    theGrid *= 100.0

    if shape[0] == 1 or shape[1] == 1:
        print("% energia radiada")
        print(theGrid)
        return

    fig = pyplot.figure()
    ax = pyplot.axes()

    cmap = ('viridis', 'afmhot', 'cividis', 'inferno', 'magma', 'plasma', 'PuBu_r', 'copper')[1] + '_r'
    cmap = 'Spectral_r'

    func = (lambda x: plot_single.log_abs(x, logEps), lambda x: x)[0]
    invFunc = (lambda x: plot_single.log_abs_inv(x, logEps), lambda x: 1. / x)[0]
    theGrid = func(theGrid)

    extent = _extractExtent(results)
    im = ax.imshow(theGrid, origin='lower', extent=extent, cmap=cmap, interpolation='quadric')

    region = None
    if region is None: region = extent
    ax.set_xlim(region[0], region[1])
    ax.set_ylim(region[2], region[3])

    cbar = plot_single.generateLinearizedColorbar(ax, fig, im, theGrid.min(), theGrid.max(), func, invFunc)
    cbar.ax.set_yticklabels([l.get_text()+"%" for l in cbar.ax.get_yticklabels()])

    linewidth = 0.6
    ax.plot((0,1), (0,1), linestyle='--', color='k', linewidth=linewidth)
    ax.plot((0,.5), (.5,1), linestyle='-.', color='w', linewidth=linewidth)
    ax.plot((.5,1), (0,.5), linestyle='-.', color='w', linewidth=linewidth)

    ax.set_xlabel("$\\alpha_L$")
    ax.set_ylabel("$\\alpha_R$")

    fig.savefig(outputFileName, bbox_inches='tight', dpi=300)

    if showEndResult:
        pyplot.show()


def PlotEnergyMapFromUserInput(parser: argparse.ArgumentParser):
    args = parser.parse_args()

    files = glob.glob(args.file)
    files.sort()

    for fileName in files:
        if args.file[-6:] != '.enmap':
            continue

        print("Started " + fileName + "... ", end='', flush=True)

        try:
            results = []
            with open(fileName) as file:
                file.readline()  # COMENTARIO NO CABECALHO
                v1, v2, L, u, N, t = lit_eval(file.readline())
                shape = lit_eval(file.readline())
                a1_v = asarray(lit_eval(file.readline()))
                a2_v = asarray(lit_eval(file.readline()))

                rawTypeEnergies = lit_eval(file.readline())
                for (E_left, E_right, E_tot, i, j) in rawTypeEnergies:
                    reducedParams = ReducedParameters(a1_v[i], a2_v[j], (i, j), shape, u, fileName)
                    energyCompute = EnergyCompute(reducedParams, (E_left, E_right, E_tot))

                    results.append(energyCompute)

            plotFileName = fileName[:-6] + '.' + args.ext

            PlotEnergyMap(results, plotFileName, args.logEps, args.showEndResult)
        except Exception:
            print("Error: " + utils.FAIL + Exception.args + utils.bcolors.ENDC)
            continue

        print(utils.OKGREEN + "Done!" + utils.bcolors.ENDC)


if __name__ == "__main__":
    parser = GET_PLOTTING_ARGUMENTS()
    PlotEnergyMapFromUserInput(parser)
