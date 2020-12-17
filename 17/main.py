#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Dec 17 12:16:40 2020

@author: danw
"""
import numpy as np
from scipy.sparse import coo_matrix
from math import floor, ceil
from scipy.ndimage import convolve

def parse_grid(fname:str):
    with open(fname, "r") as f:
        lines = [_.strip() for _ in f.readlines()]
    xlen = len(lines[0])
    ylen = len(lines)
    out = np.zeros(dtype=np.uint8, shape=(xlen,ylen))
    for i in range(xlen):
        for j in range(ylen):
            out[j,i] = 1 if lines[j][i] == "#" else 0
    return out

grid = parse_grid("input.txt")
#can't grow by more than 2 in z for each turn, so for 6 turns we need 12 in z axis
NTURNS = 6
simul_grid = np.zeros_like(grid,shape=(2*NTURNS +1, grid.shape[0]+ 2 * NTURNS, grid.shape[1] + 2 * NTURNS))
middle_slice = (simul_grid.shape[0] //2, 
                slice(simul_grid.shape[1]//2-floor(grid.shape[0]/2), simul_grid.shape[1]//2+ceil(grid.shape[0]/2)),
                slice(simul_grid.shape[2]//2-floor(grid.shape[1]/2), simul_grid.shape[2]//2+ ceil(grid.shape[1]/2)))


def get_conv_weights(D: int = 3):
    shp = tuple([3] * D)
    cent  = tuple([1] * D)
    out = np.ones(shp,dtype=np.uint8)
    out[cent] = 0
    return out

#ok, seems to work
def count_active_neighbours_conv(grid, D: int =3):
    weights = get_conv_weights(D)
    neighboursum = convolve(grid, weights, mode="constant", cval=0.0)
    return neighboursum

def step_grid(grid, *args, **kwargs):
    active_neighbours = count_active_neighbours_conv(grid, *args, **kwargs)
    remain_active_selec = np.logical_and(grid, np.logical_or(active_neighbours==2, active_neighbours==3))
    deactivate_selec = np.logical_and(grid, np.logical_not(remain_active_selec))
    activate_selec = np.logical_and(np.logical_not(grid), active_neighbours==3)
    grid[activate_selec] = 1
    grid[deactivate_selec] = 0


simul_grid[middle_slice] = grid

neighbours = count_active_neighbours_conv(simul_grid)
simul_grid_start = simul_grid.copy()

for i in range(6):
    step_grid(simul_grid, 3)
print(f"number active: {np.count_nonzero(simul_grid)}")


simul_grid = simul_grid_start.copy()
simul_grid4 = simul_grid.reshape((1,*simul_grid.shape))
simul_grid4 = np.pad(simul_grid4, [((2*NTURNS+1)//2, (2*NTURNS+1)//2), (0,0), (0,0),(0,0)])

for i in range(6):
    step_grid(simul_grid4, 4)
    


