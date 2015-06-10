#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# analyze_old.py

import os
import pickle
from glob import glob
import numpy as np
import matplotlib.pyplot as plt


RESULT_CATEGORY = 'old-code-test'
RESULT_DIR = os.path.join('raw_results', RESULT_CATEGORY)
LOD_FILES = glob(os.path.join(RESULT_DIR, '*/LOD.txt'))

COMPILED_RESULT_DIR = os.path.join('compiled_results', 'old-code-test')

CORRECT_COUNTS_FILEPATH = os.path.join(COMPILED_RESULT_DIR,
                                       'correct_counts.pkl')

COMPILED_LODS_FILEPATH = os.path.join(COMPILED_RESULT_DIR, 'lods.pkl')

if not os.path.exists(COMPILED_RESULT_DIR):
    os.makedirs(COMPILED_RESULT_DIR)


def save_lods(input_filepath=LOD_FILES,
              output_filepath=COMPILED_LODS_FILEPATH):
    lods = {}
    for f in LOD_FILES:
        print('Processing ' + f + '...')
        lods[f] = np.genfromtxt(f, delimiter=' ', dtype=int)
    with open(output_filepath, 'wb') as f:
        pickle.dump(lods, f)
    return lods


def load_lods(input_filepath=COMPILED_LODS_FILEPATH):
    if os.path.exists(input_filepath):
        with open(input_filepath, 'rb') as f:
            return pickle.load(f)
    return save_lods(input_filepath)


def plot_lods(input_filepath=COMPILED_LODS_FILEPATH, avg=True):
    lods = list(load_lods(input_filepath).values())
    ngen = lods[0][-1, 0]
    data = np.array([lod[:, 3] for lod in lods])
    if avg:
        plt.plot(lods[0][:, 0], data.mean(0))
    else:
        for lod in lods:
            plt.plot(lod[:, 0], lod[:, 3])
    plt.xlabel('$\mathrm{Generation}$', fontsize=22)
    plt.ylabel('$\mathrm{Correct\ trials}$', fontsize=22)
    plt.title('$\mathrm{OLD\ CODE}$\n$\mathrm{Animat\ Fitness\ over\ '
              + str(ngen) + '\ generations,\ population\ size\ 100}$',
              fontsize=22)
    plt.grid(True)
    plt.show()


def save_correct_counts(output_filepath=CORRECT_COUNTS_FILEPATH):
    lods = load_lods()
    ngen = lods[list(lods.keys())[0]][-1, 0]
    # Get the number of correct trials at the last generation.
    data = {
        'correct': np.array([lod[-1, 3] for lod in lods.values()]),
        'ngen': ngen
    }
    with open(output_filepath, 'wb') as f:
        pickle.dump(data, f)
        print('Saved analysis to `{}`.'.format(output_filepath))
    return data


def load_correct_counts(input_filepath=CORRECT_COUNTS_FILEPATH):
    if os.path.exists(input_filepath):
        with open(input_filepath, 'rb') as f:
            return pickle.load(f)
    return save_correct_counts()


def plot_correct_counts(input_filepath=CORRECT_COUNTS_FILEPATH,
                        bins=np.arange(64, 128, 2)):
    data = load_correct_counts(input_filepath)
    correct, ngen = data['correct'], data['ngen']
    plt.hist(correct, bins, normed=True, alpha=0.5)
    plt.xlabel('$\mathrm{Fitness}$', fontsize=22)
    plt.ylabel('$\mathrm{Number\ of\ Animats}$', fontsize=22)
    plt.title('$\mathrm{OLD\ CODE}$\n$\mathrm{Histogram\ of\ Animat\ '
              'Performance\ after\ ' + str(ngen) +
              '\ generations,\ population\ size\ 100}$', fontsize=22)
    plt.ylim([60, 130])
    plt.grid(True)

    plt.show()
