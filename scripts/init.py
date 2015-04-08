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
import sys
import os

FILEPATH = os.path.dirname(os.path.realpath(__file__)) + '\\'

print("Loading PLAY clf...")
sys.stdout.flush()
clf_play = joblib.load(FILEPATH + "clfs\\" + hu.dbs[0])
playMask = clf_play.classes_ == 1
clf_play.n_jobs = 1

print("Loading TARGET clf...")
sys.stdout.flush()
clf_target = joblib.load(FILEPATH + "clfs\\" + hu.dbs[1])
targetMask = clf_target.classes_ == 1
clf_target.n_jobs = 1

print("Ready to go!")
sys.stdout.flush()

def predPlay(playActions):
    samples = np.array(playActions)
    #print("Predicting for {} targeted actions...".format(samples.shape[0]))
    #sys.stdout.flush()
    y = clf_play.predict_proba(samples)
    return y[:, playMask].tolist()
    
def predTarget(targetActions):
    samples = np.array(targetActions)
    #print("Predicting for {} targeted actions...".format(samples.shape[0]))
    #sys.stdout.flush()
    y = clf_target.predict_proba(samples)
    return y[:, targetMask].tolist()
