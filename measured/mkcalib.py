#!/usr/bin/env python

import os
import numpy as np
from math import exp
os.chdir(os.path.dirname(__file__))

profiles = []

def find_nearest(array, value):
    array = np.asarray(array)
    idx = (np.abs(array - value)).argmin()
    return idx


with open("measured.csv", "r") as f:
    lines = f.readlines()
    measured_profile = [float(x.strip().split(',')[-1]) for x in lines]


mag = 1.0

def fn(x):
    return mag/(1.0+exp(-10*((x/256.0)-0.6)))

values = [fn(x) for x in range(0,256)]

print("float tc0060dca_mapping[] = {")
for i in range(0, 256):
    value = values[i]

    print("%f" % (value,),end=",")
    if i % 16 == 15:
        print()
print("};")
