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

def predBCPlay(playActions):
    return 0
    
def predBCTarget(targetActions):
    return 0
    
def predBCAtk(atkActions):
    return 0

def predAggroPlay(playActions):
    return 0
    
def predAggroTarget(targetActions):
    return 0
