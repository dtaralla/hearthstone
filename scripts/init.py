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
from sklearn.externals import joblib
from sklearn.ensemble import ExtraTreesClassifier
import hearthstone_utils as hu

print("Loading PLAY clf...")
clf_play = joblib.load("clfs/" + hu.dbs[0])
playMask = clf_play.classes_ == 1

print("Loading PLAY clf...")
clf_target = joblib.load("clfs/" + hu.dbs[1])
targetMask = clf_target.classes_ == 1

print("Ready to go!")

def predPlay(playActions):
    print(playActions)
    
    samples = np.array(playActions)
    print("Predicting for {} targeted actions...".format(samples.shape[0]))
    y = clf_play.predict_proba(samples)
    return y[:, playMask].toList()
    
def predTarget(targetActions):
    print(targetActions)
    
    samples = np.array(targetActions)
    print("Predicting for {} targeted actions...".format(samples.shape[0]))
    y = clf_target.predict_proba(samples)
    return y[:, targetMask].toList()
