import matplotlib.pyplot as pyplot
from matplotlib import rcParams, colors

import numpy as np
from matplotlib.widgets import Slider  # , Button, RadioButtons

import scipy.optimize as optimize

from Fitting import fit_shockwave
# from fitting import Region
from Utils import utils

f = '{:.2e}'.format
ff = '{:.2f}'.format
fmt = lambda x, n: ('{:.' + str(n) + 'f}').format(x)

cmaps = ['ocean', 'coolwarm', 'PuBu', 'copper', 'bone', 'jet', 'CMRmap',
         'Blues_r', 'magma', 'RdBu', 'jet', 'terrain']
cmap = cmaps[2]

grad = np.gradient


def log_abs(phi, epsilon):
    return np.log(np.abs(phi) / epsilon + 1) * np.sign(phi)


def log_abs_inv(phi_t, epsilon):
    phi = phi_t
    return epsilon * (np.exp(np.abs(phi)) - 1) * np.sign(phi)


# class LogAbs(colors.Normalize):
#    def __init__(self, epsilon, vmin, vmax):
#        self.epsilon = epsilon
#
#        self.vmin_log = log_abs(vmin, epsilon)
#        self.vmax_log = log_abs(vmax, epsilon)
#        self.range_log = self.vmax_log-self.vmin_log
#
#        colors.Normalize.__init__(self, vmin, vmax, clip=False)
#
#    def __call__(self, value, clip=None):
#        newval = log_abs(value, self.epsilon)
#
#        normval = (newval - self.vmin_log)/self.range_log
#
#        return np.ma.masked_array(normval)

def getZeros(V, v, alpha, tStart, tEnd, n=0, subdivs=256):
    gamma = 1. / np.sqrt(1. - v * v)

    def ab(t):
        if t >= -1. and t < -.5:
            return 1., 1.
        elif t >= -.5 and t < 0:
            return -1., 0.
        elif t >= 0 and t < .5:
            return 1., 0.
        elif t >= .5 and t < 1.:
            return -1., 1.
        return None

    a, b = ab(t)
    x_0 = gamma / (1. + a(t) * v * V) * ((1. - V * V) * (1. + v * b) + alpha * (V + v * a))
    B = gamma / (2. * v) * (2. * alpha + n / (gamma * gamma)) - x_0(alpha)

    return np.linspace(tStart, tEnd, subdivs) / V + B  # literalmente t/V + B^n (\alpha)


def showVertex(sim_result, ax, threshold=1.e-5):
    V = sim_result['V']
    t_ = np.sqrt((1. - V) / (1. + V))
    x_ = -t_

    # vertice analitico
    ax.scatter(x_, t_, marker='+', s=1.0, c='b')

    phi = sim_result['phi']
    m, n = phi.shape  # (m linhas, n colunas)

    n = sim_result['outresX']
    m = sim_result['outresT']
    t0 = sim_result['initTime']
    tf = sim_result['T']
    L = sim_result['L']
    dot_i = int((t_ - t0) / (tf - t0) * m)
    dot_j = int((x_ + L / 2.) / L * n)

    delta_x = 0.1
    delta_t = 0.1
    n_f = delta_x / L * n
    m_f = delta_t / (tf - t0) * m
    n_ = int(n_f)
    m_ = int(m_f)
    offset_x = int(m_f / 2)
    offset_t = int(n_f / 2)
    arr = np.empty([m_, n_])
    for i in range(m_):
        for j in range(n_):
            arr[i, j] = phi[dot_i + i - offset_x, dot_j + j - offset_t]

    arrOriginal = log_abs(arr, 1.e-6)
    for i in range(m_):
        for j in range(n_):
            if np.abs(arr[i, j]) < threshold:
                arr[i, j] = 0
            else:
                arr[i, j] = 1.

    arr = np.abs(grad(arr, 1)[1])

    e = (x_ - delta_x / 2, x_ + delta_x / 2, t_ - delta_t / 2, t_ + delta_t / 2)
    x__ = []
    t__ = []
    pts = []
    hPixelSize = .5 * delta_x / n_f  # isso eh pra ser igual a delta_t/m_f
    for i in range(1, m_ - 1):
        for j in range(1, n_ - 1):
            if arr[i, j] > 0:
                if arr[i + 1, j] == 0 and arr[i - 1, j] == 0 and arr[i, j + 1] == 0 and arr[i, j - 1] == 0:
                    arr[i, j] = 0
                else:
                    _x = float(i) / m_f * delta_x - e[0] - delta_x + hPixelSize
                    _t = float(j) / n_f * delta_t - e[2] - delta_t + hPixelSize
                    x__.append(_x)
                    t__.append(_t)
                    pts.append((_x, _t))
    x__ = np.asarray(x__)
    t__ = np.asarray(t__)
    pts = np.asarray(pts)

    class Vfit(object):
        def __init__(self, pointSet):
            self.pointSet = pointSet
            self._tries = []

        def V(self, x, x_0, a, b):
            return a * np.abs(x - x_0) + b

        def __call__(self, params):
            pts = self.pointSet
            x_0, b = params
            a = -V

            val = 0.
            for p in pts:
                x, t = p
                Vt = self.V(x, x_0, a, b)
                dif = t - Vt
                val += dif * dif

            self._tries.append(params)
            return val

    func = Vfit(pts)
    initGuess = (-x_, -t_)  # x_0, b
    bounds = (-x_ - delta_x / 2, -x_ + delta_x / 2), (-t_ - delta_t / 2, -t_ + delta_t / 2)
    methods = 'Nelder-Mead', 'Powell'
    result = optimize.minimize(func, initGuess, bounds=bounds)
    x_0, b = result.x
    a = -V
    V_ = lambda x, x_0, a, b: a * np.abs(x - x_0) + b

    if 0:  # mostrar resultado do fit
        Vt = np.linspace(e[2], e[3], 100)
        Vx = V_(Vt, x_0, a, b)
        ax.scatter(Vx, Vt, marker=',', s=1.0, c='r')
    if 0:  # mostrar todas as tentativas de encontrar parametros do 'V(x)'
        # Vt = np.linspace(e[2], e[3], 100)
        Vt = np.linspace(0.25, 0.75, 100)
        for try_ in func._tries:
            x_0, b = try_
            Vx = V_(Vt, x_0, a, b)
            ax.scatter(Vx, Vt, marker=',', s=1.0, c='r', alpha=0.2)

    im = ax.imshow(arrOriginal, extent=e, origin='lower', interpolation='none')
    if 0:  # mostrar o "V" da borda analitico (a` partir de $\sqrt{1-V}/\sqrt{1+V})$
        V_ = lambda x, x_0, a, b: a * np.abs(x - x_0) + b
        Vt = np.linspace(e[2], e[3], 100)
        Vx = V_(Vt, x_0=-x_, a=-V, b=-t_)
        ax.scatter(Vx, Vt, marker='+', s=1.0, c='g')

    if 0:  # mostrar os pontos inferidos da imagem
        ax.scatter(t__, x__, marker='+', s=1.0, c='k')

    if 1:  # mostrar ponto analitico
        ax.scatter(b, x_0, marker='+', s=1.0, c='b')
        f = open("numeric.txt", "a+")
        f5 = '{:.7f}'.format
        f10 = '{:.10f}'.format
        sep = "  "
        linetext = f5(V) + sep + f10(b) + sep + f10(x_0) + "\n"
        f.write(linetext)
        f.close()


def generateLinearizedColorbar(ax, fig, im, vmin, vmax, func, invFunc, fontsize=None):
    from mpl_toolkits.axes_grid1 import make_axes_locatable
    from utils import round_sd

    divider = make_axes_locatable(ax)
    cax = divider.append_axes('right', size='5%', pad=0.05)  # colocar o colorbar a direita e do tamanho certinho.

    vmin_lin, vmax_lin = invFunc(np.asarray((vmin, vmax)))
    boundaries = [func(v) for v in np.linspace(vmin_lin, vmax_lin, 1000)]

    linTicksMin, linTicksMax, linTicksStep = round_sd(vmin_lin * 1.5), \
                                             round_sd(vmax_lin * 1.5), \
                                             round_sd(1.5 * (vmax_lin - vmin_lin) / 8)
    if linTicksMin < 0:
        arr1 = np.arange(-linTicksStep, linTicksMin, -linTicksStep)
        arr2 = np.arange(0, linTicksMax, linTicksStep)
        linTicks = np.concatenate((arr1, arr2))
    else:
        linTicks = np.arange(linTicksMin, linTicksMax, linTicksStep)
    ticks = func(linTicks)

    # THE COLORBAR
    cbar = fig.colorbar(im, cax, boundaries=boundaries, ticks=ticks)

    # THE LABELS....
    if 0:
        labels = [round_sd(v * (vmax_lin - vmin_lin) + vmin_lin, 2) for v in cbar.ax.get_yticks()]
        cutoffThreshold = 1.e-7  # este valor determina se usamos notacao cientifica ou ponto.
        labels = [l if not np.abs(l) < cutoffThreshold else 0 for l in labels]
        cbar.ax.set_yticklabels(labels)
        if fontsize is not None:
            cbar.ax.tick_params(labelsize=fontsize)
        else:
            cbar.ax.tick_params()
    elif len(cbar.ax.get_yticks()) > 0:
        ''' Esse if eh para botar bandeide num problema cuja origem desconheco. Ocorre que por algum motivo o colorbar
            nao esta colocando os ticks na barra de cor. '''

        def cutoffVal(yticks):
            ''' Este valor determina se usamos notacao cientifica ou ponto. '''
            vals = np.asarray([round_sd(v * (vmax_lin - vmin_lin) + vmin_lin, 2) for v in yticks])
            lvals = np.log10(np.abs(vals))
            vals[np.argmin(lvals)] = 0.0
            return vals

        labels = cutoffVal(cbar.ax.get_yticks())
        cbar.ax.set_yticklabels(labels)
        if fontsize is not None:
            cbar.ax.tick_params(labelsize=fontsize)
        else:
            cbar.ax.tick_params()

    return cbar


def plot(sim_result, fit_params=None, args=None, showLog=True, logEpsilon=1.e-6, showResult=False, \
         show_colorbar=False, extension='.png', fontsize=10, fontsize_ticks=8, dpi=300, \
         imgWidthInches=5, region=None, annotationPosition=(0.25, 0.25), \
         annotations=None, annotationColor='k', outputFileName=None, xlabel='$x$', ylabel='$t$',
         saveToFile=True, closeAfterUse=True, cmap=None, showZeros=False,
         showBorderLightCone=False, showBorderConeVertex=False, showEnergyDens=False,
         valueRange=(0, 0., 0.), noframe=False, labelroom=False, draw_box=None, fig_ax=None, \
         folded=False, verbose=True):
    if args is not None:
        ''' Aqui vamos colocando tudo o que for novo. '''
        showSelfSimilar = args.showselfsimilar

    if cmap is None:
        if showEnergyDens is True:
            cmap = 'afmhot_r'
        else:
            cmap = 'BrBG'

    print("Using cmap \'" + cmap + "\'")

    rcParams.update({'font.size': fontsize})

    sr = sim_result

    xLeft, L, T, initT = sr['xLeft'], sr['L'], sr['T'], sr['initTime']
    phi = None
    if region is None:
        phi = sr['phi']
    else:
        # NOTA 1
        # O que era feito anteriormente era plotar tudo (todo o extent)  e depois so salvar a regiao (region).
        # Agora, o que fazemos eh fazer de tudo uma coisa so. Nao sei no que pode dar...
        phi = sr.getRegion(region[0], region[1], region[2], region[3])
    if 0: print("Region full resolution is " + str(phi.shape[1]) + "x" + str(phi.shape[0]))

    fontsize_small = fontsize_ticks

    if folded:  # TA ERRADO?
        Hn = lambda eta, n: np.heaviside(eta - 4. * n + 2, 0.5) - np.heaviside(eta - 4. * n - 2, 0.5)

        def eta_bar(eta, n):
            val = np.zeros(eta.shape)

            for i in range(-n, n + 1):
                val = val + np.abs(eta - 4. * n) * Hn(eta, n)

            return val

        phi = eta_bar(phi, 2)

    if showEnergyDens:
        nx, nt = sr.getItems(('outresX', 'outresT'))
        h1, h2 = T / nt, L / nx
        ddt = lambda phi: np.gradient(phi, h1, axis=0)
        ddx = lambda phi: np.gradient(phi, h2, axis=1)
        dphidt = ddt(phi)
        dphidx = ddx(phi)

        if 1:
            V = np.abs(phi)  # Nonperturbed potential
        else:
            if verbose: print(utils.WARNING + "USANDO POTENCIAL PERTURBADO PARA CALCULAR ENERGIA." + utils.ENDC)

            def VPert(eta):
                UnitStep = lambda arg: np.heaviside(arg, 0.5)
                Hn = lambda arg: UnitStep(arg + 2.0) - UnitStep(arg - 2.0)

                V = np.zeros(eta.shape)
                for n in range(-2, 3):
                    arg = eta - 4.0 * float(n)
                    # arg = np.full(phi.shape, 2.05) - 4.0*float(n)
                    V = V + (np.abs(arg) - 0.5 * arg ** 2) * Hn(arg)

                return V

            V = VPert(phi)

        phi = .5 * dphidt ** 2 + .5 * dphidx ** 2 + V
        sim_result['phi'] = phi

    if fig_ax is None:
        fig_ax = pyplot.subplots()
    fig, ax = fig_ax

    pyplot.xlabel(xlabel)
    pyplot.ylabel(ylabel, rotation=0)

    field = phi

    if region is None:
        # extent = -L*0.5, L*0.5, initT, T
        extent = xLeft, xLeft + L, initT, T
        region = extent
    else:
        extent = region  # Vide 'NOTA 1' acima
    vmin, vmax = None, None

    # Mostrar os valores do campo em escala log. A barra de cor segue linear (trabalhoso...).
    if showLog:
        field = log_abs(field, logEpsilon)

    if valueRange[0] == 0:  # auto
        vmin = field.min()
        vmax = field.max()
    elif valueRange[0] == 1:
        vmin = field.min()
        vmax = np.abs(vmin)
    elif valueRange[0] == 2:
        vmax = field.max()
        vmin = -np.abs(vmax)
    elif valueRange[0] == 3:
        vmin = log_abs(valueRange[1], logEpsilon)
        vmax = log_abs(valueRange[2], logEpsilon)

    if 0:  # COOL SHADING!!!
        from matplotlib.colors import LightSource
        ls = LightSource(azdeg=15, altdeg=15)

        if 1:
            field = ls.hillshade(field, vert_exag=1)
            cmap = 'gray'
            # field = ls.shade(field, vert_exag=1, cmap=cmap, blend_mode='hsv')
            im = ax.imshow(field, extent=extent, cmap=cmap, origin='lower', interpolation='quadric',
                           vmin=vmin, vmax=vmax)
        else:
            rgb = ls.shade(field, cmap=pyplot.cm.copper, vert_exag=1, blend_mode=('overlay', 'hsv')[1])
            im = ax.imshow(rgb, extent=extent, origin='lower', interpolation='quadric')

    else:
        im = ax.imshow(field, extent=extent, cmap=cmap, origin='lower', interpolation='quadric',
                       vmin=vmin, vmax=vmax)

    if draw_box is not None:
        if type(draw_box) in (tuple, list):
            xmin, xmax, tmin, tmax = draw_box
            ax.plot((xmin, xmax), (tmin, tmin), linewidth=0.5, color='black', linestyle='dashed')
            ax.plot((xmax, xmax), (tmin, tmax), linewidth=0.5, color='black', linestyle='dashed')
            ax.plot((xmax, xmin), (tmax, tmax), linewidth=0.5, color='black', linestyle='dashed')
            ax.plot((xmin, xmin), (tmax, tmin), linewidth=0.5, color='black', linestyle='dashed')
        elif isinstance(draw_box, Region):
            ax.add_patch(draw_box.getPlotPolygon(color='k'))
        else:
            print('Unknown box type', type(draw_box))

    # Se a variavel 'eps' estiver presente na simulacao 'e sinal que se trata de
    # perturbacao, e mostramos a parte analitica dentro da regiao pontilhada,
    # a regiao de dissipacao de energia e colocamos informacao extra no nome do
    # arquivo (sobre o interval de tempo sendo mostrado, ja que eh comum nesses
    # casos nao mostrarmos tudo).
    if 'eps' in sr:
        eps = sr['eps']
        l = sr['l']
        if showSelfSimilar:
            if eps > 0.5:
                v_0 = 1. / eps - 1.

            else:
                v_0 = 1.

            x_0 = -.5 * l
            t__ = 0.5 * l / (1 + v_0)

            # Cone de luz, lado esquerdo
            ax.plot((0, -t__), (0, t__), linewidth=0.5, color=annotationColor, linestyle='dashed')
            # Limite superior da auto-similar, lado esquerdo
            ax.plot((x_0, x_0 + v_0 * t__), (0, t__), linewidth=0.5, color=annotationColor, linestyle='dashed')

            # Cone de luz, lado direito
            ax.plot((0, t__), (0, t__), linewidth=0.5, color=annotationColor, linestyle='dashed')
            # Limite superior da auto-similar, lado direito
            ax.plot((-x_0, -x_0 - v_0 * t__), (0, t__), linewidth=0.5, color=annotationColor, linestyle='dashed')

        if 'is_dissipating' in sr:
            is_diss = int(sr['is_dissipating'])
            if is_diss == 1:
                dist = sr['dissD']
                xLeft, xRight = -L / 2.0 + dist, L / 2.0 - dist
                xMin, xMax = region[:2]
                if xLeft > xMin: ax.plot((xLeft, xLeft), (0, T), linewidth=0.5, color=annotationColor,
                                         linestyle='dashed')
                if xRight < xMax: ax.plot((xRight, xRight), (0, T), linewidth=0.5, color=annotationColor,
                                          linestyle='dashed')

    if show_colorbar:
        linearize_cmap = showLog
        if linearize_cmap:
            func = lambda x: log_abs(x, logEpsilon)
            invFunc = lambda x: log_abs_inv(x, logEpsilon)
            generateLinearizedColorbar(ax, fig, im, vmin, vmax, func, invFunc, fontsize_small)
        else:
            fig.colorbar(im)

    # TODO: as linhas abaixo eram necessarias (talvez ainda venham a ser...) quando region e extent eram duas coisas direntes.
    # Elas agora, essencialmente, sao exatamente a mesma coisa. No funturo, podem passar a ser uma coisa so.
    # Procure por 'NOTA 1' acima.
    if region is not None:
        ax.set_xlim(region[0], region[1])
        ax.set_ylim(region[2], region[3])
    else:
        region = extent

    if showZeros:
        pass
    if showBorderLightCone:
        x = (0., -L * 0.25)
        y = (0., L * 0.25)
        ax.plot(x, y, color='k', linewidth=.1)
    if showBorderConeVertex:
        showVertex(sim_result, ax)

    if (annotations is not None and annotations != "") and annotationPosition[0] > 0 and annotationPosition[1] > 0:
        xy = (region[0] + annotationPosition[0] * (region[1] - region[0]), \
              region[2] + annotationPosition[1] * (region[3] - region[2]))
        strAnnotation = ""
        for a in annotations.split(','):
            if a[0] == '.':
                strAnnotation += a[1:] + '\n'
            else:
                a = [l.strip() for l in a.split('=')]
                val = None
                if a[0] in sim_result:
                    val = str(sim_result[a[0]])
                elif fit_params is not None:
                    if a[0] in fit_params:
                        val = f(fit_params[a[0]])
                        val_s = val.split('e')
                        val = val_s[0] + "\\times 10^{" + val_s[1] + "}"
                else:
                    print(utils.FAIL + "Can't annotate \'" + a[0] + "\': key not found in sim file." + utils.ENDC)
                    continue
                symbol = a[0]
                if len(a) > 1:
                    symbol = a[1]
                strAnnotation = strAnnotation + "$" + symbol + "=" + str(val) + "$\n"

        ax.annotate(strAnnotation, xy=xy, color=annotationColor, size=fontsize)

    ax.tick_params(labelsize=fontsize_small)
    if labelroom:
        ax.xaxis.set_major_locator(pyplot.MultipleLocator(np.floor(100 * 8 / 60 * np.ceil(L)) / 100.0))
        # ax.xaxis.set_major_locator(pyplot.MaxNLocator(6))
        ax.xaxis.set_label_coords(1.0, -0.04)
        ax.yaxis.set_major_locator(pyplot.MultipleLocator(np.floor(100 * 8 / 30 * np.ceil(T)) / 100.0))
        ax.yaxis.set_label_coords(-0.04, 0.95)

    if 0:
        test = np.linspace(-0.1, 0.1, 25)
        for v in test:
            t = log_abs(v, logEpsilon)
            inv = log_abs_inv(t, logEpsilon)
            print(v - inv)

    if fit_params is not None:
        T0, a0 = fit_params['T0'], fit_params['a0']
        a_k = fit_shockwave.get_ak(fit_params['n_curves'])
        for ak in a_k:
            ak *= a0
            xsi = np.arange(-L / 2, L / 2, L / 100)
            tau = fit_shockwave.tau_k(xsi, ak) + T0

            idxs = np.argwhere(tau <= T)
            xsi = xsi[idxs]
            tau = tau[idxs]
            fitLinesColor = fit_params['fitlinescolor']

            ax.plot(xsi, tau, fitLinesColor, linewidth=fit_params['lwidth'], linestyle='--')

    if imgWidthInches is not None:
        xmin, xmax, initT_, T_ = region
        imgHeightInches = abs(T_ - initT_) / (xmax - xmin) * imgWidthInches
        fig.set_size_inches(imgWidthInches, imgHeightInches)

    if noframe:
        fig.patch.set_visible(False)
        ax.axis('off')

    if saveToFile:
        if outputFileName is None:
            outputFileName = sr['filename'].rstrip('.osc') + extension
        bbox = 'tight'
        if noframe:
            bbox = None
        fig.savefig(outputFileName, bbox_inches=bbox, dpi=dpi)
        if verbose: print("Saved file to", utils.OKBLUE + utils.BOLD + outputFileName + utils.ENDC)

    if showResult:
        pyplot.show()

    if closeAfterUse:
        pyplot.close()
    else:
        return fig, ax, pyplot


def showHistory(sim_result, showLog=True):
    sr = sim_result
    phi, T = sr['phi'], sr['T']
    nT, nX = sr['outresT'], sr['outresX']

    # phi = np.gradient(np.gradient(np.gradient(phi, 1)[0], 1)[0], 1)[0]
    if showLog:
        phi = log_abs(phi, 1.e-7)

    # Regarding the plot
    detailColor = 'teal'

    fig, ax = pyplot.subplots(1, 2)
    pyplot.subplots_adjust(left=0.25, bottom=0.25)

    generalPicture = ax[0]
    timeSlice = ax[1]

    generalPicture.imshow(phi, cmap=cmap, origin='lower', \
                          interpolation='quadric')
    generalPicture.axes.set_xticks([], [])
    generalPicture.axes.set_yticks([], [])
    marker = generalPicture.plot([0, nX - 1], [0, 0], color='red')

    collision, = timeSlice.plot(phi[0], color=detailColor)
    # collision.axes.set_ylim([allTimeMin, allTimeMax])
    collision.axes.set_ylim(phi.min(), phi.max())
    collision.axes.set_xlabel(r'$x$', fontsize=16)
    collision.axes.set_ylabel(r'$\phi$', fontsize=16)

    time = pyplot.axes([0.20, 0.15, 0.65, 0.03])

    timeSlider = Slider(time, r'$\tau$', 0, T - 1. / nT, valinit=0.0, \
                        valfmt=u'%.2f', color=detailColor, alpha=0.2)

    def onUpdate(val):
        i = int(val / T * nT)
        collision.set_ydata(phi[i])
        marker[0].set_ydata(i)
        fig.canvas.draw_idle()

    timeSlider.on_changed(onUpdate)

    # ... and show it
    pyplot.show()
