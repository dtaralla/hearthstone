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

DB_PATH  = "../../build-hearthstone-MinGW_64-Release/hsdatabasegenerator_wMinionScore/release/generated/"
MAT_PATH = "roc/"
ENV_SIZE = 176
TARGET_SIZE = 5
CHARACTERDESC_SIZE = 4
PLAYACTION_SIZE = 1
TARGETEDACTION_SIZE = 1 + TARGET_SIZE
ATKACTION_SIZE = CHARACTERDESC_SIZE + TARGET_SIZE

dbs = ["boardCtrl.db.play", "boardCtrl.db.target", "boardCtrl.db.atk"]#, "aggro.db.play", "aggro.db.target"]
dbsCustom = ["play", "target", "atk"]
n_features = { dbs[0]: ENV_SIZE + PLAYACTION_SIZE, dbs[1]: ENV_SIZE + TARGETEDACTION_SIZE, dbs[2]: ENV_SIZE + ATKACTION_SIZE } #159 and 173 for previous environment() version

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
        elif "atk" in db:
            n = n_features[dbs[2]]
        
        print("{} contains a {}x{} matrix; parsing...".format(db, num_lines, n+1))
        loaded = np.zeros((num_lines, n + 1))
        with open(DB_PATH + db, mode='r') as f:
            reader = csv.reader(f)
            for i, row in enumerate(reader):
                if (i % 50000 == 0):
                    print("Line {}".format(i))
                #if (i > 400000):
                #    break
                loaded[i, :] = np.array(row[0].split(), dtype=np.float64)
        
        # Replace zeroes by random, small uncertain values (else we would have 3 classes)
        #mask = loaded[:, -1] == 0
        #loaded[mask, -1] = random_state.normal(0, 0.01, mask.sum())
        #mask = 0
        
        # Remove actions which did nothing bad nor good
        #loaded = loaded[loaded[:, -1] != 0, :]
        
        
        #if "target" in db:
        #    loaded[loaded[:, -1] >= 0, -1] = 1
        #    loaded[loaded[:, -1] < 0, -1] = -1
        #else:
        loaded = loaded[loaded[:, -1] != 0, :]
        loaded[loaded[:, -1] > 0, -1] = 1
        loaded[loaded[:, -1] < 0, -1] = -1
            
        print("Pruned sample space: {}.".format(loaded.shape[0]))
        
        return loaded
    else:
        raise ValueError("Missing {}, aborting".format(DB_PATH + db))

def loadFinalClassifiedDB(db, genTest=False, random_state=0):
    random_state = check_random_state(random_state)
    
    if (os.path.exists(DB_PATH + db)):
        n = n_features[dbs[1]]
        if "play" in db:
            n = n_features[dbs[0]]
        elif "atk" in db:
            n = n_features[dbs[2]]
        
        nbSamples = 800000
        byClass = nbSamples / 2
        genTestStart = 4550000 #worst was gen training set of target, which went up to 4,500,000 !
        
        loaded = np.zeros((nbSamples, n + 1))
        
        j = 0
        nbPos = 0
        nbNeg = 0
        #if "target" in db:
        #    genTestStart = 4250000 #gen training set of target went up to 4,200,000 !
        #    if (genTest):
        #        nbSamples = 230000
        #    else:
        #        nbSamples = 400000
        #    byClass = nbSamples / 2
        #    loaded = np.zeros((nbSamples, n + 1))
                
        with open(DB_PATH + db, mode='r') as f:
            reader = csv.reader(f)
            for i, row in enumerate(reader):
                if (i % 50000 == 0):
                    print("Line {}, j is {}".format(i, j))
                if (genTest and i < genTestStart):
                    continue
                if (j >= nbSamples):
                    break
                loaded[j, :] = np.array(row[0].split(), dtype=np.float64)
                if (loaded[j, -1] == 0):
                    continue
                    
                if (loaded[j, -1] > 0):
                    if (nbPos < byClass):
                        j += 1
                        nbPos += 1
                elif (nbNeg < byClass):
                    j += 1
                    nbNeg += 1
        
        mask = loaded[:, -1] >= 0
        loaded[mask, -1] = 1
        loaded[mask == False, -1] = -1
        
        sum = mask.sum()
        print("There are {} positive samples and {} negative ones".format(sum, nbSamples - sum))
        print("nbNeg is {} and nbPos is {}".format(nbNeg, nbPos))
        
        return loaded
    else:
        raise ValueError("Missing {}, aborting".format(DB_PATH + db))

def trainFinalClassifier(db):
    clf = ExtraTreesClassifier(n_estimators=100, random_state=0, verbose=100, n_jobs=-1)
    print("Loading training set...")
    loaded = joblib.load(db + ".dump")
    print("Fitting...")
    clf.fit(loaded[:, 0:-1], loaded[:, -1])
    loaded = 0
    print("Saving...")
    if (os.path.exists("clfs/") == False):
        os.mkdir("clfs")
    clf.verbose = 0
    joblib.dump(clf, "clfs/" + db)
        
def roc_precision(db, usecols=None, test="unnamed", random_state=0, show_plots=False):
    if (os.path.exists(MAT_PATH) == False):
        os.mkdir(MAT_PATH)
        
    random_state = check_random_state(random_state)
    
    clf = 0
    if (not os.path.exists("clfs/" + db)):
        clf = ExtraTreesClassifier(n_estimators=100, random_state=0, n_jobs=-1)
        print("Loading training set...")
        loaded = loadClassifiedDB(db + ".train.csv", random_state=random_state, usecols=usecols)#, skipheader=234100)
        print("Fitting...")
        clf.fit(loaded[:, 0:-1], loaded[:, -1])
        loaded = 0
        print("Saving...")
        if (os.path.exists("clfs/") == False):
            os.mkdir("clfs")
        clf.verbose = 0
        joblib.dump(clf, "clfs/" + db)
    else:
        print("Loading {}...".format(db))
        clf = joblib.load("clfs/" + db)
        
    classes = clf.classes_
    
    print("Loading test set...")
    loaded = loadClassifiedDB(db + ".csv", random_state=random_state, usecols=usecols)#, skipheader=232800)
    y_true = loaded[:, -1]

    
    print("Predict proba...")
    y_score = clf.predict_proba(loaded[:, 0:-1])
    loaded = 0
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
        
def roc_precision_final(db, fac=1):
    if (os.path.exists(MAT_PATH) == False):
        os.mkdir(MAT_PATH)
        
    random_state = check_random_state(0)
    
    print("Loading {}...".format(db))
    clf = joblib.load("clfs/" + db)
        
    classes = clf.classes_
    
    print("Loading test set...")
    loaded = joblib.load("testSet/" + db)
    y_true = loaded[:, -1]

    
    print("Predict proba...")
    y_score = clf.predict_proba(loaded[:, 0:-1])
    loaded = 0
    clf = 0
    y_score = y_score[:, classes == 1] * fac
    
    print("ROC...")
    if (fac != 1):
        db = db + str(fac)
    fpr, tpr, thresholds = roc_curve(y_true, y_score)
    sio.savemat(MAT_PATH + 'final.roc.' + db + '.mat', {'fpr':fpr, 'tpr':tpr, 'thresholds':thresholds})
    
    print("Precision/Recall...")
    precision, recall, thresholds = precision_recall_curve(y_true, y_score)
    sio.savemat(MAT_PATH + 'final.precall.' + db + '.mat', {'precision':precision, 'recall':recall, 'thresholds':thresholds})
    
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
    clf.verbose = 0
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
            clf = ExtraTreesClassifier(n_estimators=100, random_state=0, n_jobs=-1, verbose=100)
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
