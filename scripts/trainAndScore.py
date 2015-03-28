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
    hu.trainClassifiersAndSave(True);

