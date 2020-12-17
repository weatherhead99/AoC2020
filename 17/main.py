#!/usr/bin/env python3
import numpy as np
from math import floor, ceil
from scipy.ndimage import convolve

def parse_grid(fname: str) -> np.ndarray:
    with open(fname, "r") as f:
        lines = [list(_.strip()) for _ in f.readlines()]
    out = np.array(lines)
    return out == '#'

def get_simul_grid(grid_in: np.ndarray, D: int=3) -> np.ndarray:
    inD = len(grid_in.shape)
    simul_grid = grid_in.reshape( (*tuple([1]*(D-inD)), *grid_in.shape))
    simul_grid = np.pad(simul_grid, [(NTURNS,NTURNS)] * len(simul_grid.shape))
    return simul_grid

def count_active_neighbours_conv(grid: np.ndarray, D: int =3) -> np.ndarray:
    weights = np.ones(tuple([3]*D),dtype=np.uint8)
    weights[tuple([1]*D)] = 0
    neighboursum = convolve(grid.astype(np.uint8), weights, mode="constant", cval=0)
    return neighboursum

def step_grid(grid: np.ndarray, *args, **kwargs) -> None:
    active_neighbours = count_active_neighbours_conv(grid, *args, **kwargs)
    deactivate_selec = np.logical_and(grid, np.logical_not(np.logical_or(active_neighbours==2, active_neighbours==3)))
    activate_selec = np.logical_and(np.logical_not(grid), active_neighbours==3)
    grid[activate_selec] = 1
    grid[deactivate_selec] = 0

#NB, grid size cannot grow by more than 2  in each dimension at each turn, 
#so we know in advance we just need to padf each axis by 6 on each side of the given
#numbers
if __name__ == "__main__":
    grid = parse_grid("input.txt")
    NTURNS = 6
    for D in (3,4):
        simul_grid = get_simul_grid(grid,D)
        for i in range(NTURNS):
            step_grid(simul_grid,D)
        n_active = np.count_nonzero(simul_grid)
        print(f"dimension: {D}, number active: {n_active}")

