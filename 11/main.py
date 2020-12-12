#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Fri Dec 11 22:27:06 2020

@author: danw
"""
import numpy as np

mapper = {"#" : 1, 'L' : 0, '.' : -1}
rev_mapper = {v:k for k,v in mapper.items()}
from scipy.ndimage import convolve


def load_grid(fname: str):
    gridout = []
    with open(fname, "r") as f:
        for line in f:
            gridout.append([mapper[_] for _ in line.strip()])
    grid_raw = np.array(gridout, dtype=np.int8)
    mask = grid_raw < 0
    grid_raw[mask] = 0
    return np.ma.masked_where(mask, grid_raw)


def get_n_neighbours(grid):
    weights = np.array([[1,1,1],
                        [1,0,1],
                        [1,1,1]])
    n_neighbours = convolve(grid, weights, mode="constant", cval=0)
    return np.ma.masked_array(n_neighbours, grid.mask)

def get_nearest_chair_neighbours(grid):
    offgrid = lambda s, dr : (int(s) < 0) or (int(s) >= grid.shape[dr])
    out =np.zeros_like(grid)
    def nearest_chair_values(grid, x, y):
        nearvals = []
        xdirec = [-1,0,1]
        ydirec = [-1,0,1]
        basepos = np.array([x,y], dtype=np.int16)
        for xd in xdirec:
            for yd in ydirec:
                if xd == 0 and yd ==0:
                    continue
                vec = np.array([xd,yd],dtype=np.int16)
                i = 1 
                while True:
                    newpos = i*vec + basepos
                    #print(newpos)
                    i+=1
                    if offgrid(newpos[0],0) or offgrid(newpos[1],1):
                        break
                    if grid.mask[tuple(newpos)] == False:
                        #print("got one")
                        nearvals.append(grid[tuple(newpos)])
                        break
                #print("---------")
        return nearvals

    for x in range(grid.shape[0]):
        for y in range(grid.shape[1]):
            nvals = nearest_chair_values(grid,x,y)
            #print("#####################")
            out[x,y] = np.sum(nvals)
    out.mask = grid.mask
    return out


def update_grid(grid, neighbourfunc = get_n_neighbours, max_occupancy=4):
    n_neighbours = neighbourfunc(grid)
    gridout = grid.copy()
    gridout[np.logical_and(grid, n_neighbours >=max_occupancy )]  = 0
    gridout[np.logical_and(grid==0, n_neighbours==0)] = 1
    return gridout


def find_stationary_grid(grid, *args, **kwargs):
    grid_copy  = grid.copy()
    grid_upd = update_grid(grid, *args, **kwargs)
    i=0
    while True:
        print(f"iteraetion {i}")
        i+=1
        if(np.ma.all(grid_upd == grid_copy)):
            break
        grid_copy = grid_upd
        grid_upd = update_grid(grid_upd, *args, **kwargs)
    print(i)
    return grid_upd


grid1 = load_grid("input.txt")
stat_grid = find_stationary_grid(grid1)
print(np.count_nonzero(stat_grid))
print("------")
stat_grid2 = find_stationary_grid(grid1, get_nearest_chair_neighbours, 5)
print(np.count_nonzero(stat_grid2))


# nn2 = get_nearest_chair_neighbours(grid2_test[:3,:3])
# print("grid2_test")
# print(grid2_test)
# print("nn2")
# print(nn2)



