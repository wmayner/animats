#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# plot.py

import os
import pickle
import matplotlib.pyplot as plt

COMPILED_RESULTS_DIR = 'compiled_results'

with open(os.path.join(COMPILED_RESULTS_DIR, 'correct_counts.pkl'), 'rb') as f:
    data = pickle.load(f)
n, bins, patches = plt.hist(data, 12, normed=True, facecolor='blue', alpha=0.8)

plt.xlabel('$\mathrm{Fitness}$')
plt.ylabel('$\mathrm{Number\ of\ Animats}$')
plt.title('$\mathrm{Histogram\ of\ Animat Performance:\ 30,000\ '
          'generations,\ population\ size\ 100}$')
plt.grid(True)

plt.show()
