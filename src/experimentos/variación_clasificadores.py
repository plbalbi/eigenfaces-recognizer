import subprocess # call
import pandas as pd # read_table
import matplotlib.pyplot as plt # bar, show
import numpy as np # array
import os # devnull
import sys

v_max = 20
k = 30
nro_folds = 5

precisions = [0] * v_max
recalls = [0] * v_max
f1s = [0] * v_max

for method in ['KNN', 'WEIGHTED']:
    for v in range(1, v_max+1):
        # INIT ######
        print("V = {}    its:".format(v), end="")
        sys.stdout.flush()

        media_precision_global = 0.
        media_recall_global = 0.
        media_f1_global = 0.

        # DATA ######
        for i in range(1, nro_folds+1):
            print(" {}".format(i), end="")
            sys.stdout.flush()

            test_cols = str([2*i-1, 2*i])

            # Sys Calls
            subprocess.call(['python2', 'metnum.py', 'build'])
            subprocess.call(['python', './tools/make_input.py' \
                , '../data/ImagenesCaras/', str(k) \
                ,'-ti', test_cols])
            subprocess.call(['./tp', 'data.in', 'data.out', '-k', str(v)]
                , stdout=open(os.devnull, 'wb'))

            # Reading Output
            prec_table = pd.read_table('metricas/' + method + '_precision.out', delim_whitespace=True, \
              names =['class','precision'])
            rec_table = pd.read_table('metricas/' + method + '_recall.out', delim_whitespace=True, \
              names =['class','recall'])
            f1_table = pd.read_table('metricas/' + method + '_f1.out', delim_whitespace=True, \
              names =['class','f1'])

            # Global Values
            #  Undefined values are mapped to np.nan so 'nanmean' and 'nanvar' can ignore them
            prec_arr_global = np.array([np.nan if e == -1 else e  for e in prec_table['precision']])
            rec_arr_global = np.array([np.nan if e == -1 else e  for e in rec_table['recall']])
            f1_arr_global = np.array([np.nan if e == -1 else e  for e in f1_table['f1']])
            media_precision_global += np.nanmean(prec_arr_global)
            media_recall_global += np.nanmean(rec_arr_global)
            media_f1_global += np.nanmean(f1_arr_global)

        # Transform Summations into Means 
        media_precision_global /= nro_folds
        media_recall_global /= nro_folds
        media_f1_global /= nro_folds

        # RESULTS ######
        precisions[v-1] = media_precision_global
        recalls[v-1] = media_recall_global
        f1s[v-1] = media_f1_global
        print("\n  Precision - Media: {:.2%}".format(media_precision_global))
        print("  Recall - Media: {:.2%}".format(media_recall_global))
        print("  F1 Score - Media: {:.2%}".format(media_f1_global))
        print("")

    #plt.plot(range(1,v_max+1), precisions, label='Precision')
    #plt.plot(range(1,v_max+1), recalls, label='Recall')
    if method == 'KNN':
        plt.plot(range(1,v_max+1), f1s, label='kNN', c='r')
    else:
        plt.plot(range(1,v_max+1), f1s, label='Weighted kNN', c='g')

plt.xlabel('Cantidad de vecinos')
plt.ylabel('F1 Score')
plt.legend(loc=4)
plt.xticks([e  for e in np.arange(v_max+1)  if e % 2 == 0])
plt.grid(axis='y', color='#cccccc', linestyle='dashed')
plt.xlim(0, v_max+1)
plt.ylim(0.5, 1.05)
plt.show()