#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon May 21 17:01:23 2018

@author: johnny
"""

from os.path import exists as pathExists, isfile, getsize
from os import makedirs
from os import environ as env
import subprocess
from time import sleep
import numpy as np
from sys import stdout
import bcolors

f = '{:.5f}'.format

try:
    _cwd = env['SKYRMEDATA'] + '/sym/'
except Exception:
    _cwd = './sym/'

def getCWD():
    return _cwd

class SimException(Exception):
    def __init__(self, value):
        self.value = value

    def __str__(self):
        return "Simulation error: " + repr(self.value)

class Simulation(object):
    def __init__(self, T, L, N, r=0.1, v=0, V=0, ph=0, overrideExisting=False):
        self.T=T
        self.L=L
        self.N=N
        self.v=v
        self.V=V
        self.ph=ph
        self.outresX = None
        self.outresT = None
        self._phi = None

        global _cwd
        self._cwd = _cwd

        self.loc = self._cwd + 'v=' + f(v) + '/V=' + f(V) + '/ph=' + f(ph) + '/'
        if not pathExists(self.loc):
            makedirs(self.loc)

        self.fileName = self.loc + 'N=' + str(N) + '-r=' + str(r) + '.osc'
        self.hasImage = self.imgExists()

        if overrideExisting:
            self.runSim(overrideExisting)

    def getCWD(self):
        return self._cwd

    def imgFileName(self):
        return self.fileName+'.png'

    def getPhi(self):
        if not self.simExists():
            self.runSim()
        self.openSim()

        return self._phi

    def simExists(self):
        return isfile(self.fileName)

    def imgExists(self):
        return isfile(self.imgFileName())

    def runSim(self, overrideExisting=False):
        if isfile(self.fileName) and not overrideExisting:
            return

        print(bcolors.OKGREEN + 'Running sim: ', end='\n\t' + bcolors.OKBLUE)
        T, L, N, V, v, ph = self.T, self.L, self.N, self.V, self.v, self.ph
        arglist = ["skyrme-radial", '--T='+f(T), '--L='+f(L), '--N='+str(N),
                   '--V='+f(V), '--v='+f(v), '--phase='+f(ph), '--out='+self.fileName, '--threads=4']
        for arg in arglist:
            print(arg, end=' ')
        print("\n\t", end=bcolors.WARNING)
        proc = subprocess.Popen(arglist, stdout=subprocess.DEVNULL)
        while proc.returncode is None:
            proc.poll()
            print('.', end='')
            stdout.flush()
            sleep(0.1)

        print(bcolors.OKGREEN + ' Done.' + bcolors.ENDC)

    def openSim(self):
        if self._phi is not None:
            return

        fileName = self.fileName
        with open(fileName) as file:
            param_keys = [val for val in file.readline().replace(' ', '').strip('#').strip().split('|')]
            param_vals= [float(val) for val in file.readline().split()]
            #N, n, h, dT, L, T, v, V, phase, d, r, outresX, outresT = [float(val) for val in file.readline().split()]
            phi = [[float(digit) for digit in line.split()] for line in file]
            phi = np.asarray(phi)
        #grad = np.gradient(phi)
        #H = (grad[0]**2 + grad[1]**2 + np.abs(phi)) + 1.e-6
        dictionary = dict(zip(param_keys, param_vals))
        dictionary['filename'] = fileName
        dictionary['filesize'] = getsize(fileName)

        self._phi = phi
        self._alles = dictionary
