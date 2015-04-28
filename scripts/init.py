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

FILEPATH = os.path.dirname(os.path.realpath(__file__)) + '\\clfs\\'

print("Loading Board Control PLAY clf...")
sys.stdout.flush()
clf_bc_play = joblib.load(FILEPATH + "boardCtrl." + hu.dbs[0])
bc_playMask = clf_bc_play.classes_ == 1
clf_bc_play.n_jobs = 1

print("Loading Board Control TARGET clf...")
sys.stdout.flush()
clf_bc_target = joblib.load(FILEPATH + "boardCtrl." + hu.dbs[1])
bc_targetMask = clf_bc_target.classes_ == 1
clf_bc_target.n_jobs = 1

print("Loading Aggro PLAY clf...")
sys.stdout.flush()
clf_aggro_play = joblib.load(FILEPATH + "aggro." + hu.dbs[0])
bc_playMask = clf_aggro_play.classes_ == 1
clf_aggro_play.n_jobs = 1

print("Loading Aggro TARGET clf...")
sys.stdout.flush()
clf_aggro_target = joblib.load(FILEPATH + "aggro." + hu.dbs[1])
aggro_targetMask = clf_aggro_target.classes_ == 1
clf_aggro_target.n_jobs = 1

print("Ready to go!")
sys.stdout.flush()

def predBCPlay(playActions):
    samples = np.array(playActions)
    #print("Predicting for {} targeted actions...".format(samples.shape[0]))
    #sys.stdout.flush()
    y = clf_bc_play.predict_proba(samples)
    return y[:, bc_playMask].flatten().tolist()
    
def predBCTarget(targetActions):
    samples = np.array(targetActions)
    #print("Predicting for {} targeted actions...".format(samples.shape[0]))
    #sys.stdout.flush()
    y = clf_bc_target.predict_proba(samples)
    return y[:, bc_targetMask].flatten().tolist()

def predAggroPlay(playActions):
    samples = np.array(playActions)
    #print("Predicting for {} targeted actions...".format(samples.shape[0]))
    #sys.stdout.flush()
    y = clf_bc_play.predict_proba(samples)
    return y[:, bc_playMask].flatten().tolist()
    
def predAggroTarget(targetActions):
    samples = np.array(targetActions)
    #print("Predicting for {} targeted actions...".format(samples.shape[0]))
    #sys.stdout.flush()
    y = clf_aggro_target.predict_proba(samples)
    return y[:, aggro_targetMask].flatten().tolist()
