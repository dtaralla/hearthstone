#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: David Taralla

# Only py3 / so that 2 / 3 = 0.66..
from __future__ import division
# Only py3 string encoding
from __future__ import unicode_literals
# Only py3 print
from __future__ import print_function

import numpy as np
from sklearn.utils import check_random_state
import scipy.io as sio
import hearthstone_utils as hu

def makeTest(testName, removedCols=None, db=1, randomState=0):
    random_state = check_random_state(randomState)
    useCols = np.arange(hu.n_features[hu.dbs[db]] + 1)
    mask = np.ones_like(useCols, dtype=bool)
    if (removedCols == None):
        hu.roc_precision(hu.dbs[db], random_state=random_state, test=testName)
    else:
        mask[removedCols] = False
        hu.roc_precision(hu.dbs[db], random_state=random_state, usecols=useCols[mask], test=testName)
  
if __name__ == "__main__":   
    # TARGET database tests
    #removedCols = np.zeros(14, dtype=int)
    #j = 0
    #for i in range(0, 14):
    #    offset = j * 6
    #    removedCols[j] = offset + 4
    #    removedCols[j] = offset + 5
    #    j += 1
    #makeTest("no_silence_enchant", removedCols)
    #
    #
    #removedCols = np.zeros(14, dtype=int)
    #j = 0
    #for i in range(0, 14):
    #    offset = j * 6
    #    removedCols[j] = offset + 1
    #    removedCols[j] = offset + 4
    #    removedCols[j] = offset + 5
    #    j += 1
    #makeTest("no_silence_enchant_canattack", removedCols)
    #
    #
    #removedCols = np.array([155, 156, 157], dtype=int)
    #makeTest("no_manainfo", removedCols)
    #
    #
    #removedCols = np.array([157], dtype=int)
    #makeTest("no_enemymana", removedCols)
    #
    #
    #removedCols = np.array([151, 152], dtype=int)
    #makeTest("no_minioncounts", removedCols)
    #
    #makeTest("all")
    
    # PLAY database test
    makeTest("all", db=0)
    