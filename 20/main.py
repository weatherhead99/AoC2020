#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Mon Dec 21 16:50:15 2020

@author: danw
"""
import numpy as np
from scipy.spatial.distance import jaccard
from typing import Dict, Tuple

def parse_grids(fname:str) -> Dict[int, np.ndarray]:
    with open(fname, "r") as f:
        dat = f.read().split("\n\n")
    grids_out = {}
    for grid_in in dat:
        grid_lines = grid_in.split("\n")
        grid_num = int(grid_lines[0].split("Tile ")[1].split(":")[0])
        grid_array = np.array([list(_) for _ in grid_lines[1:]])
        grids_out[grid_num] = grid_array == "#"
    return grids_out


def edge_distance(edge1: np.ndarray, edge2: np.ndarray) -> float:
    a = jaccard(edge1, edge2)
    b = jaccard(edge1[::-1], edge2)
    m = min(a,b)
    if m == a:
        return m, False
    else:
        return m, True

def get_edges(grid: np.ndarray) -> Tuple[np.ndarray]:
    #edges in clockwise order north first
    return  grid[0,:], grid[:,-1], grid[-1,:], grid[:,0]


def search_single_edge(gridkey: int, tgtgrid: int, allgrids):
    grid = allgrids[gridkey]
    edges = get_edges(grid)
    found = []
    for idx1, edge1 in enumerate(edges):
        for idx2, edge2 in enumerate(get_edges(tgtgrid)):
            jc = edge_distance(edge1, edge2)
            if(jc[0] ==0.0):
                found.append((idx1, idx2, k, jc[1]))
    return found

def search_edges(gridkey: int, allgrids: Dict[int, np.ndarray]):
    found = []
    for k,v in allgrids.items():
        if k == gridkey:
            continue
        tgtgrid = allgrids[k]
        found.extend(search_single_edge(gridkey, tgtgrid, allgrids))
    return found

def rotate_flip_tile(adjlist, tgt_grid):
    pass


if __name__ == "__main__":
    grids = parse_grids("input2.txt")
    edge = int(np.sqrt(len(grids)))
    tile_locations =  np.zeros((edge,edge), dtype=np.uint16)
    
    
    alljclists = {}
    for k,v in grids.items():
        jclist = search_edges(k,grids)
        alljclists[k] = jclist
    
    
    corners = [k for k, v in alljclists.items() if len(v) ==2]
    edges  = [k for k, v in alljclists.items() if len(v) ==3]
    middles = [k for k, v in alljclists.items() if len(v) ==4]
    
#%%
    #top left corner has edges 1 & 2 connected
    topleft = [_ for _ in corners if alljclists[_][0][0]==1 and alljclists[_][1][0]==2]
    tile_locations[0,0] = topleft[0]
    
    
    #part 1
    #---------------------------
    #corners = []
    # for k,v in grids.items():
    #     jclist = search_edges(k,grids)
    #     if len(jclist) ==2:
    #         corners.append(k)
    # print(f"corner id product: {np.product(corners)}")
