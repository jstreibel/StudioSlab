#!/usr/bin/env python3
# -*- coding: utf-8 -*-

from numpy import outer, linspace, ones, mgrid, exp, ogrid, sin
from mayavi import mlab

import open_sim

x, y, z = ogrid[-10:10:20j, -10:10:20j, -10:10:20j]
s = sin(x*y*z)/(x*y*z)

mlab.pipeline.volume(mlab.pipeline.scalar_field(s))

