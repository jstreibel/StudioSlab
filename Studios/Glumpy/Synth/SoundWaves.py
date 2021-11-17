#!/usr/bin/env python

import numpy
import pygame
from matplotlib import pyplot

sampleRate = 44100
freq = 440

pygame.mixer.init(44100,-16,2,512)
# sampling frequency, size, channels, buffer

# Sampling frequency
# Analog audio is recorded by sampling it 44,100 times per second, 
# and then these samples are used to reconstruct the audio signal 
# when playing it back.

# size
# The size argument represents how many bits are used for each 
# audio sample. If the value is negative then signed sample 
# values will be used.

# channels
# 1 = mono, 2 = stereo

# buffer
# The buffer argument controls the number of internal samples 
# used in the sound mixer. It can be lowered to reduce latency, 
# but sound dropout may occur. It can be raised to larger values
# to ensure playback never skips, but it will impose latency on sound playback. 

def func(t): 
    return numpy.sin(1.0 * numpy.pi * freq * t / sampleRate) + 0.25*numpy.sin(4.0 * numpy.pi * freq * t / sampleRate)

arr = numpy.array([4096 * func(t)
                for t in range(0, sampleRate)]).astype(numpy.int16)

pyplot.plot(arr)
pyplot.xlim(0, 2*44100/440)
pyplot.show()

arr2 = numpy.c_[arr,arr]
                   
sound = pygame.sndarray.make_sound(arr2)
sound.play(-1)
pygame.time.delay(1000)
sound.stop()
