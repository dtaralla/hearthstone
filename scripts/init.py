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
import time

FILEPATH = os.path.dirname(os.path.realpath(__file__)) + '\\clfs\\'

start_t = time.clock()
print("Loading Board Control PLAY clf...")
sys.stdout.flush()
clf_bc_play = joblib.load(FILEPATH + hu.dbs[0])
bc_playMask = clf_bc_play.classes_ == 1
clf_bc_play.n_jobs = 1

print("Loading Board Control TARGET clf...")
sys.stdout.flush()
clf_bc_target = joblib.load(FILEPATH + hu.dbs[1])
bc_targetMask = clf_bc_target.classes_ == 1
clf_bc_target.n_jobs = 1

print("Loading Board Control ATTACK clf...")
sys.stdout.flush()
clf_bc_atk = joblib.load(FILEPATH + hu.dbs[2])
bc_atkMask = clf_bc_atk.classes_ == 1
clf_bc_atk.n_jobs = 1

#print("Loading Aggro PLAY clf...")
#sys.stdout.flush()
#clf_aggro_play = joblib.load(FILEPATH + hu.dbs[2])
#bc_playMask = clf_aggro_play.classes_ == 1
#clf_aggro_play.n_jobs = 1
#
#print("Loading Aggro TARGET clf...")
#sys.stdout.flush()
#clf_aggro_target = joblib.load(FILEPATH + hu.dbs[3])
#aggro_targetMask = clf_aggro_target.classes_ == 1
#clf_aggro_target.n_jobs = 1

print("Ready to go after {} seconds".format(time.clock() - start_t))
sys.stdout.flush()

def predBCPlay(playActions):
    samples = np.array(playActions)
    y = clf_bc_play.predict_proba(samples)
    return y[:, bc_playMask].flatten().tolist()
    
def predBCTarget(targetActions):
    samples = np.array(targetActions)
    y = clf_bc_target.predict_proba(samples)
    return y[:, bc_targetMask].flatten().tolist()
    
def predBCAtk(atkActions):
    samples = np.array(atkActions)
    y = clf_bc_atk.predict_proba(samples)
    return y[:, bc_atkMask].flatten().tolist()

def predAggroPlay(playActions):
    samples = np.array(playActions)
    y = clf_bc_play.predict_proba(samples)
    return y[:, bc_playMask].flatten().tolist()
    
def predAggroTarget(targetActions):
    samples = np.array(targetActions)
    y = clf_aggro_target.predict_proba(samples)
    return y[:, aggro_targetMask].flatten().tolist()
