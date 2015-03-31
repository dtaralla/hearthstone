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
from sklearn.utils import check_random_state
import scipy.io as sio
import hearthstone_utils as hu
  
if __name__ == "__main__":
    useCols = np.arange(hu.n_features[hu.dbs[1]] + 1)
    
    
    # ALL FEATURES
    random_state = check_random_state(0)
    hu.roc_precision(hu.dbs[1], random_state=random_state, test="all", show_plots=True)
    
    
    # Not taking silence/enchant features for board minions
    random_state = check_random_state(0)
    mask = np.ones_like(useCols, dtype=bool)
    for i in range(0, 14):
        offset = i * 6
        mask[offset + 4] = mask[offset + 5] = False
    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_silence_enchant")
    
    
    # Not taking silence/enchant/can_attack features for board minions
    random_state = check_random_state(0)
    useCols = np.arange(hu.n_features[hu.dbs[1]])
    mask = np.ones_like(useCols, dtype=bool)
    for i in range(0, 14):
        offset = i * 6
        mask[offset + 1] = mask[offset + 4] = mask[offset + 5] = False
    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_silence_enchant_canattack")
    
    
    # Not taking any feature relative to players' mana
    random_state = check_random_state(0)
    mask = np.ones_like(useCols, dtype=bool)
    mask[155] = mask[156] = mask[157] = False
    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_manainfo")
    
    
    # Not taking enemy player max mana
    random_state = check_random_state(0)
    useCols = np.arange(hu.n_features[hu.dbs[1]])
    mask = np.ones_like(useCols, dtype=bool)
    mask[157] = False
    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_enemymana")
    
    
    # Not taking minions on board count features
    random_state = check_random_state(0)
    useCols = np.arange(hu.n_features[hu.dbs[1]])
    mask = np.ones_like(useCols, dtype=bool)
    mask[151] = mask[152] = False
    hu.roc_precision(hu.dbs[1], random_state=random_state, usecols=useCols[mask], test="no_minioncounts")
