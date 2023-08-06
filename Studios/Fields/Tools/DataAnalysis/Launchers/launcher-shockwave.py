#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu May 10 19:19:17 2018

@author: johnny
"""

import subprocess
import os
isfile = os.path.isfile
from numpy import unique, linspace, power, arange
import time
from datetime import timedelta
import argparse

import utils

parser = argparse.ArgumentParser()
parser.add_argument('--T', help="Total simulation time per simulation.", type=float, default=4.0)
parser.add_argument('--L', help="Simulation width.", type=float, default=8)
parser.add_argument('--N', help="Simulation resolution.", type=int, default=2**12)
parser.add_argument('--outN', help="Simulation output resolution.", type=int, default=1024)
parser.add_argument('--a', help="Sweep a in n steps (vmin, vmax, steps). Example \'--a 0.1 0.2 10\' will sweep \'a\' from 0.1 to 0.2 in 10 steps.", nargs=3, type=float, default=(0.1, 0.1, 1))
parser.add_argument('--deltafunc', help="Delta function to use.", type=int, default=0)
parser.add_argument('--eps', help="Sweep \'epsilon\' in n steps. Same as for \'v\'.", nargs=3, type=float, default=(1.e-4, 1.e-4, 1))
parser.add_argument('--r', help="Sweep r in n steps. Same as for \'v\'.", nargs=3, type=float, default=(0.1, 0.1, 1))
parser.add_argument('--epslog', '-el', help="Use log scale to step between values of epsilon.", action='store_true')
parser.add_argument('--maxprocs', help="Maximum number of parallel running programs.", type=int, default=7)
parser.add_argument('--usegpu', '-g', help="Use GPUS when available. All others processes no GPU. ***experimental***", action='store_true')

args = parser.parse_args()

MAX_PROCS = args.maxprocs
USE_GPU = args.usegpu
gpu_occupation_PID=[0,0]

T = args.T
L = args.L
N = args.N
outN = args.outN
delta = args.deltafunc
logScale = args.epslog

amin, amax, asteps = args.a
asteps = int(asteps)
epsmin, epsmax, epssteps = args.eps
epssteps = int(epssteps)
rmin, rmax, rsteps = args.r
simlist = []

rangeFunc = lambda a, b, n: unique(linspace(a, b, n))
if logScale:
    rangeFunc = lambda a, b, n: a * power((b/a)**(1./n), arange(n))

a_range = unique(linspace(amin, amax, asteps))
eps_range = rangeFunc(epsmin, epsmax, epssteps)
r_range = unique(linspace(rmin, rmax, rsteps))

f = '{:.8f}'.format
for a in a_range:
    for eps in eps_range:
        for r in r_range:

            filename = './shockwave-a=' + f(a) + '-eps=' + f(eps) + '-N=' + str(N) + '-r=' + f(r) + '.osc'

            arglist = ["skyrme-radial", '--sim=3', '--T='+f(T), '--L='+f(L), '--N='+str(N), '--r='+f(r), '--outN='+str(outN), '--a='+f(a) , '--eps='+f(eps), '--out='+filename, '--threads=1', "--deltafunc="+str(delta)]
            siminfo = (a, eps, r)
            simlist.append((arglist, filename, siminfo))

proctuples = []
deadprocs = []
total = len(simlist)
totaltime = 0.0
n = 0
while len(simlist) or not (len(deadprocs) == total):
    while not len(proctuples) == MAX_PROCS and len(simlist):
        sim = simlist.pop()
        siminfo = utils.yellow('a=%.5f eps=%.5f r=%.5f' %  (sim[2]))
        print(total-len(simlist), '/', total, 'STARTED with', siminfo, end='. ')

        fileOscExists = isfile(sim[1]) and os.stat(sim[1]).st_size > 0
        filePngExists = isfile(sim[1] + '.png')
        if filePngExists:
            print(utils.okfail('The .png file already exists.', True))
            continue
        if fileOscExists:
            print(utils.okfail('The .osc file already exists.', True))
            continue

        program_call = sim[0]
        dev=0
        if USE_GPU:
            if gpu_occupation_PID[0] == 0:
                program_call.append("--dev=1")
                dev=1
            elif gpu_occupation_PID[1] == 0:
                program_call.append("--dev=2")
                dev=2

        proc = subprocess.Popen(program_call, stdout=subprocess.DEVNULL)
        proctuples.append((proc, time.time()))

        if USE_GPU:
            if dev != 0:
                gpu_occupation_PID[dev-1] = proc.pid

        print('PID', proc.pid)


    for proct in proctuples:
        proc = proct[0]
        poll = proc.poll()
        if proc.returncode is not None:
            eltime = time.time() - proct[1]
            totaltime += eltime
            n += 1
            deadprocs.append((proc, eltime))
            proctuples.remove(proct)
            retcodestring = utils.okfail('return code ' + str(proc.returncode), proc.returncode)
            avgstring = utils.blue('Avg ' + f(totaltime/n))
            expectedstring = utils.Bblue('expected ~' + str(timedelta(seconds=(totaltime/n)*(len(simlist)/MAX_PROCS))))

            if USE_GPU:
                if gpu_occupation_PID[0] == proc.pid:
                    gpu_occupation_PID[0] = 0
                elif gpu_occupation_PID[1] == proc.pid:
                    gpu_occupation_PID[1] = 0

            print('FINISHED', proc.pid, retcodestring,
                  'approx. elapsed time', f(eltime), 'sec.' , avgstring,
                  expectedstring)

    time.sleep(.1)

print('\n\nAll done.')
