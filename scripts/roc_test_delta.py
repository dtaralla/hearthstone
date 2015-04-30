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

def makeTest(testName, db, n_features, randomState=0):
    random_state = check_random_state(randomState)
    useCols = np.arange(n_features + 1)
    hu.roc_precision(db, random_state=random_state, test=testName)
  
if __name__ == "__main__":
    makeTest("all", hu.dbs[0], hu.n_features[hu.dbs[0]])
    makeTest("all", hu.dbs[1], hu.n_features[hu.dbs[1]])
    