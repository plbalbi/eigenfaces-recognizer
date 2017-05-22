import subprocess # call
import pandas as pd # read_table
import matplotlib.pyplot as plt # bar, show
import numpy as np # array

# INIT ######
nro_folds = 5
nro_sujetos = 8

media_precisiones = [0] * nro_sujetos
media_precision_global = 0
media_recall_global = 0
media_f1_global = 0
varianza_precision_global = 0
varianza_recall_global = 0
varianza_f1_global = 0


# DATA ######
for i in [1, 2, 3, 4, 5]:
    test_cols = str([2*i-1, 2*i])

    # Sys Calls
    subprocess.call(['python2', 'metnum.py', 'build'])
    subprocess.call(['python', './tools/make_input.py' \
        , '../data/ImagenesCaras/', '3' \
        , '-s', str(nro_sujetos), '-ti', test_cols])
    subprocess.call(['./tp', 'data.in', 'data.out'])
    subprocess.call(['rm', 'data.in', 'data.out'])

    # Reading Output
    prec_table = pd.read_table('metricas/KNN_accuracy.out', delim_whitespace=True, \
      names =['class','precision'])
    rec_table = pd.read_table('metricas/KNN_recall.out', delim_whitespace=True, \
      names =['class','recall'])
    f1_table = pd.read_table('metricas/KNN_f1.out', delim_whitespace=True, \
      names =['class','f1'])

    # Updating Values
    media_precisiones += prec_table['precision']
    media_precision_global += prec_table['precision'].mean()
    varianza_precision_global += prec_table['precision'].var()
    media_recall_global += rec_table['recall'].mean()
    varianza_recall_global += rec_table['recall'].var()
    media_f1_global += f1_table['f1'].mean()
    varianza_f1_global += f1_table['f1'].var()

media_precisiones /= nro_folds
media_precision_global /= nro_folds
varianza_precision_global /= nro_folds
media_recall_global /= nro_folds
varianza_recall_global /= nro_folds
media_f1_global /= nro_folds
varianza_f1_global /= nro_folds


# PLOT ######
plt.bar(prec_table['class'], prec_table['precision'], width=0.5)
plt.xlabel('Clases')
plt.ylabel('Precision')
plt.show()