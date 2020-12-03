#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Thu Dec  3 17:31:05 2020

@author: danw
"""
import os
import sys
import requests
from argparse import ArgumentParser

YEAR = 2020

if __name__ == "__main__":
    ap = ArgumentParser("setup AoC puzzle for c++")
    ap.add_argument("puzzle_number", type=int)
    args = ap.parse_args()
    
    if not os.path.exists(".secret"):
        print("no .secret file exists, please provide it...")
        sys.exit(1)
    with open(".secret", "r") as f:
        secret = f.read().strip()
    
    puzzle_dir = os.path.join(".", str(args.puzzle_number))
    
    print(f"setting up for puzzle number {args.puzzle_number}")
    print("making directory...")
    if not os.path.exists(puzzle_dir):
        os.mkdir(puzzle_dir)
    else:
        print("directory already exists...")
    
    input_file = os.path.join(puzzle_dir, "input.txt")
    if not os.path.exists(input_file):
        print("downloading input file...")
        cookie = {"session" : secret}
        requrl = f"https://adventofcode.com/{YEAR}/day/{args.puzzle_number}/input"
        ret = requests.get(requrl, cookies=cookie)
        if ret.status_code == 404:
            print("couldn't download input file, maybe puzzle isn't available yet")
            sys.exit(1)
        with open(input_file, "w") as f:
            f.write(ret.text)
    else:
        print("input file exists already!")


