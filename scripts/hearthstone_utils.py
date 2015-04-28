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
from itertools import izip
import numpy as np
from sklearn.externals import joblib
from sklearn.ensemble import ExtraTreesRegressor
from sklearn.ensemble import ExtraTreesClassifier
from sklearn.ensemble import RandomForestRegressor
from sklearn.utils import check_random_state
from sklearn.metrics import roc_curve
from sklearn.metrics import precision_recall_curve
from matplotlib import pyplot as plt
import scipy.io as sio
import csv

DB_PATH  = "../../build-hearthstone-MinGW_64-Release/hsdatabasegenerator/release/generated/"
MAT_PATH = "roc/"

dbs = ["db.play", "db.target"]
dbsCustom = ["play", "target"]
n_features = { dbs[0]: 177, dbs[1]: 181 } #159 and 173 for previous environment() version

def loadDB(db, usecols=None, skipheader=0, skipfooter=0):
    if (os.path.exists(DB_PATH + db)):
        return np.genfromtxt(DB_PATH + db, skip_header=skipheader, skip_footer=skipfooter, usecols=usecols)

    raise ValueError("Missing {}, aborting".format(DB_PATH + db))

def loadClassifiedDB(db, usecols=None, skipheader=0, skipfooter=0, random_state=0):
    random_state = check_random_state(random_state)
    
    if (os.path.exists(DB_PATH + db)):
        with open(DB_PATH + db, mode='r') as f:
            num_lines = sum(1 for line in f)
        
        n = n_features[dbs[1]]
        if "play" in db:
            n = n_features[dbs[0]]
        
        print("{} contains a {}x{} matrix; parsing...".format(db, num_lines, n))
        loaded = np.zeros((num_lines, n + 1))
        with open(DB_PATH + db, mode='r') as f:
            reader = csv.reader(f)
            for i, row in enumerate(reader):
                loaded[i, :] = np.array(row[0].split(), dtype=np.float64)
        
        # Replace zeroes by random, small uncertain values (else we would have 3 classes)
        mask = loaded[:, -1] == 0
        loaded[mask, -1] = random_state.normal(0, 0.01, mask.sum())
        mask = 0
        loaded[loaded[:, -1] >= 0, -1] = 1
        loaded[loaded[:, -1] < 0, -1] = -1
        
        return loaded
    else:
        raise ValueError("Missing {}, aborting".format(DB_PATH + db))


def roc_precision(db, usecols=None, test="unnamed", random_state=0, show_plots=False):
    if (os.path.exists(MAT_PATH) == False):
        os.mkdir(MAT_PATH)
        
    random_state = check_random_state(random_state)
    
    print("Loading training set...")    
    loaded = loadClassifiedDB(db + ".train.csv", random_state=random_state, usecols=usecols)#, skipheader=234100)
    clf = ExtraTreesClassifier(n_estimators=100, random_state=random_state, n_jobs=-1)
    
    print("Fitting...")
    clf.fit(loaded[:, 0:-1], loaded[:, -1])
    classes = clf.classes_
    loaded = 0
    
    print("Loading test set...")
    loaded = loadClassifiedDB(db + ".csv", random_state=random_state, usecols=usecols)#, skipheader=232800)
    y_true = loaded[:, -1]

    
    print("Predict proba...")
    y_score = clf.predict_proba(loaded[:, 0:-1])
    loaded = loaded[:, -1]
    clf = 0
    y_score = y_score[:, classes == 1]
    
    print("ROC...")
    fpr, tpr, thresholds = roc_curve(y_true, y_score)
    sio.savemat(MAT_PATH + test + '.roc.' + db + '.mat', {'fpr':fpr, 'tpr':tpr, 'thresholds':thresholds})
    
    if (show_plots):
        plt.plot(fpr, tpr)
        plt.title("ROC curve")
        plt.xlabel("False Positive Rate")
        plt.ylabel("True Positive Rate")
        for i in range(0, thresholds.size):
            plt.annotate(str(thresholds[i]), xy=(fpr[i], tpr[i]), xytext=(10,10), textcoords='offset points', arrowprops=dict(facecolor='black', shrink=0.025))
        plt.show()
    
    print("Precision/Recall...")
    precision, recall, thresholds = precision_recall_curve(y_true, y_score)
    sio.savemat(MAT_PATH + test + '.precall.' + db + '.mat', {'precision':precision, 'recall':recall, 'thresholds':thresholds})
    
    if (show_plots):
        plt.plot(recall, precision)
        plt.title("Precision/Recall")
        plt.xlabel("Recall (TP / (TP+FN))")
        plt.ylabel("Precision (TP / (TP + FP))")
        for i in range(0, thresholds.size):
            plt.annotate(str(thresholds[i]), xy=(recall[i], precision[i]), xytext=(10,10), textcoords='offset points', arrowprops=dict(facecolor='black', shrink=0.025))
        plt.show()
    
def saveTrainedClassifier(db, clf):
    print("Loading data for classifier on " + db)
    loaded = loadClassifiedDB(db + ".train.csv", random_state=0)

    print("X_train size: {}.".format(loaded[:, 0:-1].shape))
    print("y_train size: {}.".format(loaded[:, -1].shape))

    print("Fitting classifier...")
    clf.fit(loaded[:, 0:-1], loaded[:, -1])
    loaded = 0

    print("Saving classifier...")
    if (os.path.exists("clfs/") == False):
        os.mkdir("clfs")
    joblib.dump(clf, "clfs/" + db)

# Give insight about whether or not the score is really better than a randomized input/output classifier
# Did the classifier find a *real* input-output relation?
def pValue(X_train, y_train, X_test, y_test, clf, N = 1000, seed = None):
    random_state = seed
    random_state = check_random_state(random_state)
    score_original = clf.score(X_test, y_test)
    
    print("Score is {}".format(clf.score(X_test, y_test)))
    nbTimesRandomWasBetter = 0
    for i in range(N):
        y_trainR = random_state.permutation(y_train)
        clf.fit(X_train, y_trainR)
        score = clf.score(X_test, y_test)
        if (score >= score_original):
            nbTimesRandomWasBetter += 1
        print("pValue step {}/{} done: {} were better (last score was {})".format(i, N, nbTimesRandomWasBetter, score))
            
    print("pValue is {}".format(nbTimesRandomWasBetter / N))
        
def trainClassifiersAndSave(computeScore=False):
    for db in dbs:
        if (not os.path.exists("clfs/" + db)):
            clf = ExtraTreesClassifier(n_estimators=100, random_state=4, n_jobs=-1, verbose=50)
            saveTrainedClassifier(db, clf)
        elif (computeScore):
            clf = joblib.load("clfs/" + db)

        if (computeScore):
            print("Loading test data...")
            loaded = loadDB(db + ".csv")
            X_test = loaded[:, 0:-1]
            y_test = loaded[:, -1]

            print("Normalized score is {}".format(clf.score(X_test, y_test)))
            X_test = y_test = 0
        
def trainRegressorsAndSave(computeScore=False):
    for db in dbs:
        if (not os.path.exists("clfs/" + db)):
            clf = ExtraTreesRegressor(n_estimators=500, random_state=1, n_jobs=-1)
            saveTrainedClassifier(db, clf)
        elif (computeScore):
            clf = joblib.load("clfs/" + db)

        if (computeScore):
            print("Loading test data...")
            loaded = loadDB(db + ".csv")
            X_test = loaded[:, 0:-1]
            y_test = loaded[:, -1]

            print("Normalized score is {}".format(clf.score(X_test, y_test)))
            X_test = y_test = 0

def normalizeScore(db, score):
    fname = DB_PATH + db + ".csv"
    if (not os.path.exists(fname)):
        raise ValueError("Missing {}, aborting".format(fname))
    
    y_test = np.genfromtxt(fname, usecols=(-1))
    return 1 - score / ((y_test - y_test.mean()) ** 2).sum()


def scoreClassifiers_deprecated(step=50000):
    for db in dbs:
        f = open(DB_PATH + db + ".csv", mode='r')
        num_lines = sum(1 for line in f)
        f.close()

        if (num_lines < step):
            step = num_lines
            
        skipheader = range(0, num_lines - step, step)
        skipheader.append(num_lines - step)
        skipfooter = range(num_lines - step, 0, -step)
        skipfooter.append(0)

        print("\nLoading classifier {}...".format(db))
        clf = joblib.load("clfs/" + db)

        error = 0
        for sh, sf in izip(skipheader, skipfooter):
            X_test = y_test = 0

            print("Loading test data, from line #{} to line #{}...".format(sh, num_lines - sf))
            loaded = loadDB(db + ".csv", skipheader=sh, skipfooter=sf)
            X_test = loaded[:, 0:-1]
            y_test = loaded[:, -1]

            print("Updating error...")
            error += ((y_test - clf.predict(X_test))**2).sum()

            print("Error is now {}".format(error))

        print("Normalized score is {}".format(1 - normalizeScore(db + ".csv", error)))

def scoreRegressors():
    for db in dbs:
        print("\nLoading classifier {}...".format(db))
        clf = joblib.load("clfs/" + db)

        print("Loading test data...")
        loaded = loadDB(db + ".csv")
        X_test = loaded[:, 0:-1]
        y_test = loaded[:, -1]

        print("Normalized score is {}".format(clf.score(X_test, y_test)))
