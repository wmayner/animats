#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# analyze.py

import os
import pickle
from glob import glob
import numpy as np
import matplotlib.pyplot as plt


RESULT_DIR = os.path.join('raw_results', '3-4-6-5')
LOD_FILES = glob(os.path.join(RESULT_DIR, '*/LOD.csv'))

CORRECT_COUNTS_FILEPATH = os.path.join('compiled_results',
                                       'correct_counts.pkl')


def save_correct_counts(output_filepath=CORRECT_COUNTS_FILEPATH):
    lods = {}
    for f in LOD_FILES:
        print('Processing ' + f + '...')
        try:
            lods[f] = np.genfromtxt(f, delimiter=',', dtype=int, skip_header=1)
        except StopIteration:
            print('\tEmpty file: ' + f)
    # Check that the same number of generations were used for every seed.
    ngen = lods[list(lods.keys())[0]][-1][0]
    assert all(ngen == lod[-1][0] for lod in lods.values())
    # Get the number of correct trials at the last generation.
    data = {
        'correct': np.array([lod[(-1, 1)] for lod in lods.values()]),
        'ngen': ngen
    }
    with open(output_filepath, 'wb') as f:
        pickle.dump(data, f)
        print('Saved analysis to `{}`.'.format(output_filepath))
    return data


def load_correct_counts(input_filepath=CORRECT_COUNTS_FILEPATH):
    with open(input_filepath, 'rb') as f:
        return pickle.load(f)


def plot_correct_counts(input_filepath=CORRECT_COUNTS_FILEPATH):
    with open(input_filepath, 'rb') as f:
        data = pickle.load(f)
    correct, ngen = data['correct'], data['ngen']
    n, bins, patches = plt.hist(correct, 20, normed=True, facecolor='blue',
                                alpha=0.8)
    plt.xlabel('$\mathrm{Fitness}$')
    plt.ylabel('$\mathrm{Number\ of\ Animats}$')
    plt.title('$\mathrm{Histogram\ of\ Animat\ Performance\ after\ '
              + str(ngen) + '\ generations,\ population\ size\ 100}$')
    plt.grid(True)

    plt.show()


def plot_lods(input_filepaths=LOD_FILES)
