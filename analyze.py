#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# analyze.py

import pickle
from glob import glob
import numpy as np

lod_files = (glob('results/current/*/LOD.csv'))

lods = {}
for f in lod_files:
    print('Processing ' + f + '...')
    lods[f] = np.genfromtxt(f, delimiter=',', dtype=int, skip_header=1)

# Get the number of correct trials at the last generation.
correct = np.array([lods[f][(-1, 1)] for f in lods])

DATA_FILE = 'data.pkl'
with open(DATA_FILE, 'wb') as f:
    pickle.dump(correct, f)
    print('Saved analysis to `{}`.'.format(DATA_FILE))
