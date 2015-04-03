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
    removedCols = np.zeros(14, dtype=int)
    j = 0
    for i in range(0, 14):
        offset = j * 6
        removedCols[j] = offset + 4
        removedCols[j] = offset + 5
        j += 1
    makeTest("no_silence_enchant", removedCols)
    
    
    removedCols = np.zeros(14, dtype=int)
    j = 0
    for i in range(0, 14):
        offset = j * 6
        removedCols[j] = offset + 1
        removedCols[j] = offset + 4
        removedCols[j] = offset + 5
        j += 1
    makeTest("no_silence_enchant_canattack", removedCols)
    
    
    removedCols = np.array([155, 156, 157], dtype=int)
    makeTest("no_manainfo", removedCols)
    
    
    removedCols = np.array([157], dtype=int)
    makeTest("no_enemymana", removedCols)
    
    
    removedCols = np.array([151, 152], dtype=int)
    makeTest("no_minioncounts", removedCols)
    
    
    makeTest("all")

  
#    # ALL FEATURES
#    random_state = check_random_state(0)
#    hu.roc_precision(hu.dbs[1], random_state=random_state, test="all")
#    
#    
#    # Not taking silence/enchant features for board minions
#    random_state = check_random_state(0)
#    useCols = np.arange(hu.n_features[hu.dbs[1]] + 1)
#    mask = np.ones_like(useCols, dtype=bool)
#    for i in range(0, 14):
#        offset = i * 6
#        mask[offset + 4] = mask[offset + 5] = False
#    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_silence_enchant")
#    
#    
#    # Not taking silence/enchant/can_attack features for board minions
#    random_state = check_random_state(0)
#    useCols = np.arange(hu.n_features[hu.dbs[1]] + 1)
#    mask = np.ones_like(useCols, dtype=bool)
#    for i in range(0, 14):
#        offset = i * 6
#        mask[offset + 1] = mask[offset + 4] = mask[offset + 5] = False
#    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_silence_enchant_canattack")
#    
#    
#    # Not taking any feature relative to players' mana
#    random_state = check_random_state(0)
#    useCols = np.arange(hu.n_features[hu.dbs[1]] + 1)
#    mask = np.ones_like(useCols, dtype=bool)
#    mask[155] = mask[156] = mask[157] = False
#    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_manainfo")
#    
#    
#    # Not taking enemy player max mana
#    random_state = check_random_state(0)
#    useCols = np.arange(hu.n_features[hu.dbs[1]] + 1)
#    mask = np.ones_like(useCols, dtype=bool)
#    mask[157] = False
#    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_enemymana")
#    
#    
#    # Not taking minions on board count features
#    random_state = check_random_state(0)
#    useCols = np.arange(hu.n_features[hu.dbs[1]] + 1)
#    mask = np.ones_like(useCols, dtype=bool)
#    mask[151] = mask[152] = False
#    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_minioncounts")
