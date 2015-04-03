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
clf_play   = joblib.load("clfs/" + hu.dbs[0])

print("Loading PLAY clf...")
clf_target = joblib.load("clfs/" + hu.dbs[1])

print("Ready to go!")
