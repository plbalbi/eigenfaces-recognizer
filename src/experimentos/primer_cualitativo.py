import subprocess # call
import pandas as pd # read_table
import matplotlib.pyplot as plt # bar, show
import numpy as np # array

# INIT ######
nro_folds = 5
nro_sujetos = 8
k = 3

media_precisiones = np.array([0.] * nro_sujetos)
count_precisiones = np.array([0] * nro_sujetos)
media_recalls = np.array([0.] * nro_sujetos)
count_recalls = np.array([0] * nro_sujetos)
media_precision_global = 0.
media_recall_global = 0.
media_f1_global = 0.
varianza_precision_global = 0.
varianza_recall_global = 0.
varianza_f1_global = 0.


# DATA ######
for i in range(1, nro_folds+1):
    test_cols = str([2*i-1, 2*i])

    # Sys Calls
    subprocess.call(['python2', 'metnum.py', 'build'])
    subprocess.call(['python', './tools/make_input.py' \
        , '../data/ImagenesCaras/', str(k) \
        , '-s', str(nro_sujetos), '-ti', test_cols])
    subprocess.call(['./tp', 'data.in', 'data.out'])
    #subprocess.call(['rm', 'data.in', 'data.out'])

    # Reading Output
    prec_table = pd.read_table('metricas/KNN_precision.out', delim_whitespace=True, \
      names =['class','precision'])
    rec_table = pd.read_table('metricas/KNN_recall.out', delim_whitespace=True, \
      names =['class','recall'])
    f1_table = pd.read_table('metricas/KNN_f1.out', delim_whitespace=True, \
      names =['class','f1'])

    # Global Values
    #  Undefined values are mapped to np.nan so 'nanmean' and 'nanvar' can ignore them
    prec_arr_global = np.array([np.nan if e == -1 else e  for e in prec_table['precision']])
    rec_arr_global = np.array([np.nan if e == -1 else e  for e in rec_table['recall']])
    f1_arr_global = np.array([np.nan if e == -1 else e  for e in f1_table['f1']])

    media_precision_global += np.nanmean(prec_arr_global)
    varianza_precision_global += np.nanvar(prec_arr_global)
    media_recall_global += np.nanmean(rec_arr_global)
    varianza_recall_global += np.nanvar(rec_arr_global)
    media_f1_global += np.nanmean(f1_arr_global)
    varianza_f1_global += np.nanvar(f1_arr_global)

    # Class Values
    #  Undefined values are mapped to 0 so they don't polute the means
    #  Count is kept as a way of knowing how many defined values contibuted to each mean
    #  (count[i] is the number of terms that I added to media[i])
    prec_arr_class = np.array([0. if e == -1 else e  for e in prec_table['precision']])
    prec_count_class = np.array([0 if e == -1 else 1  for e in prec_table['precision']])
    rec_arr_class = np.array([0. if e == -1 else e  for e in rec_table['recall']])
    rec_count_class = np.array([0 if e == -1 else 1  for e in rec_table['recall']])

    media_precisiones += prec_arr_class
    count_precisiones += prec_count_class
    media_recalls += rec_arr_class
    count_recalls += rec_count_class

# Transform Summations into Means
#  If count[i] is 0, el f1 se invalido en todos los folds, con lo cual
#  media[i] debe ser NaN (algo / np.nan = np.nan)
count_precisiones = np.array([np.nan if e == 0 else e  for e in count_precisiones])
count_recalls = np.array([np.nan if e == 0 else e  for e in count_recalls])

media_precisiones /= count_precisiones
media_recalls /= count_recalls
media_precision_global /= nro_folds
varianza_precision_global /= nro_folds
media_recall_global /= nro_folds
varianza_recall_global /= nro_folds
media_f1_global /= nro_folds
varianza_f1_global /= nro_folds


# RESULTS ######
print("\n\nVALORACIoN (Porcentajes)")
print("Precision - Media: {:.2%}".format(media_precision_global))
print("Precision - Varianza: {:.2%}".format(varianza_precision_global))
print("Recall - Media: {:.2%}".format(media_recall_global))
print("Recall - Varianza: {:.2%}".format(varianza_recall_global))
print("F1 - Media: {:.2%}".format(media_f1_global))
print("F1 - Varianza: {:.2%}".format(varianza_recall_global))
print("")

fig, ax = plt.subplots(nrows=1, ncols=1)
ax.yaxis.grid(color='#cccccc', linestyle='dashed')
plt.bar(prec_table['class']-0.16, media_precisiones, width=0.32, zorder=3)
plt.bar(prec_table['class']+0.16, media_recalls, width=0.32, zorder=3)
plt.xlabel('Clases')
plt.ylabel('Medicion')
plt.legend(['Precision', 'Recall'], loc=4)
plt.show()
