#!/usr/bin/env python
# -*- coding: utf-8 -*-

from SimData_old import SimData_old
from SimData import SimData
from SnapshotData import SnapshotData

from numpy import asarray, linspace, gradient, mean, abs as np_abs, array, sqrt
from os.path import isfile


def OpenSimData(fileName):
    if not isfile(fileName):
        print("File not found \'" + fileName + "\'")
        raise FileNotFoundError

    if fileName[-4:] == '.osc':
        return SimData_old(fileName)
    elif fileName[-5:] == '.oscb':
        return SimData(fileName)
    elif fileName[-5:] == '.oscs':
        return SnapshotData(fileName)


if __name__ == '__main__':
    from sys import argv
    #import argparse
    #parser = argparse.ArgumentParser()

    file = argv[1]
    sim = OpenSimData(file)

    metaData = sim.MetaData

    no_show = ('phi', 'timestamps')
    for key in metaData:
        value = '...'
        if key not in no_show: value = metaData[key]

        value_type = type(value)
        value = str(value)

        print("{:<25}: {:>75}    {}".format(key, value, value_type))

    if type(sim) is SimData and False:
        L = sim["L"]
        u = sim["V"]
        numMomentum = sim.integrateLinMomentumInterval(0.0, -.5*L, 0.)
        analyticMomentum = u/(24.*sqrt(1.-u**2))
        print("{:<25}: {:+>15}".format("Left osc. num. momentum", numMomentum))
        print("{:<25}: {:+>15}".format("Left osc. an. momentum", analyticMomentum))
        print("{:<25}: {:+>15}%".format("Left osc. mom. err %", 100.+100.*analyticMomentum/numMomentum))

        print("{:<25}: {:>15}".format("Total momentum", sim.integrateLinMomentumInterval(0.0, -.5*L, .5*L)))
        for t in linspace(0, sim["T"]*0.9, 20):
            print("{:<25}: {:>15}".format("Total momentum @ t = " + str(t) , sim.integrateLinMomentumInterval(t, -.5*L, .5*L)))

