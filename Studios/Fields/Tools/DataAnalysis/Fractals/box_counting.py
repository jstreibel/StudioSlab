from pointset import *

# This import registers the 3D projection, but is otherwise unused.
from mpl_toolkits.mplot3d import Axes3D  # noqa: F401 unused import
from matplotlib import pyplot

from numpy import arange, delete, log, log10, sin, cos, pi

from Fractals.FracCalculator import *


def showBoxCountingInfo(pointset, ax1, ax21, ax22, epsMin = 1.e-6, epsMax = 0.1, n_divs = 90):
    p = pointset
    points, phaseTrans, analyticDim, analyticDimStr, pointsetName, pointsetDescr = p.getPoints(), p.getExpectedPhaseTransitions(), p.getAnalyticDimension(), p.getAnalyticDimensionStr(), p.getName(), p.getDescription()

    #calc = PyFracCalculator(pointset)
    calc = CFracCalculator(pointset)

    if 1:
        eps_list = []
        d_list = []

        eps_list = logspace(log10(epsMin),log10(epsMax), n_divs)
        for eps in eps_list:
            d_list.append(calc.dim(eps))
            print("eps =", eps)
        d_list = asarray(d_list)

        ax1.scatter(eps_list, d_list, label=pointsetDescr, marker=",")
        y = (d_list.min(), d_list.max())
        for trans in phaseTrans:
            x = (trans, trans)
            ax1.plot(x, y)
        ax1.plot((eps_list.min(), eps_list.max()), (analyticDim  , analyticDim), label="$dim_{"+pointsetName+"} = " + analyticDimStr + "$ (analitico)")

        ax1.set_xscale('log')

        ax1.set_xlabel("$\epsilon$")
        ax1.set_ylabel("$dim_b(set)=\dfrac{log(N(\epsilon))}{log(1/\epsilon)}$")

        ax1.grid()
        ax1.legend()

    if 1:
        inv_eps_list = linspace(1./epsMax, 1./epsMin, n_divs)
        print("inv_eps_min =",1./epsMax, "inv_eps_max =", 0.25/epsMin)
        N_list = []
        for inv_eps in inv_eps_list:
            eps = 1./inv_eps
            N_list.append(calc.N(eps))
            print("inv_eps =", inv_eps, "  --  ( eps =", eps, ")")
        N_list = asarray(N_list)

        ax21.scatter(inv_eps_list, N_list, marker=",")


        y = (N_list.min(), N_list.max())
        for eps in phaseTrans:
            x = (1./eps, 1./eps)
            ax21.plot(x, y)
            ax22.plot(log(x), log(y))


        inv_eps_list = logspace(log10(1./epsMax), log10(1./epsMin), n_divs)
        N_list = []
        for inv_eps in inv_eps_list:
            eps = 1./inv_eps
            N_list.append(calc.N(eps))
            print("inv_eps =", inv_eps)
        N_list = asarray(N_list)

        ax22.scatter(log(inv_eps_list), log(N_list), marker=",")

        ax21.set_xlabel("$\\dfrac{1}{\\epsilon}$")
        ax21.set_ylabel("$N(\\epsilon)$")
        ax22.set_xlabel("$\\log(1/\\epsilon)$")
        ax22.set_ylabel("$\\log(N(\\epsilon))$")

    print("\n\tAs linhas verticais marcam as \"transições de fase\" esperadas para o conjunto de pontos, isto é: as escalas à partir das quais esperamos que o algoritmo da box-counting-dimension (BCD) comece a computar dimensões fractais diferentes para o conjunto de pontos.\n\n"
            "\tPor exemplo, para um conjunto de pontos disposto de forma razoavelmente regular, à partir do momento em que a distância entre os pontos for maior do que o tamanho das caixas, efetivamente o que se estará medindo é a dimensão de um conjunto contável de pontos, cuja dimensão Hausdorf é zero.\n\n"
            "\tOutro exemplo é o de uma linha (1-d) em espiral regular formando um objeto semelhante à um tubo. À uma distância muito grande (uma escala muito grande das caixas / muito pequena do conj. de pontos), o tubo deve parecer-se com uma linha reta, também 1-d. À uma distância um pouco mais curta, o tubo deve parecer-se com uma superfície 2-d. Ainda mais próximo, estaremos medindo a dimensão da linha, 1-d. E, mais próximo ainda, dos pontos da linha que, se gerados computacionalmente, devem resultar em 0-d.\n\n"
            "\tNos gráficos, o \\varepsilon refere-se à escala das caixas (boxes) do algoritmo que mede a dimensão do conjunto. Variar \\varepsilon é essencialmente o mesmo que reescalar o conjunto por um fator 1/\\varepsilon. Como a dimensão Hausdorf (aproximada) do conjunto será o expoente de como a variação na escala do conjunto afeta o numero de caixas dentro das quais pelo menos um ponto do conjunto caiu, então as diferentes \"fases\" do conjunto devem aparecer como retas distintas no gráfico log(N) em função de log(1/\\varepsilon).")


def showPointSet(pointset, ax3d):
    from matplotlib import pyplot
    import matplotlib.patches as mpatches
    from matplotlib.collections import PatchCollection
    import numpy as np

    points = pointset.getPoints()
    x, y, z = points[:,0], points[:,1], points[:,2]

    if 1:
        ax3d.scatter(x, y, z, marker='.', label='Koch', s=1)
    else:
        boxes = []
        for key in pointdict:
            i, j = key
            x = i*eps
            y = j*eps

            sqr = mpatches.Rectangle((x, y), eps, eps, fill=False)
            boxes.append(sqr)

        colors = 100*np.random.rand(len(boxes))
        collection = PatchCollection(boxes)
        collection.set_array(colors)
        fig, ax = pyplot.subplots()
        ax.add_collection(collection)

    pyplot.xlim(x.min(), x.max())
    pyplot.ylim(y.min(), y.max())
    pyplot.show()


if __name__ == "__main__":
    from numpy import logspace, linspace, asarray

    fig = pyplot.figure()
    pointset = None

    epsMin = (1.e-3)/3.
    epsMax = 0.01

    if 1: # KOCH
        pointset = KochPointSet(koch_order=6)
    elif 1: # PLANO
        n = 100
        m = 100
        baseamp = 0.100
        def func(u,v):
            z = 0.1
            k = 100
            z = z + baseamp/k*sin(2*pi*k*u)
            return u, v, z

        pointset = TwoParamPointSet(n, m, func=func)
        epsMin = 0.1/n
        epsMax = 1.e-1
    elif 1: # TUBO
        n = 10000
        amp = 0.00025*0
        om = 100*2*pi
        s = 1.
        func = lambda t: (s*t, amp*sin(om*s*t), amp*cos(om*s*t))
        pointset = OneParamPointSet(n, func)

        phaseTrans = 2.*pi / om
        pointset.addPhaseTransition(phaseTrans)

        epsMin = 0.1/n
        epsMax = 1.1*   phaseTrans

        print(epsMin, epsMax)

    print("Conj. de pontos tem", len(pointset.getPoints()), "pontos.")

    showBoxCountingInfo(pointset, fig.add_subplot(221), \
                                  fig.add_subplot(223), fig.add_subplot(224), \
                                  epsMin = epsMin, epsMax = epsMax, n_divs = 50)
    showPointSet(pointset, fig.add_subplot(222, projection='3d'))

    pyplot.show()
