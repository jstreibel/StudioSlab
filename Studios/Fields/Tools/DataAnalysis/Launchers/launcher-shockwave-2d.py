#!/usr/bin/env python3
# -*- coding: utf-8 -*-

###!/home/joao/Developer/v-shape/data-analysis/venv/bin python3


import argparse
from numpy import unique, linspace
import utils
from os import stat, path
import time
from datetime import timedelta
import subprocess

parser = argparse.ArgumentParser()

parser.add_argument('--t', help="Total simulation time per simulation.", type=float, default=3.0)
parser.add_argument('--L', help="Simulation width.", type=float, default=7.0)
parser.add_argument('--N', help="Simulation resolution.", type=int, default=1024)

parser.add_argument('--eps', help="Shockwave epsilon value.", type=float, default=0.025)
parser.add_argument('--ecc', help="Sweep elliptic azimuthal sym. break eccentricity in n steps.", nargs=3, type=float, default=(0, 0, 1))
parser.add_argument('--th', help="Sweep ellipse angle.", nargs=3, type=float, default=(0, 0, 1))

parser.add_argument('--maxprocs', help="Maximum number of parallel running programs.", type=int, default=2)
parser.add_argument('--maxgpu',   help="Maximum processes running in GPU. All others processes (maxprocs-maxgpu) no GPU.",
                    type=float, default=2)

parser.add_argument('--outN', help="Full history simulation output resolution.", type=int, default=1024)
parser.add_argument('--outputSnapshotAtEnd', '-s', help="Output a snapshot at the end of simulation.",
                    action='store_true')

parser.add_argument('--pipestdout', '-p', help="Pipe stdout. Default stdout is DEVNULL.", action='store_true')

args = parser.parse_args()

MAX_PROCS = args.maxprocs
MAX_GPU = args.maxgpu
gpuOccupationPID=[0,] * MAX_GPU

STDOUT = subprocess.DEVNULL
if args.pipestdout:
    print("stdout is PIPE")
    STDOUT = subprocess.PIPE
else:
    print("stdout is DEVNULL")

t = args.t
L = args.L
N = args.N

eps = args.eps
eccMin, eccMax = args.ecc[:2]
eccSteps = int(args.ecc[2])
thMin, thMax = args.th[:2]
thSteps = int(args.th[2])

simList = []

eccRange = unique(linspace(eccMin, eccMax, eccSteps))
thRange = unique(linspace(thMin, thMax, thSteps))

f = '{:.8f}'.format
for ecc in eccRange:
    for th in thRange:
        fileName = "sw2d-ecc=" + f(ecc) + "-th=" + f(th) + "-eps="+f(eps) + "-N=" + str(N)

        argList = ["v-shape", "--out="+fileName, "--sim=4", "--t="+f(t), "--L="+f(L), "--N="+str(N), "--eps="+f(eps), "--ecc="+f(ecc), "--th="+f(th)]
        simInfo = (ecc, th)
        simList.append((argList, fileName, simInfo))

processTuples = []
deadProcesses = []
totalSims = len(simList)
nFinishedSims = 0
totalTime = 0.0

while len(simList) or not (len(deadProcesses) == totalSims):
    while not len(processTuples) == MAX_PROCS and len(simList):
        argList, fileName, simInfo = simList.pop()
        simInfoStr = utils.yellow('ecc=%.5f th=%.5f' %  (simInfo))

        fileOscExists = path.isfile(fileName) and stat(fileName).st_size > 0
        filePngExists = path.isfile(fileName + '.png')

        if filePngExists:
            print(utils.okfail('The .png file already exists.', True))
            continue
        if fileOscExists:
            print(utils.okfail('The .osc file already exists.', True))
            continue

        programCall = argList
        dev=0
        for gpuIndex in range(len(gpuOccupationPID)):
            gpuPID = gpuOccupationPID[gpuIndex]
            if gpuPID == 0: # tem vaga
                dev = gpuIndex + 1
                programCall.append("--dev="+str(dev))
                break

        proc = subprocess.Popen(programCall, stdout=STDOUT)
        processTuples.append((proc, time.time()))

        if dev != 0:
            gpuOccupationPID[dev-1] = proc.pid
            print("gpuOccupationPID append:", gpuOccupationPID)

        print(totalSims-len(simList), '/', totalSims, 'STARTED with', simInfoStr, "on device "+str(dev), end='. ')

        print('PID', proc.pid)

    for pTuple in processTuples:
        proc, pInitTime = pTuple
        poll = proc.poll()
        if proc.returncode is not None:
            elTime = time.time() - pInitTime
            totalTime += elTime
            nFinishedSims += 1
            deadProcesses.append((proc, elTime))
            processTuples.remove(pTuple)
            retCodeStr = utils.okfail('return code ' + str(proc.returncode), proc.returncode!=0)
            avgstring = utils.blue('Avg ' + f(totalTime/nFinishedSims))

            if STDOUT == subprocess.PIPE: print(proc.communicate()[0].decode("utf-8"))

            expectedTimeStr = utils.Bblue('expected ~' + str(timedelta(seconds=(totalTime/nFinishedSims)*(len(simList)/MAX_PROCS))))

            if proc.pid in gpuOccupationPID:
                i = gpuOccupationPID.index(proc.pid)
                gpuOccupationPID[i] = 0
                print("gpuOccupationPID remove", proc.pid,":", gpuOccupationPID)

            print('FINISHED', proc.pid, retCodeStr, 'approx. elapsed time', f(elTime), 'sec.' , avgstring,
                  expectedTimeStr)

    time.sleep(.1)