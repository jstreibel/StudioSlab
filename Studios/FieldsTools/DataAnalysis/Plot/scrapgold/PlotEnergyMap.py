import argparse
import glob
from numpy import asarray, unique, linspace, zeros, full, sqrt
from ast import literal_eval as lit_eval
from Plot.scrapgold import plot_single
from matplotlib import pyplot

from Utils import utils


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

    #myGroup.add_argument('--calc', help="The lambda function to plot." type=str, default=)

    return parser


class SingleResult:
    def __init__(self, tinyMap: dict):
        self._tinyMap = tinyMap
        for key in tinyMap:
            setattr(self, key, tinyMap[key])
        self._loc = tinyMap['i'], tinyMap['j']

    @property
    def loc(self):
        return self._loc


class EnergyCalcResults:
    def __init__(self, fileName: str):
        results = []
        with open(fileName) as file:
            baseMap = lit_eval(file.readline())
            energyValues = lit_eval(file.readline())

            enParams = baseMap['energyParams']
            print("\nThe available energy params are", enParams)

            vParams = baseMap['variableParams']
            self._vParams = vParams
            p1_v = asarray(baseMap[vParams[0]])
            p2_v = asarray(baseMap[vParams[1]])

            nEnParams = len(enParams)

            for enParamsValues in energyValues:
                tinyMap = {}
                for l in range(nEnParams):
                    tinyMap[enParams[l]] = enParamsValues[l]

                results.append(SingleResult(tinyMap))

        self._p1_v = p1_v
        self._p2_v = p2_v
        self._results = results
        self._baseMap = baseMap


    def buildGridToPlot(self):
        shape = self._baseMap['shape']

        ELeftGrid = zeros(shape)
        ERightGrid = zeros(shape)
        ETotGrid = zeros(shape)
        PLeftGrid = zeros(shape)
        PLeftAnalyticGrid = zeros(shape)
        PRightGrid = zeros(shape)
        PRightAnalyticGrid = zeros(shape)
        EAnalyticGrid = zeros(shape)

        def getu(i: int, j: int) -> float:
            u_ = self._baseMap['V']
            if 'V' in self._vParams:
                if self._vParams[0] == 'V':
                    u_ = u_[i]
                elif self._vParams[1] == 'V':
                    u_ = u_[j]

            return u_

        def calcE(u_: float) -> float:
            return 2./(24.*(sqrt(1-u_**2)))

        def calcP(u_: float) -> float:
            return u_/(24.*sqrt(1.-u_**2))

        for point in self._results:
            i, j = point.loc

            u = getu(i, j)
            E = calcE(u)
            PLeft = calcP(u)

            ELeftGrid[i][j] = point.E_left
            ERightGrid[i][j] = point.E_right
            ETotGrid[i][j] = point.E_tot

            PLeftGrid[i][j] = point.P_left
            PLeftAnalyticGrid[i][j] = PLeft
            PRightGrid[i][j] = point.P_right
            PRightAnalyticGrid[i][j] = -PLeft

            EAnalyticGrid[i][j] = calcE(u)

        return ELeftGrid.T, ERightGrid.T, ETotGrid.T, PLeftGrid.T, PLeftAnalyticGrid.T,\
                                                      PRightGrid.T, PRightAnalyticGrid.T, EAnalyticGrid.T

    def getExtents(self):
        p1 = self._p1_v
        p2 = self._p2_v
        return p1.min(), p1.max(), p2.min(), p2.max()

    def getLabel(self, param):
        labels = {"v1": "$v_L$", "v2": "$v_R$",
                  "a1": "$\\alpha_L$", "a2": '$\\alpha_R$',
                  "V": "$V$", "l": "$\\lambda$"}
        return labels[param]

    @property
    def p1Label(self):
        return self.getLabel(self._vParams[0])
    @property
    def p2Label(self):
        return self.getLabel(self._vParams[1])

    def __getitem__(self, item: int):
        return self._results[item]


def PlotEnergyMap(results: EnergyCalcResults, outputFileName: str,
                  logEps: float = 1.e-5, showEndResult: bool = False) -> None:


    ELeftGrid, ERightGrid, ETotGrid, PLeftGrid, PLeftAnalyticGrid, PRightGrid, PRightAnalyticGrid, EAnalyticGrid = results.buildGridToPlot()

    #numericalError = 1.-EAnalyticGrid/ETotGrid
    #leftOscBalance = 1.-2*ELeftGrid/ETotGrid
    #rightOscBalance = 1.-2*ERightGrid/ETotGrid
    #totalConvertedToRadiation = 1.-(ELeftGrid+ERightGrid)/ETotGrid # = 0.5*(leftOscBalance+rightOscBalance)
    #totalConvertedToRadiation_ANALYTIC_COMP = 1.-(ELeftGrid+ERightGrid)/EAnalyticGrid # = 0.5*(leftOscBalance+rightOscBalance)

    # Multiply by 100 to turn into percentage
    #theGrid = 100. * totalConvertedToRadiation

    radiationMomentum = - (PLeftGrid + PRightGrid)
    percentLeftMomentum = PLeftGrid/PLeftAnalyticGrid
    theGrid = PLeftGrid/PLeftAnalyticGrid

    fig = pyplot.figure()
    ax = pyplot.axes()

    seq_cmaps = ('viridis', 'afmhot', 'cividis', 'inferno', 'magma', 'plasma', 'PuBu', 'copper')
    div_cmaps = ('Spectral', 'PuOr')

    #cmap = seq_cmaps[3]
    #cmap = seq_cmaps[1]
    cmap = div_cmaps[1] + '_r'

    TRANSF_FUNC=1
    func = (lambda x: plot_single.log_abs(x, logEps), lambda x: x)[TRANSF_FUNC]
    invFunc = (lambda x: plot_single.log_abs_inv(x, logEps), lambda x: 1./x)[TRANSF_FUNC]
    theGrid = func(theGrid)

    extent = results.getExtents()
    if 0:
        vRange = max((abs(theGrid.max()), abs(theGrid.min())))
        vmax = vRange
        vmin = -vRange
    else:
        vmin = theGrid.min()
        vmax = theGrid.max()
    im = ax.imshow(theGrid, origin='lower', extent=extent, cmap=cmap, vmin=vmin, vmax=vmax) #, interpolation='quadric')

    region = None
    if region is None: region = extent
    ax.set_xlim(region[0], region[1])
    ax.set_ylim(region[2], region[3])

    if TRANSF_FUNC == 0:
        cbar = plot_single.generateLinearizedColorbar(ax, fig, im, vmin, vmax, func, invFunc)
        #cbar.ax.set_yticklabels([l.get_text()+"%" for l in cbar.ax.get_yticklabels()])
    else:
        cbar = fig.colorbar(im)

    ax.set_xlabel(results.p1Label)
    ax.set_ylabel(results.p2Label)

    fig.savefig(outputFileName, bbox_inches='tight', dpi=300)

    if showEndResult:
        pyplot.show()


def PlotEnergyMapFromUserInput(parser: argparse.ArgumentParser) -> None:
    args = parser.parse_args()

    files = glob.glob(args.file)
    files.sort()

    for fileName in files:
        if args.file[-6:] != '.enmap':
            continue

        print("Started " + fileName + "... ", end='', flush=True)

        try:
            results = EnergyCalcResults(fileName)

            plotFileName = fileName[:-6] + '.' + args.ext
            PlotEnergyMap(results, plotFileName, args.logEps, args.showEndResult)
        except Exception:
            print("Error: " + utils.FAIL + Exception.args + utils.bcolors.ENDC)
            continue

        print(utils.OKGREEN + "Done!" + utils.bcolors.ENDC)


