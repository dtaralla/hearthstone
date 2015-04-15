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
import sys

def makeTest(testName, removedCols=None, prefix="db", db=1, randomState=0):
    random_state = check_random_state(randomState)
    useCols = np.arange(hu.n_features[hu.dbs[db]] + 1)
    mask = np.ones_like(useCols, dtype=bool)
    if (removedCols == None):
        hu.roc_precision(prefix + "." + hu.dbsCustom[db], random_state=random_state, test=testName)
    else:
        mask[removedCols] = False
        hu.roc_precision(prefix + "." + hu.dbsCustom[db], random_state=random_state, usecols=useCols[mask], test=testName)
  
if __name__ == "__main__":
    dbCount = 1
    if (len(sys.argv) > 1):
        dbCount = int(sys.argv[1])
    
    # TARGET database tests
    #removedCols = np.zeros(14, dtype=int)
    #j = 0
    #for i in range(0, 14):
    #    offset = j * 6
    #    removedCols[j] = offset + 4
    #    removedCols[j] = offset + 5
    #    j += 1
    #if (dbCount <= 1):
    #    makeTest("no_silence_enchant", removedCols)
    #else:
    #    for i in range(1, dbCount+1):
    #        makeTest("no_silence_enchant", removedCols, prefix="db" + str(i))
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
    #if (dbCount <= 1):
    #    makeTest("no_silence_enchant_canattack", removedCols)
    #else:
    #    for i in range(1, dbCount+1):
    #        makeTest("no_silence_enchant_canattack", removedCols, prefix="db" + str(i))
    #    
    #
    #removedCols = np.array([155, 156, 157], dtype=int)
    #if (dbCount <= 1):
    #    makeTest("no_manainfo", removedCols)
    #else:
    #    for i in range(1, dbCount+1):
    #        makeTest("no_manainfo", removedCols, prefix="db" + str(i))
        
    
    if (dbCount <= 1):
        makeTest("all")
    else:
        for i in range(1, dbCount+1):
            makeTest("all", prefix="db" + str(i))
        
    # PLAY database test
    removedCols = np.zeros(14, dtype=int)
    j = 0
    for i in range(0, 14):
        offset = j * 6
        removedCols[j] = offset + 4
        removedCols[j] = offset + 5
        j += 1
    if (dbCount <= 1):
        makeTest("no_silence_enchant", removedCols, db=0)
    else:
        for i in range(1, dbCount+1):
            makeTest("no_silence_enchant", removedCols, prefix="db" + str(i), db=0)
        
    
    removedCols = np.zeros(14, dtype=int)
    j = 0
    for i in range(0, 14):
        offset = j * 6
        removedCols[j] = offset + 1
        removedCols[j] = offset + 4
        removedCols[j] = offset + 5
        j += 1
    if (dbCount <= 1):
        makeTest("no_silence_enchant_canattack", removedCols, db=0)
    else:
        for i in range(1, dbCount+1):
            makeTest("no_silence_enchant_canattack", removedCols, prefix="db" + str(i), db=0)
        
        
    removedCols = np.array([155, 156, 157], dtype=int)
    if (dbCount <= 1):
        makeTest("no_manainfo", removedCols, db=0)
    else:
        for i in range(1, dbCount+1):
            makeTest("no_manainfo", removedCols, prefix="db" + str(i), db=0)
        
    
    if (dbCount <= 1):
        makeTest("all")
    else:
        for i in range(1, dbCount+1):
            makeTest("all", db=0, prefix="db" + str(i))
        
    