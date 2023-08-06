from .fitting import py_oscillon
from numpy import asarray, linspace

def getPhiValue(t, x, v):
	return py_oscillon.getPhiVal(t, x, v)

def getdPhidtValue(t, x, v):
	return py_oscillon.getdPhidtVal(t, x, v)

class AnalyticOscillon:
	def __init__(self, N, xMin, xMax, mirrored=False, anti_sym=False):
		''' Interface em Python para pegar dados em C++ de breathers analiticos.

			Parameters
			----------
			N : int
				Numero de sitios para discretizar a regiao entre xMin e xMax.
			xMin : float
				Limite esquerdo do espaco.
			xMax : float
				Limite direito do espaco.
			mirrored : bool
				Determina se o oscillon e espelhado em torno do eixo t ou nao. O oscillon
				mais comum, que pertence ao intervalo [0, 1] eh espelhado.
			anti_sym : bool
				Determina se o oscillon deve ser espelhado sobre o eixo x.
		'''
		self.myIndex = py_oscillon.newBreather(N, xMin, xMax, mirrored, anti_sym)

		self._N, self._xMin, self._xMax, self._mirrored, self._anti_sym = N, xMin, xMax, mirrored, anti_sym

	def __del__(self):
		py_oscillon.finishBreather(self.myIndex)

	def get(self, t, v, V, alpha, d, l):
		rawOsc = py_oscillon.get(self.myIndex, t, v, V, alpha, d, l)
		osc = py_oscillon.doubleArray_frompointer(rawOsc)

		return asarray([osc[i] for i in range(self._N)])

	def getx(self):
		return linspace(start=self._xMin, stop=self._xMax, num=self._N)
