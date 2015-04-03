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
import glob
import numpy as np
from sklearn.externals import joblib
from sklearn.ensemble import ExtraTreesRegressor
from sklearn.ensemble import ExtraTreesClassifier
from sklearn.utils import check_random_state
from matplotlib import pyplot as plt
import scipy.io as sio
import hearthstone_utils as hu
  
if __name__ == "__main__":
    for f in glob.glob("roc/*.roc.*.mat"):
        print(f)
        contents = sio.loadmat(f)
        plt.plot(contents['fpr'][0], contents['tpr'][0], label=f)
        
    plt.title("ROC")
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.legend()
    plt.show()
    
    for f in glob.glob("roc/*.precall.*.mat"):
        print(f)
        contents = sio.loadmat(f)
        plt.plot(contents['recall'][0], contents['precision'][0], label=f)
        
    plt.title("Precision/Recall")
    plt.xlabel("Recall (TP / (TP+FN))")
    plt.ylabel("Precision (TP / (TP + FP))")
    plt.legend()
    plt.show()
