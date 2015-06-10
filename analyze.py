#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# analyze.py

import os
import pickle
from glob import glob
import numpy as np
import matplotlib.pyplot as plt


RESULT_CATEGORY = '3-4-6-5'
RESULT_DIR = os.path.join('raw_results', RESULT_CATEGORY)
LOD_FILES = glob(os.path.join(RESULT_DIR, '*/LOD.csv'))

COMPILED_RESULT_DIR = os.path.join('compiled_results', '3-4-6-5')

CORRECT_COUNTS_FILEPATH = os.path.join(COMPILED_RESULT_DIR,
                                       'correct_counts.pkl')

COMPILED_LODS_FILEPATH = os.path.join(COMPILED_RESULT_DIR, 'lods.pkl')

if not os.path.exists(COMPILED_RESULT_DIR):
    os.makedirs(COMPILED_RESULT_DIR)


def save_lods(input_filepaths=LOD_FILES,
              output_filepath=COMPILED_LODS_FILEPATH):
    lods = {}
    for f in LOD_FILES:
        print('Processing ' + f + '...')
        try:
            lods[f] = np.genfromtxt(f, delimiter=',', dtype=int, names=True)
        except (StopIteration, IndexError):
            print('\tEmpty file: ' + f)
    with open(output_filepath, 'wb') as f:
        pickle.dump(lods, f)
    return lods


def load_lods(input_filepath=COMPILED_LODS_FILEPATH):
    if os.path.exists(input_filepath):
        with open(input_filepath, 'rb') as f:
            return pickle.load(f)
    return save_lods(input_filepath)


def save_correct_counts(output_filepath=CORRECT_COUNTS_FILEPATH):
    lods = load_lods()
    # Check that the same number of generations were used for every seed.
    ngen = lods[list(lods.keys())[0]]['gen'][-1]
    assert all(ngen == lod[-1][0] for lod in lods.values())
    # Get the number of correct trials at the last generation.
    data = {
        'correct': np.array([lod['correct'][-1] for lod in lods.values()]),
        'ngen': ngen
    }
    with open(output_filepath, 'wb') as f:
        pickle.dump(data, f)
        print('Saved analysis to `{}`.'.format(output_filepath))
    return data


def plot_lods(input_filepaths=LOD_FILES, avg=True):
    lods = list(load_lods().values())
    ngen = lods[0]['gen'][-1]
    data = np.array([lod['correct'] for lod in lods])
    if avg:
        plt.plot(lods[0]['gen'], data.mean(0))
    else:
        for lod in lods:
            plt.plot(lod['gen'], lod['correct'])
    plt.xlabel('$\mathrm{Generation}$', fontsize=22)
    plt.ylabel('$\mathrm{Correct\ trials}$', fontsize=22)
    plt.title('$\mathrm{Animat\ Fitness\ over\ ' + str(ngen) +
              '\ generations,\ population\ size\ 100}$', fontsize=22)
    plt.ylim([60, 130])
    plt.grid(True)
    plt.show()


def load_correct_counts(input_filepath=CORRECT_COUNTS_FILEPATH):
    with open(input_filepath, 'rb') as f:
        return pickle.load(f)


def plot_correct_counts(input_filepath=CORRECT_COUNTS_FILEPATH,
                        bins=np.arange(64, 128, 2)):
    data = load_correct_counts(input_filepath)
    correct, ngen = data['correct'], data['ngen']
    plt.hist(correct, bins, normed=True, alpha=0.5)
    plt.xlabel('$\mathrm{Fitness}$', fontsize=22)
    plt.ylabel('$\mathrm{Number\ of\ Animats}$', fontsize=22)
    plt.title('$\mathrm{Histogram\ of\ Animat\ Performance\ after\ '
              + str(ngen) + '\ generations,\ population\ size\ 100}$',
              fontsize=22)
    plt.grid(True)

    plt.show()
