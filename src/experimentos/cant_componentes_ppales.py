#TODO: Hacer que el eje x sea de enteros, y setear el rango del eje y en [0..1]

import subprocess # call
import pandas as pd # read_table
import matplotlib.pyplot as plt # bar, show
import numpy as np # array
import os # devnull
import sys

k_max = 2
nro_folds = 5

precisions = [0] * k_max
recalls = [0] * k_max

for k in range(1, k_max+1):
    # INIT ######
    print("K = {}    its:".format(k), end="")
    sys.stdout.flush()

    media_precision_global = 0.
    media_recall_global = 0.

    # DATA ######
    for i in [1, 2, 3, 4, 5]:
        print(" {}".format(i), end="")
        sys.stdout.flush()

        test_cols = str([2*i-1, 2*i])

        # Sys Calls
        subprocess.call(['python2', 'metnum.py', 'build'])
        subprocess.call(['python', './tools/make_input.py' \
            , '../data/ImagenesCaras/', str(k) \
            ,'-ti', test_cols])
        subprocess.call(['./tp', 'data.in', 'data.out'] \
            , stdout=open(os.devnull, 'wb'))

        # Reading Output
        prec_table = pd.read_table('metricas/KNN_precision.out', delim_whitespace=True, \
          names =['class','precision'])
        rec_table = pd.read_table('metricas/KNN_recall.out', delim_whitespace=True, \
          names =['class','recall'])

        # Global Values
        #  Undefined values are mapped to np.nan so 'nanmean' and 'nanvar' can ignore them
        prec_arr_global = np.array([np.nan if e == -1 else e  for e in prec_table['precision']])
        rec_arr_global = np.array([np.nan if e == -1 else e  for e in rec_table['recall']])
        media_precision_global += np.nanmean(prec_arr_global)
        media_recall_global += np.nanmean(rec_arr_global)

    # Transform Summations into Means 
    media_precision_global /= nro_folds
    media_recall_global /= nro_folds

    # RESULTS ######
    precisions[k-1] = media_precision_global
    recalls[k-1] = media_recall_global
    print("\n  Precision - Media: {:.2%}".format(media_precision_global))
    print("  Recall - Media: {:.2%}".format(media_recall_global))
    print("")

plt.plot(range(1,k_max+1), precisions)
plt.plot(range(1,k_max+1), recalls)
plt.xlabel('Cantidad de Componentes Principales')
plt.ylabel('Medición')
plt.legend(['Precision', 'Recall'], loc=4)
plt.show()