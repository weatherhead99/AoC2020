#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun Dec 13 18:12:00 2020

@author: danw
"""

import numpy as np
from functools import reduce

def parse_input(inline: str):
    out = []
    numstrs = inline.split(",")
    for ind, num in enumerate(numstrs):
        if num != 'x':
            out.append( (ind, int(num) ) )
    return out

with open("input.txt", "r") as f:
    dat1 = parse_input(f.readlines()[1])
    
# dat1 = parse_input("1789,37,47,1889")

def calc_rhs_coeff(offset, modulo):
    return (-offset % modulo) % modulo


coeffs = [calc_rhs_coeff(*_) for _ in dat1]


def chinese_remainder(n, a):
    sm = 0
    prod=reduce(lambda a, b: a*b, n)
    for n_i, a_i in zip(n,a):
        p=int(prod/n_i)
        sm += a_i* pow(p,-1, n_i)*p
    return sm % prod


a = [_[0] for _ in dat1]
n = [_[1] for _ in dat1]

c = chinese_remainder(n, coeffs)



