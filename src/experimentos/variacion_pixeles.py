import subprocess # call
import os # FNULL
import pandas as pd # read_table
import numpy as np
import matplotlib.pyplot as plt # show
import sys

subprocess.call(['python2', 'metnum.py', 'build'])

FNULL = open(os.devnull, 'w')

# Variables
k = 24
nro_folds = 5

sizes = []
sizes.append((10,8))
sizes.append((31,36))
sizes.append((39,47))
sizes.append((58,70))
sizes.append((69,84))
sizes.append((81,98))
sizes.append((112,92))

precisions = [0] * len(sizes)
recalls = [0] * len(sizes)

for j in range(0, len(sizes)):
    # INIT ######
    size = sizes[j]
    print("Size = {}    ".format(size), end="")
    sys.stdout.flush()

    # Creating images
    print("creacion    ", end="")
    sys.stdout.flush()
    base = '../data/ImagenesCaras_' + str(size[0]) + 'x' + str(size[1])
    subprocess.call(['rm', '-f', '-r', base])
    subprocess.call(['./tools/resizePGM.sh', \
    '../data/ImagenesCaras',str(size[0]),str(size[1])])
    print("its: ", end="")

    # Accumulators
    media_precision_global = 0.
    media_recall_global = 0.

    # DATA ######
    for i in [1, 2, 3, 4, 5]:
        print(" {}".format(i), end="")
        sys.stdout.flush()

        test_cols = str([2*i-1, 2*i])

        # Sys Calls
        subprocess.call(['python', './tools/make_input.py' \
            , base+'/', str(k) \
            ,'-ti', test_cols])
        subprocess.call(['./tp', 'data.in', 'data.out'] \
            , stdout=open(os.devnull, 'wb'))

        # Reading Output
        prec_table = pd.read_table('metricas/VECINO_precision.out', delim_whitespace=True, \
          names =['class','precision'])
        rec_table = pd.read_table('metricas/VECINO_recall.out', delim_whitespace=True, \
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
    precisions[j] = media_precision_global
    recalls[j] = media_recall_global
    print("\n  Precision - Media: {:.2%}".format(media_precision_global))
    print("  Recall - Media: {:.2%}".format(media_recall_global))
    print("")

pixeles = [s[0]*s[1]  for s in sizes] 

plt.plot(pixeles, precisions, label='Precision')
plt.plot(pixeles, recalls, label='Recall')
plt.xlabel('Cantidad de Píxeles')
plt.ylabel('Medición')
plt.legend(loc=4)
plt.ylim(0, 1.05)
plt.grid(axis='y', color='#cccccc', linestyle='dashed')
plt.show()