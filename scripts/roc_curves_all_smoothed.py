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
import sys
  
if __name__ == "__main__":
    prefix = sys.argv[1]
    dbCount = int(sys.argv[2])
        
    for i in range(1, dbCount + 1):
        f = "roc/all.roc.{}{}.play.mat".format(prefix, i)
        contents = sio.loadmat(f)
        plt.plot(contents['fpr'][0], contents['tpr'][0], label=f)
        i = 0
        for t in contents['thresholds'][0]:
            plt.annotate(t, 
                         xy=(contents['fpr'][0][i], contents['tpr'][0][i]), 
                         xytext=(20, -20), 
                         textcoords = 'offset points', 
                         ha = 'left', 
                         va = 'top', 
                         arrowprops = dict(arrowstyle = '->', connectionstyle = 'arc3,rad=0'))
            i += 1
            
        #f = "roc/all.roc.{}{}.target.mat".format(prefix, i)
        #contents = sio.loadmat(f)
        #plt.plot(contents['fpr'][0], contents['tpr'][0], label=f)
        #i = 0
        #for t in contents['thresholds'][0]:
        #    plt.annotate(t, 
        #                 xy=(contents['fpr'][0][i], contents['tpr'][0][i]), 
        #                 xytext=(-20, 20), 
        #                 textcoords = 'offset points', 
        #                 ha = 'right', 
        #                 va = 'bottom', 
        #                 arrowprops = dict(arrowstyle = '->', connectionstyle = 'arc3,rad=0'))
        #    i += 1
        
    plt.title("ROC")
    plt.xlabel("False Positive Rate")
    plt.ylabel("True Positive Rate")
    plt.legend()
    plt.show()
    
    
    for i in range(1, dbCount + 1):
        f = "roc/all.precall.{}{}.play.mat".format(prefix, i)
        contents = sio.loadmat(f)
        plt.plot(contents['recall'][0], contents['precision'][0], label=f)
        i = 0
        for t in contents['thresholds'][0]:
            plt.annotate(t, 
                         xy=(contents['recall'][0][i], contents['precision'][0][i]), 
                         xytext=(-20, -20), 
                         textcoords = 'offset points', 
                         ha = 'right', 
                         va = 'top', 
                         arrowprops = dict(arrowstyle = '->', connectionstyle = 'arc3,rad=0'))
            i += 1
            
        #f = "roc/all.precall.{}{}.target.mat".format(prefix, i)
        #contents = sio.loadmat(f)
        #plt.plot(contents['recall'][0], contents['precision'][0], label=f)
        #i = 0
        #for t in contents['thresholds'][0]:
        #    plt.annotate(t, 
        #                 xy=(contents['recall'][0][i], contents['precision'][0][i]), 
        #                 xytext=(20, 20), 
        #                 textcoords = 'offset points', 
        #                 ha = 'left', 
        #                 va = 'bottom', 
        #                 arrowprops = dict(arrowstyle = '->', connectionstyle = 'arc3,rad=0'))
        #    i += 1
        
    plt.title("Precision/Recall")
    plt.xlabel("Recall (TP / (TP+FN))")
    plt.ylabel("Precision (TP / (TP + FP))")
    plt.legend()
    plt.show()
