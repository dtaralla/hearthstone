#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: David Taralla

# Only py3 / so that 2 / 3 = 0.66..
from __future__ import division
# Only py3 string encoding
from __future__ import unicode_literals
# Only py3 print
from __future__ import print_function

import os
import numpy as np
from sklearn.externals import joblib
from sklearn.ensemble import ExtraTreesRegressor
from sklearn.utils import check_random_state
import hearthstone_utils as hu
  
if __name__ == "__main__":
    for db in hu.dbs:
        print("Loading classifier {}...".format(db))
        clf = joblib.load("clfs/" + db)
        
        print("Loading training data...")
        loaded = hu.loadDB(db + ".train.csv")
        X_train = loaded[:, 0:-2]
        y_train = loaded[:, -1]
        
        print("Loading test data...")
        loaded = hu.loadDB(db + ".csv")
        X_test = loaded[:, 0:-2]
        y_test = loaded[:, -1]
        
        print("Computing pValue...")
        hu.pValue(X_train, y_train, X_test, y_test, clf, 20, 0)

