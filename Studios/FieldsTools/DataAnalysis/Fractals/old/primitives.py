#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Oct 11 18:02:49 2018

@author: joao
"""

from numpy import sqrt, gradient, trapz

def length(dx, dy):
    return sqrt(dx*dx + dy*dy)

def calculateLength(dx, y):
    dy = gradient(y)
    dx2 = dx*dx
    
    return sqrt(dy*dy+dx2).sum()

def calculateAreaUnder(dx, y):
    return abs(trapz(y, dx=dx))
    

def area(a, b, c, d, phi_ac, phi_bc, phi_bd, phi_ad):
    dx = b-a
    dt = c-d
    dphi1x = phi_bc-phi_ac
    dphi1t = phi_ac-phi_ad
    dphi2x = phi_ad-phi_bd
    dphi2t = phi_bd-phi_bc
    
    a1 = dt*dphi1x
    a2 = dx*dphi1t
    a3 = dx*dt
    A1 = sqrt(a1*a1 + a2*a2 + a3*a3)
    
    a1 = dt*dphi2x
    a2 = dx*dphi2t
    a3 = dx*dt
    A2 = sqrt(a1*a1 + a2*a2 + a3*a3)
    
    return .5*(A1+A2)

    