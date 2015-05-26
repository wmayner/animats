#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# analyze.py

import os
import pickle
from glob import glob
import numpy as np


RESULT_DIR = os.path.join('raw_results', '3-4-6-5')
LOD_FILES = glob(os.path.join(RESULT_DIR, '*/LOD.csv'))

CORRECT_COUNTS_FILEPATH = os.path.join('compiled_results',
                                       'correct_counts.pkl')


def save_correct_counts(output_filepath=CORRECT_COUNTS_FILEPATH):
    lods = {}
    for f in LOD_FILES:
        print('Processing ' + f + '...')
        lods[f] = np.genfromtxt(f, delimiter=',', dtype=int, skip_header=1)
    # Get the number of correct trials at the last generation.
    correct = np.array([lods[f][(-1, 1)] for f in lods])
    with open(output_filepath, 'wb') as f:
        pickle.dump(correct, f)
        print('Saved analysis to `{}`.'.format(output_filepath))
    return correct


def load_correct_counts(input_filepath=CORRECT_COUNTS_FILEPATH):
    with open(input_filepath, 'rb') as f:
        return pickle.load(f)
