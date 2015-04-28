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
import hearthstone_utils as hu
from sklearn.externals import joblib
from sklearn.ensemble import ExtraTreesClassifier
import numpy as np
  
if __name__ == "__main__":
    print("Loading data")
    loaded = hu.loadClassifiedDB("db1.play.train.csv", random_state=0)
    loaded = np.vstack((loaded, hu.loadClassifiedDB("db2.play.train.csv", random_state=0)))

    print("Fitting classifier...")
    clf = ExtraTreesClassifier(n_estimators=100, random_state=0, n_jobs=-1, verbose=50)
    clf.fit(loaded[:, 0:-1], loaded[:, -1])
    
    print("Saving classifier...")
    if (os.path.exists("clfs/") == False):
        os.mkdir("clfs")
    joblib.dump(clf, "clfs/db.play")
    
    
    print("Loading data")
    loaded = hu.loadClassifiedDB("db1.target.train.csv", random_state=0)
    loaded = np.vstack((loaded, hu.loadClassifiedDB("db2.target.train.csv", random_state=0)))

    print("Fitting classifier...")
    clf = ExtraTreesClassifier(n_estimators=100, random_state=0, n_jobs=-1, verbose=50)
    clf.fit(loaded[:, 0:-1], loaded[:, -1])
    
    print("Saving classifier...")
    if (os.path.exists("clfs/") == False):
        os.mkdir("clfs")
    joblib.dump(clf, "clfs/db.target")
