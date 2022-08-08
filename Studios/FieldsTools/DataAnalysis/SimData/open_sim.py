#!/usr/bin/env python
# -*- coding: utf-8 -*-

from numpy import asarray, linspace, gradient, mean, abs as np_abs, array, sqrt
from os.path import isfile


def OpenSimData(fileName):
    if not isfile(fileName):
        print("File not found \'" + fileName + "\'")
        raise FileNotFoundError

    if fileName[-4:] == '.osc':
        from SimData_depr import SimData_depr
        return SimData_depr(fileName)
    elif fileName[-5:] == '.oscb':
        from SimData import SimData
        return SimData(fileName)
    elif fileName[-5:] == '.oscs':
        from SnapshotData import SnapshotData
        return SnapshotData(fileName)

def ShowHeaderOfOscFile(fileName):
    sim = OpenSimData(fileName)

    metaData = sim.MetaData

    no_show = ('phi', 'timestamps')
    for key in metaData:
        value = '...'
        if key not in no_show: value = metaData[key]

        value_type = type(value)
        value = str(value)

        print("{:<25}: {:>75}    {}".format(key, value, value_type))

    from SimData import SimData

    if type(sim) is SimData and False:
        L = sim["L"]
        u = sim["V"]
        numMomentum = sim.integrateLinMomentumInterval(0.0, -.5 * L, 0.)
        analyticMomentum = u / (24. * sqrt(1. - u ** 2))
        print("{:<25}: {:+>15}".format("Left osc. num. momentum", numMomentum))
        print("{:<25}: {:+>15}".format("Left osc. an. momentum", analyticMomentum))
        print("{:<25}: {:+>15}%".format("Left osc. mom. err %", 100. + 100. * analyticMomentum / numMomentum))

        print("{:<25}: {:>15}".format("Total momentum", sim.integrateLinMomentumInterval(0.0, -.5 * L, .5 * L)))
        for t in linspace(0, sim["T"] * 0.9, 20):
            print("{:<25}: {:>15}".format("Total momentum @ t = " + str(t),
                                          sim.integrateLinMomentumInterval(t, -.5 * L, .5 * L)))


if __name__ == '__main__':
    from sys import argv
    #import argparse
    #parser = argparse.ArgumentParser()

    fileName = argv[1]

    ShowHeaderOfOscFile(fileName)


