#!/usr/bin/env python
# -*- coding: utf-8 -*-

import argparse
from sys import exit
from numpy import asarray, sqrt
from Fitting import Params



def View(args):

	filename = args.filename

	if filename[-4:] != '.fit':
		print("Fit file, please. " + filename[0] + " not recognized.")
		exit(1)

	plot = (lambda x, y, latexLabel: pyplot.scatter(x, y, marker='x', label="$"+latexLabel+"$")) if not args.useLine \
			else (lambda x, y, latexLabel: pyplot.plot(x, y, label="$"+latexLabel+"$"))

	params = {}
	with open(filename) as file:
		param_keys = [key for key in file.readline().rstrip().split()]
		fits = [[float(v) for v in line.rstrip().split()] for line in file]

	params = {}
	for key in param_keys:
		index = param_keys.index(key)
		params[key] = asarray([v[index] for v in fits])

	E_in_list = []
	E_out_list = []
	for i in range(len(fits)):
		V_in = params['V_{in}'][i]
		V_out = params['V_{out}'][i]
		l_in = params['l_{in}'][i]
		l_out = params['l_{out}'][i]

		E_in = (l_in**3)/(24.*sqrt(1.-V_in**2))
		E_out = l_out**3/(24.*sqrt(1.-V_out**2))

		E_in_list.append(E_in)
		E_out_list.append(E_out)
	params['E_{in}'] = asarray(E_in_list)
	params['E_{out}'] = asarray(E_out_list)

	xParam = args.x
	yParams = args.y.replace(' ', '').split(',')

	from matplotlib import pyplot
	pyplot.xlabel(xParam)
	for yParam in yParams:
		if not yParam in params:
			print(yParam, 'not in param list.')
			continue
		plot(params[xParam], params[yParam], yParam)

	#from matplotlib import pyplot
	#pyplot.xlabel("$"+xParam.replace('phase', '\\alpha')+"$")

	#if not yParam in params: print("'" + yParam + "'", 'not in param list.')

	# pyplot.scatter(params[xParam], params[yParam], marker=',', label='$'+yParam.replace('phase', '\\alpha')+'$')
	pyplot.legend()
	pyplot.show()


if __name__ == '__main__':
	params = Params.getNames()
	names = "E_{in}; E_{out}; "
	for p in params:
		names += p + "_{in}; " + p + "_{out}; "

	parser = argparse.ArgumentParser()
	parser.add_argument('--filename', '-f', help="Filename. Default is 'fitOutput.fit' files in folder.", \
						type=str, default='./fitOutput.fit')
	parser.add_argument('--y', '-y', help='y-axis param. Available params are ' + names, type=str, default='phase_{out}')
	parser.add_argument('--x', '-x', help='x-axis param.', type=str, default='phase_{in}')
	parser.add_argument('--useLine', '-l', help='Connect points with line, instead of scatter.', action='store_true')

	args = parser.parse_args()

	View(args)
