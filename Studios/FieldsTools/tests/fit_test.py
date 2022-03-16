from fitting import oscillon, fitting
from open_sim import SimData
from matplotlib import pyplot, gridspec
from numpy import linspace
import plot_single

SHOW_REGION_ON_WHOLE = True

if 0: # Mostrando um oscillon analitico qualquer em uma regiao do espaco-tempo
    N, Lmin, Lmax = int(2**10), -3., 1.
    osc = oscillon.AnalyticOscillon(N, Lmin, Lmax, False, False)
    x = linspace(Lmin, Lmax, N)
    space = []
    for _t in linspace(0, 1, 100):
        phi = osc.get(t=_t, v=.95, V=-.9, alpha=.0, d=0., l=1.)
        space.append(phi)

    pyplot.imshow(space, extent=(-3.,1.,0.,1.), origin='lower', interpolation='quadric')
    pyplot.show()

if 0: # Mostrando um oscillon analitico qualquer em um instante de tempo
    xMin, xMax = -1.,0.4
    osc = oscillon.AnalyticOscillon(1000, xMin=xMin, xMax=xMax)
    pyplot.plot(linspace(xMin, xMax, 1000), osc.get(t=0.0, v=0, V=0.5, alpha=0, d=-.1, l=0.2))
    pyplot.show()

if 0:
	import matplotlib.pyplot as plt
	import matplotlib.gridspec as gridspec

	fig = plt.figure(figsize=(10, 8))
	outer = gridspec.GridSpec(2, 2, wspace=0.2, hspace=0.2)
	inner = gridspec.GridSpecFromSubplotSpec(2, 1, subplot_spec=outer[3], wspace=0.1, hspace=0.1)

	for j in range(2):
		ax = plt.Subplot(fig, inner[j])
		t = ax.text(0.5,0.5, 'inner=%d' % (j))
		t.set_ha('center')
		ax.set_xticks([])
		ax.set_yticks([])
		fig.add_subplot(ax)

	for i in range(3):
		ax = plt.Subplot(fig, outer[i])
		t = ax.text(0.5,0.5, 'outer=%d' % (i
		))
		t.set_ha('center')
		ax.set_xticks([])
		ax.set_yticks([])
		fig.add_subplot(ax)

	plt.show()

if 1: # Testando fitting
	filename = '/home/joao/Documents/result/data/workdir/SYMMETRIC/V=0.84-ph=0.27/sym-v=0.0000-V=0.8400-ph=0.2700-d=0.5436-(r=0.1000-N=131072).osc'
	simData = SimData(filename)
	oscFit = fitting.OscillonFitting(simData)

	skew = False

	#region = fitting.Region(xMin=-1.4, xMax=-0.8, tMin=2.5, tMax=3.5, u=-.3, skew=skew)
	#initGuess = fitting.Params(alpha=0.6, v=0.0, V=-0.3, l=0.2, d=-.3)
	#bounds = (0.,1.), (0., 0.95), (-0.95, 0.95), (0.02, 0.2), (-1.2, 1.2)

	region = fitting.Region(xMin=-2.2, xMax=-1.45, tMin=2.5, tMax=3.0, u=-simData['V'], skew=True)
	initGuess = fitting.Params(alpha=simData['phase'], v=simData['v'], V=-simData['V'], l=1.0, d=0.0)
	bounds = (0.,1.), (0., 0.95), (-simData['V']-0.05, -simData['V']+0.05), (0.9, 1.1), (-0.1, 0.1)

	nSteps = 25
	if nSteps < 2: raise Exception("Ta erado.")


	# *********************************************************************************
	result = oscFit.fitRegion(region, n=nSteps, initGuess=initGuess, bounds=bounds) # *
	# *********************************************************************************


	analyticDataFromResult = oscFit.getAnalyticDataFromComputation(result.x)
	numericData = oscFit.getNumericalDataFromComputation()
	analyticDataFromInitGuess = oscFit.getAnalyticDataFromComputation(initGuess)
	fullAnalyticInitGuess = oscFit.getAnalyticData(params=initGuess, \
									region=fitting.Region(-simData['L']*.5, simData['L']*.5, 0.0, simData['T'], u=0, skew=skew), \
									tRange=linspace(0., simData['T'], 500),
									N=1000)
	fullAnalyticFit = oscFit.getAnalyticData(params=result.x, \
									region=fitting.Region(-simData['L']*.5, simData['L']*.5, 0.0, simData['T'], u=0, skew=skew), \
									tRange=linspace(0., simData['T'], 500),
									N=1000)

	epsilon = 1.e-6
	scaleFunc = (plot_single.log_abs, lambda phi, eps: phi)[1]
	analyticDataFromResult = scaleFunc(analyticDataFromResult, epsilon)
	numericData = scaleFunc(numericData, epsilon)
	analyticDataFromInitGuess = scaleFunc(analyticDataFromInitGuess, epsilon)


	fig = pyplot.figure(figsize=(10, 5))
	big_grid = gridspec.GridSpec(2, 2, wspace=0.2, hspace=0.2)
	small_grid = gridspec.GridSpecFromSubplotSpec(1, 4, subplot_spec=big_grid[3], wspace=0, hspace=1.0)

	vmin = numericData.min()
	vmax = numericData.max()

	ax = pyplot.Subplot(fig, small_grid[1])
	im01 = ax.imshow(analyticDataFromResult, extent=region, origin='lower',
							interpolation='quadric', vmin=vmin, vmax=vmax)
	ax.set_title('$\\phi_{fit}(\\Omega)$')
	#fig.colorbar(im01, ax=ax)
	fig.add_subplot(ax)
	ax.set_yticklabels([])
	ax.set_ylabel(None)

	ax = pyplot.Subplot(fig, small_grid[2])
	im10 = ax.imshow(numericData, extent=region, origin='lower',
							interpolation='quadric', vmin=vmin, vmax=vmax)
	ax.set_title('$\\phi_{num}(\\Omega)$')
	#fig.colorbar(im10, ax=ax)
	fig.add_subplot(ax)
	ax.set_yticklabels([])
	ax.set_ylabel(None)

	ax = pyplot.Subplot(fig, small_grid[3])
	delta_phi = analyticDataFromResult-numericData
	delta_phi = scaleFunc(delta_phi, epsilon)
	im11 = ax.imshow(delta_phi, extent=region,
							origin='lower', interpolation='quadric', vmin=vmin, vmax=vmax)
	ax.set_title('$\\phi_{fit}-\\phi_{num}$')
	#fig.colorbar(im11, ax=ax)
	fig.add_subplot(ax)
	ax.set_yticklabels([])
	ax.set_ylabel(None)

	ax = pyplot.Subplot(fig, small_grid[0])
	im00 = ax.imshow(analyticDataFromInitGuess, extent=region, origin='lower',
							interpolation='quadric', vmin=vmin, vmax=vmax)
	ax.set_title('$\\phi_{guess}(\\Omega)$')
	#fig.colorbar(im00, ax=ax)
	fig.add_subplot(ax)


	dict = simData.getFullDictionary()

	ax = pyplot.Subplot(fig, big_grid[0])
	plot_single.plot(dict, saveToFile=False,
				showResult=False, imgWidthInches=None, dpi=150, closeAfterUse=False,
				showLog=True, cmap='BrBG', fig_ax=(fig, ax))
	ax.add_patch(region.getPlotPolygon(forceSkew=True, color='y'))
	ax.add_patch(region.getPlotPolygon(forceSkew=False))
	ax.set_title("Dados numericos")
	fig.add_subplot(ax)

	# Abaixo fazemos uma gambiarra para conseguir plotar o oscillon analitico.
	# Basicamente, colocamos no dicionario aquilo que nos convem e mandamos plotar.
	dict['phi'] = fullAnalyticInitGuess
	dict['outresX'] = fullAnalyticInitGuess.shape[0]
	dict['outresT'] = fullAnalyticInitGuess.shape[1]
	dict['d'] = initGuess._d
	ax = pyplot.Subplot(fig, big_grid[1])
	plot_single.plot(dict, saveToFile=False,
				showResult=False, imgWidthInches=None, dpi=150, closeAfterUse=False,
				showLog=True, cmap='BrBG', fig_ax=(fig, ax))
	ax.add_patch(region.getPlotPolygon(forceSkew=True, color='y'))
	ax.add_patch(region.getPlotPolygon(forceSkew=False))
	ax.set_title("Oscillon incidente (chute inicial)")
	fig.add_subplot(ax)

	dict['phi'] = fullAnalyticFit
	dict['outresX'] = fullAnalyticFit.shape[0]
	dict['outresT'] = fullAnalyticFit.shape[1]
	dict['d'] = initGuess._d
	ax = pyplot.Subplot(fig, big_grid[2])
	fig, ax, pyplot = plot_single.plot(dict, saveToFile=False,
				showResult=False, imgWidthInches=None, dpi=150, closeAfterUse=False,
				showLog=True, cmap='BrBG', fig_ax=(fig, ax))
	ax.add_patch(region.getPlotPolygon(forceSkew=True, color='y'))
	ax.add_patch(region.getPlotPolygon(forceSkew=False))
	ax.set_title("Fitting")
	fig.add_subplot(ax)

	#pyplot.tight_layout()
	pyplot.show()
