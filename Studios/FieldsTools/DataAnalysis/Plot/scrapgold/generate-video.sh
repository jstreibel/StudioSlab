#!/bin/sh

mencoder mf://*.png -mf w=1000:h=600:fps=60:type=png -ovc copy -oac copy -o exciton_dynamic.avi
