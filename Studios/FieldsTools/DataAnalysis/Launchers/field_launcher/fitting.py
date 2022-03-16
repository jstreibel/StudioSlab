from .c_analytic import py_oscillon
import Fitting
from numpy import linspace, asarray, append, zeros
from scipy.optimize import minimize
import utils


class Params:
	"""Parametros para fitting da classe OscillonFitting."""

	def __init__(self, alpha, v, V, l, d):
		""" Parametros para fitting."""

		V = -V  # Por causa de confusoes: o oscillon analitico eh espelhado.

		self._alpha = alpha
		self._v = v
		self._V = V
		self._l = l
		self._d = d

		self._list = (alpha, v, V, l, d)

	def __getitem__(self, item):
		if isinstance(item, str):
			item = ("alpha", "v", "V", "l", "d").index(item)

		return self._list[item]

	def toString(self):
		string = ""
		e = utils.fmtExp

		for a in self._list:
			string += " " + e(a)

		return string

	def getList(self):
		return self._list

	def getNames():
		''' Pegar o nome dos parametros. Metodo estatico, nao recebe 'self' como argumento.

		'''

		return ('phase', 'v', 'V', 'l', 'd')

class Region:
	def __init__(self, xMin, xMax, tMin, tMax, u, skew=True):
		self.xMin = float(xMin)
		self.xMax = float(xMax)
		self.tMin = float(tMin)
		self.tMax = float(tMax)

		self.u = float(u)

		self._arr = xMin, xMax, tMin, tMax

		self.skew=skew

	def __getitem__(self, index):
		return self._arr[index]

	def getWidth(self):
		return self.xMax - self.xMin

	def getExtent(self):
		return self.xMin, self.xMax, self.tMin, self.tMax

	def getTRange(self, n):
		return linspace(self.tMin, self.tMax, n)

	def getXLims(self, t, forceSkew=False):
		if not self.skew and not forceSkew:
			return self.xMin, self.xMax

		return self.xMin+(t-self.tMin)*self.u, self.xMax+(t-self.tMin)*self.u

	def getPlotPolygon(self, forceSkew=False, color='k'):
		from matplotlib.patches import Polygon

		t0, t1 = self.tMin, self.tMax
		xMin0, xMax0 = self.getXLims(t0)
		xMin1, xMax1 = self.getXLims(t1, forceSkew=forceSkew)

		points = asarray([[xMin0, t0],
						  [xMin1, t1],
						  [xMax1, t1],
						  [xMax0, t0]])

		return Polygon(points, facecolor='none', edgecolor=color)


class OscillonFitting:
	def __init__(self, simData, verbose=False):
		''' Classe de fitting do oscillon.

			Devido a forma simplificada como eh implementada a alocacao de memoria
			da parte em C/C++ das rotinas de fitting, esta classe so funciona garantidamente blend_mode
			quando houver apenas uma instancia dela. Ocorre que ela guarda o estado interno na
			parte em C/C++ e esse estado eh compartilhado entre todas as instancias de
			OscillonFitting, alem de ser reinicializado toda vez que as rotinas de fitting
			sao rodadas.

		'''
		self._myIndex = py_oscillon.NewFit()

		self._data = simData
		self._verbose = verbose
		self._indices = []

		self._method = OscillonFitting.getMethods(1)

		# Obs.: melhor usar soma dos valores absolutos da diferenca, por ser mais estavel
		# que a soma dos quadrados.
		self._errFunc = (py_oscillon.computeAbsDiffAvg, py_oscillon.computeLeastSquares)[0]

		self._count = 0 # so serve para fazer flush de um que outro print. Conta tambem quantas iteracoes levou para fitar.

		# valor retornado quando parametros saem do esperado.
		self._maxErr = 10.

	def __del__(self):
		py_oscillon.DelFit(self._myIndex)

	def getMethods(index = None):
		''' Funcao global de OscillonFitting, nao recebe 'self' como argumento.

		'''

		if index is not None: return OscillonFitting.getMethods()[index]

		# Bound constraints support: COBYLA, SLSQP

		# 'None; abaixo significa que o metodo eh escolhido automaticamente (ou deveria ser).
		return None, 'Nelder-Mead', 'Powell', 'CG', 'BFGS', 'Newton-CG', 'L-BFGS-B', 'TNC', \
		'COBYLA', 'SLSQP', 'trust-constr', 'dogleg', 'trust-ncg', 'trust-exact', \
		'trust-krylov'

	def _getAnalyticOsc(self, index, alpha, v, V, l, d):
		rawOsc = py_oscillon.getOscFromFit(self._myIndex, index, alpha, v, V, l, d)
		N = py_oscillon.getNFromFit(self._myIndex, index)
		osc = py_oscillon.doubleArray_frompointer(rawOsc)
		return asarray([osc[i] for i in range(N)])

	def _fitFunctional(self, params):
		alpha, v, V, l, d = params

		if abs(V) >= 1.: return self._maxErr

		f = utils.fmtFunc(10)
		if self._verbose: print("Trying (alpha, v, V, l, d) =", f(alpha), f(v), f(V), f(l), f(d))
		err = self._errFunc(self._myIndex, alpha, v, V, l, d)
		if self._verbose: print(utils.HEADER + "Err =", err, utils.ENDC)
		elif not self._count % 50: print('.', end='', flush = True)

		if err > self._maxErr: self._maxErr = err
		self._count+=1
		return err

	def fitRegion(self, region, n, initGuess, bounds, useAbsBreather=True):
		''' Faz o fitting de uma regiao nos dados simData passados para o construtor.

		Parameters
		----------
		region : Region
			A regiao dentro de simData (gerados pelo modulo open_sim.py) em que se quer fazer o fitting.
		n : int
			Numero de passos para computar o tempo entre region.tMin e region.tMax.
		initGuess : List
			"Chute" inicial dos valores do fitting. Devem ser quatro parametros tipo double: alpha, v, V, l, d
		bounds : List
			Limites que os "chutes" podem assumir. Cada entrada da lista deve ser um par de valores indicando min e max.
		useAbsBreather: bool
			Significa que o termo 'd' dos Params de fitting sera do oscillon original,
			que possui a borda esquerda em t=0 no ponto x=0. No caso contraio
			com useAbsBreather = False, o parametro 'd' ira se referir ao espaco dentro
			da Region.
		'''

		self._tRange = linspace(region.tMin, region.tMax, n)
		self._n = n
		self._initGuess = initGuess
		self._bounds = bounds
		self._region = region
		r = self._region
		x_0 = 0
		t_0 = 0

		# bounds[2] refere-se a V
		VMin, VMax = bounds[2]
		if VMin < -1. or VMax > 1.: raise Exception("Fitting bounds out of interval [-1, 1]: VMin =", VMin, "    VMax =", VMax)

		if not useAbsBreather:
			x_0 = r.xMin
			t_0 = r.tMin

		py_oscillon.clearAllData(self._myIndex)
		for t in self._tRange:
			x_i, x_f = r.getXLims(t)

			data = self._data.getInstant(t, x_i, x_f)
			# Cada vez que adicionamos dados ao mecanismo de fitting, um novo
			# oscillon analitico e gerado correspondendo aquele instante da
			# simulacao. A funcao py_oscillon.addData retorna o indice daquele
			# oscillon analitico criado.

			index = py_oscillon.addData(self._myIndex, data, t-t_0, x_i-x_0, x_f-x_0)
			self._indices.append(index)

		# FITTING PROPRIAMENTE DITO
		if self._method in ('Nelder-Mead', 'CG'): bounds = None
		self._minimizeResult = minimize(self._fitFunctional, initGuess.getList(), method=self._method, bounds=bounds)

		if 0: print("", self._count, "iteracoes.")
		else: print("")

		if self._verbose:
			alpha_i, v_i, V_i, l_i, d_i = initGuess
			alpha, v, V, l, d = self._minimizeResult.x
			print(utils.OKBLUE + "Result: param = val (init. guess)" \
				"\n\talpha =", alpha, " (" + str(alpha_i) +  ")" \
				"\n\t    v =", v,     " (" + str(v_i) +  ")" \
				"\n\t    V =", V,     " (" + str(V_i) +  ")" \
				"\n\t    l =", l,     " (" + str(l_i) +  ")" \
				"\n\t    d =", d,     " (" + str(d_i) +  ")" + utils.ENDC)


			print(utils.OKGREEN + "Erro do chute:", self._errFunc(alpha_i, v_i, V_i, l_i, d_i))
			print("Erro do fit:  ", self._errFunc(alpha, v, V, l, d), utils.ENDC)

		alpha, v, V, l, d = self._minimizeResult.x
		return Params(alpha, v, -V, l, d), self._count

	def getNumericalDataFromComputation(self):
		''' Pega os dados da simulacao na regiao onde foi feito o ultimo fitting.
		'''

		r = self._region
		simRegion = []
		size = 0
		for t in self._tRange:
			x_i, x_f = r.getXLims(t)

			data = self._data.getInstant(t, x_i, x_f)
			# Quando puxamos os dados atraves do metodo acima getInstant, dependendo
			# do intervalo de tempo (ainda que este intervalo pareca ser essencialmente
			# constante), as vezes temos um off-by-one. As linhas abaixo corrigem isso
			# na mao.
			if size == 0: size = data.shape[0]
			offBy = data.shape[0] - size
			if offBy > 0:
				data = data[:-offBy]
			elif offBy < 0:
				data = append(data, zeros(-offBy))

			simRegion.append(data)

		return asarray(simRegion)

	def getAnalyticDataFromComputation(self, params = None):
		''' Pega os dados analiticos na regiao onde foi feito o ultimo fitting.

			Parameters
			----------
			params : List
				Os parametros com os quais gerar o oscillon analitico.
		'''
		if params is None: params = self._minimizeResult.x

		alpha, v, V, l, d = params
		analyticData = []

		size = 0
		for i in self._indices:
			analytic = self._getAnalyticOsc(i, alpha=alpha, v=v, V=V, l=l, d=d)

			if size == 0: size = analytic.shape[0]
			offBy = analytic.shape[0] - size
			if offBy > 0:
				analytic = analytic[:-offBy]
			elif offBy < 0:
				analytic = append(analytic, zeros(-offBy))

			analyticData.append(analytic)

		return asarray(analyticData)

	def getAnalyticData(self, params = None, region=None, tRange=None, N=1024):
		if params is None: params = self._minimizeResult.x
		if region is None: region = self._region
		if tRange is None: tRange = self._tRange
		r = region
		x_0 = r.xMin
		t_0 = r.tMin

		alpha, v, V, l, d = params

		analyticData = []
		for t in tRange:
			analytic = Fitting.AnalyticOscillon(1000, r.xMin, r.xMax, mirrored=True)
			analyticData.append(analytic.get(t-t_0, v=v, V=V, alpha=alpha, d=d, l=l))

		return asarray(analyticData)
